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

	Optimize();

	SaveAsFile();

	cout << FileName << " ||| [SKT]:" << mSkeleton.size() << " [PG]:" << triangles.size() << " [VT]:" << vertices.size() << endl;
}

FbxLoader::~FbxLoader()
{

}

void FbxLoader::ExploreFbxHierarchy(FbxNode* pNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		LoadControlPoints(pNode);
		if (hasAnimation == true)
			LoadAnimation(pNode);
		LoadMesh(pNode);
	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) ExploreFbxHierarchy(pNode->GetChild(i));
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
	for (int i = 0; i < numCP; i++)
	{
		ControlPoint tempCP;
		XMFLOAT3 currPosition;
		tempCP.pos.x = static_cast<float>(pfbxMesh->GetControlPointAt(i).mData[0]);
		tempCP.pos.y = static_cast<float>(pfbxMesh->GetControlPointAt(i).mData[1]);
		tempCP.pos.z = static_cast<float>(pfbxMesh->GetControlPointAt(i).mData[2]);
		cpoints[i] = tempCP;
	}
}

void FbxLoader::LoadAnimation(FbxNode* pNode)
{

	FbxMesh* pfbxMesh = pNode->GetMesh();

	FbxAMatrix fbxmtxGeometryOffset = GeometricOffsetTransform(pfbxMesh->GetNode());

	// Deformer
	int numDF = pfbxMesh->GetDeformerCount();
	for (int i = 0; i < numDF; i++) {
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(pfbxMesh->GetDeformer(i, FbxDeformer::eSkin));
		if (!currSkin) continue;

		// Cluster
		int numCS = currSkin->GetClusterCount();
		for (int j = 0; j < numCS; j++) {
			FbxCluster* currCluster = currSkin->GetCluster(j);
			string jName = currCluster->GetLink()->GetName();

			int jIndex;
			for (int k = 0; k < mSkeleton.size(); k++) {
				if (mSkeleton[k].name == jName) {
					jIndex = k;
					break;
				}
			}

			FbxAMatrix fbxTransformMTX;
			currCluster->GetTransformMatrix(fbxTransformMTX);
			FbxAMatrix fbxTransformLinkMTX;
			currCluster->GetTransformLinkMatrix(fbxTransformLinkMTX);
			FbxAMatrix fbxGBindPoseInvMTX = fbxTransformLinkMTX.Inverse() * fbxTransformMTX * fbxmtxGeometryOffset;
			
			XMFLOAT4X4 tempXMF4X4;
			for (int row = 0; row < 4; row++) {
				for (int column = 0; column < 4; column++) {
					tempXMF4X4.m[row][column] = fbxGBindPoseInvMTX.Get(row, column);
				}
			}
			mSkeleton[jIndex].offset = tempXMF4X4;

			int numIDX = currCluster->GetControlPointIndicesCount();
			for (int k = 0; k < numIDX; k++) {
				BlendingInfo tempBlendinfo;
				tempBlendinfo.index = j;
				tempBlendinfo.weight = currCluster->GetControlPointWeights()[k];
				cpoints[currCluster->GetControlPointIndices()[k]].blendInfo.push_back(tempBlendinfo);
			}

			//=========================================

			FbxAnimStack* pCurrAnimStack = mFbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = pCurrAnimStack->GetName();
			animName = animStackName.Buffer();

			FbxAnimEvaluator* pSceneEvaluator = mFbxScene->GetAnimationEvaluator();

			for (FbxLongLong k = 0; k < 100; k++) {
				FbxTime currTime;
				currTime.SetFrame(k, FbxTime::eCustom);

				Keyframe tempKey;
				tempKey.frameTime = static_cast<float>(k) / 8.0f;

				FbxAMatrix currentTransformOffset = 
					pSceneEvaluator->GetNodeGlobalTransform(pNode, currTime) * fbxmtxGeometryOffset;
				FbxAMatrix tempMTX = 
					currentTransformOffset.Inverse() * pSceneEvaluator->GetNodeGlobalTransform(currCluster->GetLink(), currTime);
			
				tempKey.translation = { static_cast<float>(tempMTX.GetT().mData[0]), static_cast<float>(tempMTX.GetT().mData[1]), static_cast<float>(tempMTX.GetT().mData[2]) };
				tempKey.scale = { static_cast<float>(tempMTX.GetS().mData[0]), static_cast<float>(tempMTX.GetS().mData[1]), static_cast<float>(tempMTX.GetS().mData[2]) };
				tempKey.rotationquat = { static_cast<float>(tempMTX.GetQ().mData[0]), static_cast<float>(tempMTX.GetQ().mData[1]), static_cast<float>(tempMTX.GetQ().mData[2]), static_cast<float>(tempMTX.GetQ().mData[3]) };
			
				if (k != 0 && mSkeleton[jIndex].mAnimation.back() == tempKey)
					break;

				mSkeleton[jIndex].mAnimation.push_back(tempKey);
			}
		}//Cluster End
	}//Deformer End

	vector<Keyframe> nullKeys;

	for (int i = 0; i < mSkeleton.size(); i++)
	{
		int KeyframeSize = mSkeleton[i].mAnimation.size();
		if (KeyframeSize != 0)
		{
			for (int j = 0; j < KeyframeSize; j++) // 60 frames
			{
				Keyframe tempKey;

				tempKey.frameTime = static_cast<float>(j / 24.0f);
				tempKey.translation = { 0.0f, 0.0f, 0.0f };
				tempKey.scale = { 1.0f, 1.0f, 1.0f };
				tempKey.rotationquat = { 0.0f, 0.0f, 0.0f, 0.0f };
				nullKeys.push_back(tempKey);
			}
			break;
		}
	}

	for (int i = 0; i < mSkeleton.size(); i++)
	{
		if (mSkeleton[i].mAnimation.size() != 0)
			continue;

		mSkeleton[i].mAnimation = nullKeys;
	}

	BlendingInfo nullBlendinfo;
	nullBlendinfo.index = 0;
	nullBlendinfo.weight = 0;
	for (auto itr = cpoints.begin(); itr != cpoints.end(); itr++)
	{
		for (int i = itr->second.blendInfo.size(); i <= 4; i++)
		{
			itr->second.blendInfo.push_back(nullBlendinfo);
		}
	}
}

