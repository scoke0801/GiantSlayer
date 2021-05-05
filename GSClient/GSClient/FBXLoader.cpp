#include "stdafx.h"
#include "FbxLoader.h"
#include "Mesh.h"

FbxAMatrix GeometricOffsetTransform(FbxNode* pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

XMFLOAT4X4 FbxMatrixToXmFloat4x4(FbxAMatrix* pfbxmtxSource)
{
	FbxVector4 S = pfbxmtxSource->GetS();
	FbxVector4 R = pfbxmtxSource->GetR();
	FbxVector4 T = pfbxmtxSource->GetT();

	FbxAMatrix fbxmtxTransform = FbxAMatrix(T, R, S);

	XMFLOAT4X4 xmf4x4Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)(*pfbxmtxSource)[i][j];
	}

	XMFLOAT3 xmf3S = XMFLOAT3((float)S.mData[0], (float)S.mData[1], (float)S.mData[2]);
	XMFLOAT3 xmf3R = XMFLOAT3((float)R.mData[0], (float)R.mData[1], (float)R.mData[2]);
	XMFLOAT3 xmf3T = XMFLOAT3((float)T.mData[0], (float)T.mData[1], (float)T.mData[2]);

	XMMATRIX Rx = XMMatrixRotationX(XMConvertToRadians(xmf3R.x));
	XMMATRIX Ry = XMMatrixRotationY(XMConvertToRadians(xmf3R.y));
	XMMATRIX Rz = XMMatrixRotationZ(XMConvertToRadians(xmf3R.z));
	XMMATRIX xmR = XMMatrixMultiply(XMMatrixMultiply(Rx, Ry), Rz);
	XMFLOAT4X4 xmf4x4Multiply;
	XMStoreFloat4x4(&xmf4x4Multiply, XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(xmf3S.x, xmf3S.y, xmf3S.z), xmR), XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z)));

	return(xmf4x4Result);
}

void SetMatrixScale(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxSrcMatrix[i][j] *= pValue;
	}
}

void SetMatrixAdd(FbxAMatrix& fbxmtxDstMatrix, FbxAMatrix& fbxmtxSrcMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxDstMatrix[i][j] += fbxmtxSrcMatrix[i][j];
	}
}

//////////////////////////////////////////////////////////////////////////////////

FbxLoader::FbxLoader()
{

}

FbxLoader::FbxLoader(FbxManager* pfbxSdkManager, char* FileName)
{
	mFbxManager = pfbxSdkManager;

	LoadScene(FileName);

	LoadSkeletonHierarchy(mFbxScene->GetRootNode());
	if (mSkeleton.empty()) hasAnimation = false;

	ExploreFbxHierarchy(mFbxScene->GetRootNode());

	SaveAsFile();

	cout << FileName << " ||| [SKT]:" << mSkeleton.size() << " [PG]:" << triangles.size() << " [VT]:" << vertices.size() << endl;
}

FbxLoader::~FbxLoader()
{

}

void FbxLoader::LoadScene(char* pstrFbxFileName)
{
	int nSDKMajor, nSDKMinor, nSDKRevision;
	FbxManager::GetFileFormatVersion(nSDKMajor, nSDKMinor, nSDKRevision);

	FbxIOSettings* pfbxIOSettings = mFbxManager->GetIOSettings();
	FbxImporter* pfbxImporter = FbxImporter::Create(mFbxManager, " ");
	bool bImportStatus = pfbxImporter->Initialize(pstrFbxFileName, -1, pfbxIOSettings);

	int nFileFormatMajor, nFileFormatMinor, nFileFormatRevision;
	pfbxImporter->GetFileVersion(nFileFormatMajor, nFileFormatMinor, nFileFormatRevision);

	mFbxScene = FbxScene::Create(mFbxManager, " ");
	bool bStatus = pfbxImporter->Import(mFbxScene);

	FbxGeometryConverter fbxGeomConverter(mFbxManager);
	fbxGeomConverter.Triangulate(mFbxScene, true);

	FbxSystemUnit fbxSceneSystemUnit = mFbxScene->GetGlobalSettings().GetSystemUnit();
	if (fbxSceneSystemUnit.GetScaleFactor() != 1.0) FbxSystemUnit::cm.ConvertScene(mFbxScene);

	pfbxImporter->Destroy();
}

void FbxLoader::LoadSkeletonHierarchy(FbxNode* pNode)
{
	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		FbxNode* curNode = pNode->GetChild(i);
		LoadSkeletonRecursively(curNode, 0, 0, -1);
	}
}

void FbxLoader::LoadSkeletonRecursively(FbxNode* pNode, int inDepth, int myIndex, int inParentIndex)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton))
	{
		Joint currJoint;
		currJoint.parentIndex = inParentIndex;
		currJoint.name = pNode->GetName();
		mSkeleton.push_back(currJoint);
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadSkeletonRecursively(pNode->GetChild(i), inDepth + 1, mSkeleton.size(), myIndex);
	}
}

