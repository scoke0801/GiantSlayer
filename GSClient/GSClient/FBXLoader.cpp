#include "stdafx.h"
#include "FbxLoader.h"
#include "Mesh.h"

//==========UtilFunctions==========//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

FbxAMatrix GetGeoOffsetTransform(FbxNode* inNode)
{
	if (!inNode)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

XMFLOAT4X4 FbxAMatrixToXMFLOAT4X4(FbxAMatrix* pfbxmtxSource)
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

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

FbxLoader::FbxLoader(FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	mFbxManager = pfbxSdkManager;

	LoadScene(pstrFbxFileName);

	cout << "==================================================" << endl;
	LoadSkeletonHierarchy(mFbxScene->GetRootNode());

	cout << "==================================================" << endl;
	LoadFbxHierarchy(mFbxScene->GetRootNode());

	cout << "==================================================" << endl;
	cout << "정점: " << nVertics << "\t폴리곤: " << nPolygons << "\t제어점: " << size(mControlPoints) << endl;
	cout << "본 개수: " << mSkeleton.size() << endl;

	SaveAsFile();
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

void FbxLoader::LoadFbxHierarchy(FbxNode* pNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if (pNode == NULL) {
		cout << "씬 로드 실패" << endl;
		return;
	}

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		//LoadMesh(pNode);
		LoadControlPoints(pNode);

		//if (animation != NULL)
		LoadAnimations(pNode);
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
		LoadFbxHierarchy(pNode->GetChild(i));
}

void FbxLoader::LoadControlPoints(FbxNode* pNode)
{
	FbxMesh* pfbxMesh = pNode->GetMesh();
	int cpNum = pfbxMesh->GetControlPointsCount();

	for (unsigned int i = 0; i < cpNum; ++i)
	{
		CtrlPoint* cptemp = new CtrlPoint();
		XMFLOAT3 postemp;

		/*
		float uv1 = fbxUV[0];
		float uv2 = 1.0f - fbxUV[1];
		*/

		postemp.x = pfbxMesh->GetControlPointAt(i).mData[0];
		postemp.y = pfbxMesh->GetControlPointAt(i).mData[1];
		postemp.z = pfbxMesh->GetControlPointAt(i).mData[2];

		cptemp->mPosition = postemp;
		mControlPoints[i] = cptemp;
	}
}

void FbxLoader::LoadMesh(FbxNode* pNode)
{
	FbxMesh* pfbxMesh = pNode->GetMesh();

	nPolygons = pfbxMesh->GetPolygonCount();

	for (int pindex = 0; pindex < nPolygons; pindex++) {
		for (int vindex = 0; vindex < 3; vindex++) {
			int pvindex = pfbxMesh->GetPolygonVertex(pindex, vindex);
			int uvindex = pfbxMesh->GetTextureUVIndex(pindex, vindex, FbxLayerElement::eTextureDiffuse);
			
			FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
			FbxLayerElementUV* fbxLayerUV = pfbxMesh->GetLayer(0)->GetUVs();

			CtrlPoint* tempCtrPoint = new CtrlPoint();

			if (fbxLayerUV != NULL) {
				fbxUV = fbxLayerUV->GetDirectArray().GetAt(uvindex);

				float uv1 = fbxUV[0];
				float uv2 = 1.0f - fbxUV[1];

				tempCtrPoint->mPosition = XMFLOAT3(
					pfbxMesh->GetControlPointAt(pvindex).mData[0],
					pfbxMesh->GetControlPointAt(pvindex).mData[2],
					pfbxMesh->GetControlPointAt(pvindex).mData[1]
				);

				tempCtrPoint->mUv = XMFLOAT2(
					uv1,
					uv2
				);

			}
			else {
				tempCtrPoint->mPosition = XMFLOAT3(
					pfbxMesh->GetControlPointAt(pvindex).mData[0],
					pfbxMesh->GetControlPointAt(pvindex).mData[2],
					pfbxMesh->GetControlPointAt(pvindex).mData[1]
				);
			}

			mControlPoints[vindex+(pindex*3)] = tempCtrPoint;
		}
	}

	nVertics += nPolygons * 3;
}

//==========================================================================//

void FbxLoader::LoadAnimations(FbxNode* pNode)
{
	FbxMesh* pfbxMesh = pNode->GetMesh();
	int nDefirner = pfbxMesh->GetDeformerCount();
	FbxAMatrix geometryTransform = GetGeoOffsetTransform(pNode);
	
	for (int dindex = 0; dindex < nDefirner; dindex++) {			// Deformer
		FbxSkin* pfbxSkin = reinterpret_cast<FbxSkin*>(pfbxMesh->GetDeformer(dindex, FbxDeformer::eSkin));

		if(pfbxSkin == NULL)
			continue;

		int nCluster = pfbxSkin->GetClusterCount();
		cout << "[ClusterNum]: " << nCluster << endl;

		for (int cindex = 0; cindex < nCluster; cindex++) {			// Cluster
			FbxCluster* pfbxCluster = pfbxSkin->GetCluster(cindex);
			string jointName = pfbxCluster->GetLink()->GetName();
			
			int jindex = 0;

			for (int i = 0; i < mSkeleton.size(); i++) {
				if (mSkeleton[i].mName == jointName) {
					jindex = i;
					break;
				}
			}

			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			pfbxCluster->GetTransformMatrix(transformMatrix);
			pfbxCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			mSkeleton[jindex].mGlobalBindposeInverse = globalBindposeInverseMatrix;
			mSkeleton[jindex].mNode = pfbxCluster->GetLink();

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			
			cout << "GetControlPointIndicesCount: " << nIndices << endl;

			for (int i = 0; i < nIndices; i++)
			{
				BlendingIndexWeightPair blendingIndexWeightPair;
				blendingIndexWeightPair.mBlendingIndex = jindex;
				blendingIndexWeightPair.mBlendingWeight = pfbxCluster->GetControlPointWeights()[i];
				mControlPoints[pfbxCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(blendingIndexWeightPair);
			}

			FbxAnimStack* animStack = mFbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = animStack->GetName();

			mAnimationName = animStackName.Buffer();
			cout << "AnimationName: " << mAnimationName << endl;



			FbxTakeInfo* takeInfo = mFbxScene->GetTakeInfo(animStackName);

			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			cout << "AnimationLength: " << mAnimationLength << endl;

			Keyframe** currAnim = &mSkeleton[jindex].mAnimation;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe();
				(*currAnim)->mFrameNum = i;

				FbxAMatrix currentTransformOffset = pNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * pfbxCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim = &((*currAnim)->mNext);
			}
		}
	}

	BlendingIndexWeightPair blendingIndexWeightPair;
	blendingIndexWeightPair.mBlendingIndex = 0;
	blendingIndexWeightPair.mBlendingWeight = 0;

	for (auto i = mControlPoints.begin(); i != mControlPoints.end(); ++i)
	{
		for (unsigned int j = i->second->mBlendingInfo.size(); j <= 4; j++)
		{
			i->second->mBlendingInfo.push_back(blendingIndexWeightPair);
		}
	}
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
		currJoint.mParentIndex = inParentIndex;
		currJoint.mName = pNode->GetName();
		cout << "[JointNum]: " << currJoint.mParentIndex << "\t[JointName]: " << currJoint.mName << endl;
		mSkeleton.push_back(currJoint);
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadSkeletonRecursively(pNode->GetChild(i), inDepth + 1, mSkeleton.size(), myIndex);
	}
}

void FbxLoader::SaveAsFile()
{
	ofstream file;
	file.open("FbxExportedFile.bin", ios::out | ios::binary);

	/*
	XMFLOAT2	m_xmf2TexCoord;
	XMFLOAT3	m_xmf3Normal;

	fout.write((const char*)data, data_len); fout.close();

	FbxManager*					mFbxManager = NULL;
	FbxScene*					mFbxScene = NULL;

	int							nControlPoints = 0;
	int							nVertics = 0;
	int							nPolygons = 0;
	float						mAnimationLength;
	string						mAnimationName;

	vector<CTexturedVertex>		mMesh;
	vector<Joint>				mSkeleton;

	unordered_map<unsigned int, CtrlPoint*> mControlPoints;

	unsigned int mBlendingIndex;
	double mBlendingWeight;
	*/

	file << "[ControlPoints] " << nControlPoints << endl;
	file << "[Polygons] " << nPolygons << endl;

	file << "[Mesh]" << endl;

	for (auto point = mControlPoints.begin(); point != mControlPoints.end(); point++) {
		// 제어점 번호: Px Py Pz | UVx UVy
		string line;

		file << point->first << ": ";

		file << point->second->mPosition.x << " " << point->second->mPosition.y << " " << point->second->mPosition.z << " | "
			 << point->second->mUv.x << " " << point->second->mUv.y << endl;
	}

	file << endl;

	file << "[AnimName] " << mAnimationName << endl;
	file << "[AnimLength] " << mAnimationLength << endl << endl;


	file << "[BlendingInfo]" << endl;

	for (auto point = mControlPoints.begin(); point != mControlPoints.end(); point++) {
		file << point->first << ": ";

		for (auto bpoint = point->second->mBlendingInfo.begin();
			bpoint != point->second->mBlendingInfo.end(); bpoint++) {
			file << bpoint->mBlendingIndex << "-" << bpoint->mBlendingWeight << " ";
		}

		file << endl;
	}

	file << endl;

	/*
	std::string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	Keyframe* mAnimation;
	FbxNode* mNode;

	keyf
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	GetT R S Q .
	FbxVector4(double pX, double pY, double pZ, double pW=1.0);
	*/

	file << "[Joint]" << endl;

	for (auto point = mSkeleton.begin(); point != mSkeleton.end(); point++) {
		file << "[JointName]" << point->mName << " [ParentIndex] " << point->mParentIndex << endl;

	}

	for (auto point = mSkeleton.begin(); point != mSkeleton.end(); point++) {
		file << "[AnimFrameNum] " << point->mAnimation->mFrameNum;

		XMFLOAT4X4 temp = FbxAMatrixToXMFLOAT4X4(&point->mAnimation->mGlobalTransform);

		file << " [T] " << temp._11 << " " << temp._12 << " " << temp._13 << " " << temp._14;
		file << " [R] " << temp._21 << " " << temp._22 << " " << temp._23 << " " << temp._24;
		file << " [S] " << temp._31 << " " << temp._32 << " " << temp._33 << " " << temp._34;
		file << " [Q] " << temp._41 << " " << temp._42 << " " << temp._43 << " " << temp._44;

		file << endl;
	}



	cout << "파일 추출 끝" << endl;

	file.close();
}