void FbxLoader::LoadMesh(FbxNode* pNode)
{
	FbxMesh* pfbxMesh = pNode->GetMesh();

	int numPG = pfbxMesh->GetPolygonCount();
	int vertexCounter = 0;
	triangles.reserve(numPG);

	// Polygons(Triangle)
	for (int i = 0; i < numPG; i++) {
		TrianglePG tempTriangle;
		triangles.push_back(tempTriangle);
		
		// Vertices
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

int SearchVertex(const FbxVertex& storage, const vector<FbxVertex>& find)
{
	for (int i = 0; i < find.size(); i++) {
		if (storage == find[i]) 
			return i;
	}

	return -1;
}

void FbxLoader::Optimize()
{
	vector<FbxVertex> tempVertex;

	for (int i = 0; i < triangles.size(); i++) {
		for (int j = 0; j < 3; j++) {
			if (SearchVertex(vertices[i * 3 + j], tempVertex) == -1) {
				tempVertex.push_back(vertices[i * 3 + j]);
			}
		}
	}

	for (int i = 0; i < triangles.size(); i++) {
		for (int j = 0; j < 3; j++) {
			triangles[i].indices[j] = SearchVertex(vertices[i * 3 + j], tempVertex);
		}
	}

	vertices.clear();
	vertices = tempVertex;
	tempVertex.clear();

	//sort(triangles.begin(), triangles.end());
}

void FbxLoader::SaveAsFile()
{
	ofstream file;
	file.open("FbxTest.bin", ios::out | ios::binary);

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
	file << endl;

	if (hasAnimation == true) {
		file << "[Bone]" << endl;
		for (int i = 0; i < mSkeleton.size(); i++) {
			file << i << " " << mSkeleton[i].name << " " << mSkeleton[i].parentIndex << endl;

			file << mSkeleton[i].offset._11 << " " << mSkeleton[i].offset._12 << " " << mSkeleton[i].offset._13 << " " << mSkeleton[i].offset._14 << " " <<
					mSkeleton[i].offset._21 << " " << mSkeleton[i].offset._22 << " " << mSkeleton[i].offset._23 << " " << mSkeleton[i].offset._24 << " " <<
					mSkeleton[i].offset._31 << " " << mSkeleton[i].offset._32 << " " << mSkeleton[i].offset._33 << " " << mSkeleton[i].offset._34 << " " <<
					mSkeleton[i].offset._41 << " " << mSkeleton[i].offset._42 << " " << mSkeleton[i].offset._43 << " " << mSkeleton[i].offset._44 << endl;
		}
		file << endl;

		file << "[Animation]" << endl;
		file << "Name " << animName << endl;
		for (int i = 0; i < mSkeleton.size(); i++) {
			file << i << " " << mSkeleton[i].name << " " << mSkeleton[i].mAnimation.size() << endl;
			
			for (int j = 0; j < mSkeleton[i].mAnimation.size(); j++) {
				file << mSkeleton[i].mAnimation[j].frameTime << " ";
				file << mSkeleton[i].mAnimation[j].translation.x << " " << mSkeleton[i].mAnimation[j].translation.y << " " << mSkeleton[i].mAnimation[j].translation.z << " " <<
						mSkeleton[i].mAnimation[j].scale.x << " " << mSkeleton[i].mAnimation[j].scale.y << " " << mSkeleton[i].mAnimation[j].scale.z << " " <<
						mSkeleton[i].mAnimation[j].rotationquat.x << " " << mSkeleton[i].mAnimation[j].rotationquat.y << " " << mSkeleton[i].mAnimation[j].rotationquat.z << " " << mSkeleton[i].mAnimation[j].rotationquat.w << endl;
			}
		}
		file << endl;
	}
	file.close();
}