void FbxLoader::LoadControlPoints(FbxNode* pNode)
{
	FbxMesh* pfbxMesh = pNode->GetMesh();
	int numCP = pfbxMesh->GetControlPointsCount();
	for (int i = 0; i < numCP; ++i)
	{
		ControlPoint tempCP;
		XMFLOAT3 currPosition;
		tempCP.pos.x = pfbxMesh->GetControlPointAt(i).mData[0];
		tempCP.pos.y = pfbxMesh->GetControlPointAt(i).mData[1];
		tempCP.pos.z = pfbxMesh->GetControlPointAt(i).mData[2];
		cpoints.push_back(tempCP);
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadControlPoints(pNode->GetChild(i));
	}
}

void FbxLoader::LoadAnimation(FbxNode* pNode)
{

	FbxMesh* pfbxMesh = pNode->GetMesh();

	FbxAMatrix fbxmtxGeometryOffset = GeometricOffsetTransform(pfbxMesh->GetNode());

	int numDF = pfbxMesh->GetDeformerCount();
	for (int i = 0; i < numDF; i++) {
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(pfbxMesh->GetDeformer(i, FbxDeformer::eSkin));
		if (!currSkin) continue;

		int numCS = currSkin->GetClusterCount();
		for (int j = 0; j < numCS; j++) {
			FbxCluster* currCluster = currSkin->GetCluster(j);

			FbxAMatrix fbxTransformMTX;
			currCluster->GetTransformMatrix(fbxTransformMTX);
			FbxAMatrix fbxTransformLinkMTX;
			currCluster->GetTransformLinkMatrix(fbxTransformLinkMTX);
			FbxAMatrix fbxBindPoseInvMTX = fbxmtxGeometryOffset * fbxTransformMTX * fbxTransformLinkMTX.Inverse();
			mSkeleton[j].mGlobalBindpose = fbxBindPoseInvMTX;

			int numIDX = currCluster->GetControlPointIndicesCount();
			for (int k = 0; k < numIDX; k++) {
				BlendingInfo tempBlendinfo;
				tempBlendinfo.index = j;
				tempBlendinfo.weight = currCluster->GetControlPointWeights()[k];
				cpoints[currCluster->GetControlPointIndices()[k]].blendInfo.push_back(tempBlendinfo);
			}

			FbxAnimStack* pCurAnimStack = mFbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxString curAnimName = pCurAnimStack->GetName();
			FbxTakeInfo* pTakeInfo = mFbxScene->GetTakeInfo(curAnimName);
			FbxTime start = pTakeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = pTakeInfo->mLocalTimeSpan.GetStop();

			animLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			animName = curAnimName;

			Keyframe** currAnim = &mSkeleton[j].mAnimation;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); i++)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe();
				(*currAnim)->nFrame = i;
				FbxAMatrix currentTransformOffset = pNode->EvaluateGlobalTransform(currTime) * fbxmtxGeometryOffset;
				(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim = &((*currAnim)->mNext);
			}
		}
	}

	BlendingInfo nullBlendinfo;
	nullBlendinfo.index = 0;
	nullBlendinfo.weight = 0;
	for (auto itr = cpoints.begin(); itr != cpoints.end(); itr++)
	{
		for (int i = itr->blendInfo.size(); i <= 4; i++)
		{
			itr->blendInfo.push_back(nullBlendinfo);
		}
	}
}

void FbxLoader::LoadMesh(FbxNode* pNode)
{
	FbxMesh* pfbxMesh = pNode->GetMesh();

	int numPG = pfbxMesh->GetPolygonCount();
	int vertexCounter = 0;
	triangles.reserve(numPG);

	for (int i = 0; i < numPG; i++) {
		TrianglePG tempTriangle;
		triangles.push_back(tempTriangle);

		for (int j = 0; j < 3; j++) {
			int cpIndex = pfbxMesh->GetPolygonVertex(i, j);

			FbxVertex tempVertex;

			// position
			ControlPoint currCP = cpoints[cpIndex];
			tempVertex.pos = currCP.pos;

			// normal
			FbxGeometryElementNormal* pnormal = pfbxMesh->GetElementNormal(0);
			tempVertex.normal = XMFLOAT3(
				pnormal->GetDirectArray().GetAt(cpIndex).mData[0],
				pnormal->GetDirectArray().GetAt(cpIndex).mData[1],
				pnormal->GetDirectArray().GetAt(cpIndex).mData[2]);

			// uv
			int uvindex = pfbxMesh->GetTextureUVIndex(i, j, FbxLayerElement::eTextureDiffuse);
			FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
			FbxLayerElementUV* fbxLayerUV = pfbxMesh->GetLayer(0)->GetUVs();
			fbxUV = fbxLayerUV->GetDirectArray().GetAt(uvindex);
			tempVertex.uv = XMFLOAT2(fbxUV[0], 1.0f - fbxUV[1]);

			// blending info
			for (int k = 0; k < currCP.blendInfo.size(); k++)
			{
				BlendingInfo tempBlendinfo;
				tempBlendinfo.index = currCP.blendInfo[k].index;
				tempBlendinfo.weight = currCP.blendInfo[k].weight;
				tempVertex.blendInfo.push_back(tempBlendinfo);
			}
			tempVertex.SortBlendingInfoByWeight();

			vertices.push_back(tempVertex);
			triangles.back().indices.push_back(vertexCounter);
			vertexCounter++;
		}
	}

	cpoints.clear();
}

