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

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

FbxLoader::FbxLoader(FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	mFbxManager = pfbxSdkManager;

	LoadScene(pstrFbxFileName);

	LoadFbxHierarchy(mFbxScene->GetRootNode());

	cout << "==================================================" << endl;
	cout << "정점: " << nVertics << "\t폴리곤: " << nPolygons << endl;
	cout << "본 개수: " << mSkeleton.size() << endl;
}

FbxLoader::~FbxLoader()
{

}

void FbxLoader::LoadScene(char* pstrFbxFileName)
{
	FbxIOSettings* pfbxIOSettings = mFbxManager->GetIOSettings();
	FbxImporter* pfbxImporter = FbxImporter::Create(mFbxManager, " ");
	bool bImportStatus = pfbxImporter->Initialize(pstrFbxFileName, -1, pfbxIOSettings);

	mFbxScene = FbxScene::Create(mFbxManager, " ");

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
		LoadMesh(pNode);

		//if (animation != NULL)
		LoadAnimations(pNode);
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
		LoadFbxHierarchy(pNode->GetChild(i));
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

			if (fbxLayerUV != NULL) {
				fbxUV = fbxLayerUV->GetDirectArray().GetAt(uvindex);

				float uv1 = fbxUV[0];
				float uv2 = 1.0f - fbxUV[1];


				mMesh.push_back(
					CTexturedVertex(
						XMFLOAT3(
							pfbxMesh->GetControlPointAt(pvindex).mData[0],
							pfbxMesh->GetControlPointAt(pvindex).mData[2],
							pfbxMesh->GetControlPointAt(pvindex).mData[1]
						),
						XMFLOAT2(
							uv1,
							uv2
						)
					)
				);
			}
			else {
				mMesh.push_back(
					CTexturedVertex(
						XMFLOAT3(
							pfbxMesh->GetControlPointAt(pvindex).mData[0],
							pfbxMesh->GetControlPointAt(pvindex).mData[2],
							pfbxMesh->GetControlPointAt(pvindex).mData[1]
						)
					)
				);
			}
		}
	}

	nVertics += nPolygons * 3;

}

void FbxLoader::LoadControlPoints(FbxNode* pNode)
{

}

void FbxLoader::LoadUV(FbxNode* pNode)
{

}

void FbxLoader::LoadNormal(FbxNode* pNode)
{

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
			for (int i = 0; i < nIndices; ++i)
			{
				BlendingIndexWeightPair blendingIndexWeightPair;
				blendingIndexWeightPair.mBlendingIndex = jindex;
				blendingIndexWeightPair.mBlendingWeight = pfbxCluster->GetControlPointWeights()[i];
				mControlPoints[pfbxCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(blendingIndexWeightPair);
			}

			FbxAnimStack* animStack = mFbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = animStack->GetName();
			mAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = mFbxScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
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
	if (pNode->GetNodeAttribute() && pNode->GetNodeAttribute()->GetAttributeType() && 
		pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currJoint;
		currJoint.mParentIndex = inParentIndex;
		currJoint.mName = pNode->GetName();
		mSkeleton.push_back(currJoint);
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadSkeletonRecursively(pNode->GetChild(i), inDepth + 1, mSkeleton.size(), myIndex);
	}
}