void FbxLoader::ExploreFbxHierarchy(FbxNode* pNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		LoadControlPoints(pNode);
		if(hasAnimation == true)
			LoadAnimation(pNode);
		LoadMesh(pNode);
	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) ExploreFbxHierarchy(pNode->GetChild(i));
}

void FbxLoader::SaveAsFile()
{
	ofstream file;
	file.open("FbxExportedFile.bin", ios::out | ios::binary);

	file << "Vertex " << vertices.size() << endl;
	file << "Triangle " << triangles.size() << endl;
	file << "Bone " << mSkeleton.size() << endl;
	file << endl;

	// write vertex info
	file << "[Vertex]" << endl;
	for (int i = 0; i < vertices.size(); i++) {
		// pos + uv + normal
		file << vertices[i].pos.x << " " << vertices[i].pos.y << " " << vertices[i].pos.z << " " <<
				vertices[i].uv.x << " " << vertices[i].uv.y << " " <<
				vertices[i].normal.x << " " << vertices[i].normal.y << " " << vertices[i].normal.z << endl;
		// bindex 1~4 + bweight 1~4
		file << vertices[i].blendInfo[0].index << " " << vertices[i].blendInfo[1].index << " " <<
				vertices[i].blendInfo[2].index << " " << vertices[i].blendInfo[3].index << " " <<
				vertices[i].blendInfo[0].weight << " " << vertices[i].blendInfo[1].weight << " " <<
				vertices[i].blendInfo[2].weight << " " << vertices[i].blendInfo[3].weight << endl;
	}
	file << endl;

	// write tiangle polygon info
	file << "[Triangle]" << endl;
	for (int i = 0; i < triangles.size(); i++) {
		file << triangles[i].indices[0] << " " << triangles[i].indices[1] << " " << triangles[i].indices[2] << endl;
	}

	if (hasAnimation == true) {
		file << "[Bone]" << endl;
		for (int i = 0; i < mSkeleton.size(); i++) {
			file << i << " " << mSkeleton[i].name << " " << mSkeleton[i].parentIndex << endl;

			FbxVector4 translation = mSkeleton[i].mGlobalBindpose.GetT();
			FbxVector4 rotation = mSkeleton[i].mGlobalBindpose.GetR();
			translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
			rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
			mSkeleton[i].mGlobalBindpose.SetT(translation);
			mSkeleton[i].mGlobalBindpose.SetR(rotation);
			FbxMatrix write = mSkeleton[i].mGlobalBindpose;

			file << (float)write.Get(0, 0) << " " << (float)write.Get(0, 1) << " " << (float)write.Get(0, 2) << " " << (float)write.Get(0, 3) << " " <<
				(float)write.Get(1, 0) << " " << (float)write.Get(1, 1) << " " << (float)write.Get(1, 2) << " " << (float)write.Get(1, 3) << " " <<
				(float)write.Get(2, 0) << " " << (float)write.Get(2, 1) << " " << (float)write.Get(2, 2) << " " << (float)write.Get(2, 3) << " " <<
				(float)write.Get(3, 0) << " " << (float)write.Get(3, 1) << " " << (float)write.Get(3, 2) << " " << (float)write.Get(3, 3) << endl;
		}
		file << endl;

		file << "[Animation]" << endl;
		file << "Name " << animName << endl;
		file << "Length " << animLength << endl;
		for (int i = 0; i < mSkeleton.size(); i++) {
			file << i << " " << mSkeleton[i].name << endl;
			
			Keyframe* kframe = mSkeleton[i].mAnimation;
			while (kframe)
			{
				file << kframe->nFrame << " ";

				FbxVector4 translation = kframe->mGlobalTransform.GetT();
				FbxVector4 rotation = kframe->mGlobalTransform.GetR();
				translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
				rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
				kframe->mGlobalTransform.SetT(translation);
				kframe->mGlobalTransform.SetR(rotation);
				FbxMatrix write = kframe->mGlobalTransform;

				file << (float)write.Get(0, 0) << " " << (float)write.Get(0, 1) << " " << (float)write.Get(0, 2) << " " << (float)write.Get(0, 3) << " " <<
						(float)write.Get(1, 0) << " " << (float)write.Get(1, 1) << " " << (float)write.Get(1, 2) << " " << (float)write.Get(1, 3) << " " <<
						(float)write.Get(2, 0) << " " << (float)write.Get(2, 1) << " " << (float)write.Get(2, 2) << " " << (float)write.Get(2, 3) << " " <<
						(float)write.Get(3, 0) << " " << (float)write.Get(3, 1) << " " << (float)write.Get(3, 2) << " " << (float)write.Get(3, 3) << endl;

				kframe = kframe->mNext;
			}
		}
		file << endl;
	}
	file.close();
}
