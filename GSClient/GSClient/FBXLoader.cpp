#include "stdafx.h"
#include "FbxLoader.h"

FbxMatrix GetGeometryTransform(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
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
	if (!mSkeleton.empty()) Animation = true;

	ExploreFbxHierarchy(mFbxScene->GetRootNode());

	ExportFbxFile();
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

void FbxLoader::ExploreFbxHierarchy(FbxNode* pNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pNode->GetMesh();

		//===========================================================
		int numCP = pfbxMesh->GetControlPointsCount();
		cout << "ControlPoint Count: " << numCP << endl;

		for (int i = 0; i < numCP; i++) {
			ControlPoint* tempCP = new ControlPoint();
			XMFLOAT3 tempPos;
			tempPos.x = pfbxMesh->GetControlPointAt(i).mData[0];
			tempPos.y = pfbxMesh->GetControlPointAt(i).mData[2];
			tempPos.z = pfbxMesh->GetControlPointAt(i).mData[1];
			tempCP->pos = tempPos;
			mControlPoint.push_back(tempCP);

			//cout << mControlPoint[i]->pos.x << " " << mControlPoint[i]->pos.y << " " << mControlPoint[i]->pos.z << endl;
		}
		//===========================================================

		//===========================================================
		if (Animation == true) {
			int numDF = pfbxMesh->GetDeformerCount();

			FbxMatrix geometryTransform = GetGeometryTransform(pNode);

			for (int i = 0; i < numDF; i++) {
				FbxSkin* tempSkin = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
			
				if (!tempSkin) 
					continue;

				int numCS = tempSkin->GetClusterCount();

				for (int i = 0; i < numCS; i++) {
					FbxCluster* tempCS = tempSkin->GetCluster(i);
					string jointName = tempCS->GetLink()->GetName();
					
					int jointIndex = 0;

					for (int i = 0; i < mSkeleton.size(); i++) {
						if (mSkeleton[i].mName == jointName) {
							jointIndex = i;
							break;
						}
					}

					FbxAMatrix transformMatrix;
					FbxAMatrix transformLinkMatrix;
					FbxAMatrix globalBindposeInverseMatrix;

					tempCS->GetTransformMatrix(transformMatrix);
					tempCS->GetTransformLinkMatrix(transformLinkMatrix);
					globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * 
						FbxAMatrix(pNode->GetGeometricTranslation(FbxNode::eSourcePivot),
									pNode->GetGeometricRotation(FbxNode::eSourcePivot),
									pNode->GetGeometricScaling(FbxNode::eSourcePivot));

					mSkeleton[jointIndex].mGlobalBindposeInverse = globalBindposeInverseMatrix;
					mSkeleton[jointIndex].mNode = tempCS->GetLink();

					int numCPID = tempCS->GetControlPointIndicesCount();

					for (int i = 0; i < numCPID; i++)
					{
						BlendingIndexWeightPair currBlendingIndexWeightPair;
						currBlendingIndexWeightPair.mBlendingIndex = jointIndex;
						currBlendingIndexWeightPair.mBlendingWeight = tempCS->GetControlPointWeights()[i];
						mControlPoint[tempCS->GetControlPointIndices()[i]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
					}

					FbxAnimStack* currAnimStack = mFbxScene->GetSrcObject<FbxAnimStack>(0);
					FbxString animStackName = currAnimStack->GetName();
					mAnimationName = animStackName.Buffer();
					FbxTakeInfo* takeInfo = mFbxScene->GetTakeInfo(animStackName);
					FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
					FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
					mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
					Keyframe** currAnim = &mSkeleton[jointIndex].mAnimation;

					for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
					{
						FbxTime currTime;
						currTime.SetFrame(i, FbxTime::eFrames24);
						*currAnim = new Keyframe();
						(*currAnim)->mFrameNum = i;
						FbxAMatrix currentTransformOffset = pNode->EvaluateGlobalTransform(currTime) *
							FbxAMatrix(pNode->GetGeometricTranslation(FbxNode::eSourcePivot),
								pNode->GetGeometricRotation(FbxNode::eSourcePivot),
								pNode->GetGeometricScaling(FbxNode::eSourcePivot));
						(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * tempCS->GetLink()->EvaluateGlobalTransform(currTime);
						currAnim = &((*currAnim)->mNext);
					}
				}
			}

			BlendingIndexWeightPair currBlendingIndexWeightPair;
			currBlendingIndexWeightPair.mBlendingIndex = 0;
			currBlendingIndexWeightPair.mBlendingWeight = 0;
			for (int i = 0; i < mControlPoint.size(); i++){
				for (int j = mControlPoint[i]->mBlendingInfo.size(); j <= 4; j++){
					mControlPoint[i]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
				}
			}
		}
		//===========================================================

		//===========================================================
		int PGcount = pfbxMesh->GetPolygonCount();

		cout << "Polgon Count: " << PGcount << endl;

		int vertexnum = 0;
		mPolygon.reserve(PGcount);

		for (int i = 0; i < PGcount; i++) {
			TriPolygon temppl;
			mPolygon.push_back(temppl);

			for (int j = 0; j < 3; j++) {
				int CPindex = pfbxMesh->GetPolygonVertex(i, j);
				int UVindex = pfbxMesh->GetTextureUVIndex(i, j, FbxLayerElement::eTextureDiffuse);
				
				ControlPoint* cptemp = mControlPoint[CPindex];

				FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
				FbxLayerElementUV* fbxLayerUV = pfbxMesh->GetLayer(0)->GetUVs();

				fbxUV = fbxLayerUV->GetDirectArray().GetAt(UVindex);

				float uvX = fbxUV[0];
				float uvY = 1.0f - fbxUV[1];

				MeshVertex temp;
				temp.pos = cptemp->pos;
				temp.uv.x = uvX;
				temp.uv.y = uvY;

				//cout << "BlendingInfo Count,, BinfoSize: " << cptemp->mBlendingInfo.size() << endl;
				for (int b = 0; b < cptemp->mBlendingInfo.size(); b++)
				{
					//cout << b << endl;
					VertexBlendingInfo tempBlendInfo;
					tempBlendInfo.mBlendingIndex = cptemp->mBlendingInfo[b].mBlendingIndex;
					tempBlendInfo.mBlendingWeight = cptemp->mBlendingInfo[b].mBlendingWeight;
					temp.mVertexBlendingInfos.push_back(tempBlendInfo);
					//cout << "BInfoSize: " << temp.mVertexBlendingInfos.size() << endl;
				}

				temp.SortBlendingInfoByWeight();

				mVertex.push_back(temp);
				mPolygon.back().mIndices.push_back(vertexnum);

				//cout << temp->uv.x << " " << temp->uv.y << endl;
				vertexnum++;
			}
		}
		cout << "vertexnum: " << vertexnum << endl;
		//===========================================================



	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
		ExploreFbxHierarchy(pNode->GetChild(i));
}

void FbxLoader::ExportFbxFile()
{
	ofstream file;
	file.open("FbxExportedFile.bin", ios::out | ios::binary);

	/*
	int num = 0;

	for (int i = 0; i < 10; i++) {
		num = i;
		file.write((char*)&num, sizeof(num));
	}

	string mtext = "asdewxfsdfesfwererwrwesf23423sfser3";
	string mtext2 = "dhtfydrrsetttttre!grrger";

	file.write((char*)&mtext, sizeof(mtext));
	file.write((char*)&mtext2, sizeof(mtext2));
	
	string writetemp;
	writetemp.clear();

	writetemp = "[VertexCount]: ";
	writetemp += mVertex.size();
	writetemp += "\n";

	file << writetemp;

	file.write((char*)&writetemp, sizeof(writetemp));
	*/

	file << "[Vertex]" << endl;
	file << mVertex.size() << endl;

	for (int i = 0; i < mVertex.size(); i++) {
		file << mVertex[i].pos.x << " " << mVertex[i].pos.y << " " << mVertex[i].pos.z << " " <<
			mVertex[i].uv.x << " " << mVertex[i].uv.y << endl;
	}

	file << "[VertexEnd]" << endl;
	/*
	file << "\n";

	Animation = false;

	if (Animation == true) {
		file << "[JointCount]: " << mSkeleton.size() << "\n";
		for (int i = 0; i < mSkeleton.size(); i++) {
			file << "[" << i << "]: " <<
				"Name " << mSkeleton[i].mName << "ParentIndex " << mSkeleton[i].mParentIndex <<
				" BindPose " << mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 0) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 1) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 2) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 3) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 0) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 1) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 2) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 3) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 0) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 1) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 2) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 3) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 0) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 1) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 2) << "," <<
				mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 3) << "\n";
		}

		file << "\n";

		file << "[Animation]" << "\n";
		for (int i = 0; i < mSkeleton.size(); i++) {
			Keyframe* temp = mSkeleton[i].mAnimation;

			file << "[" << i << "]: " << "Name " << mSkeleton[i].mName << "\n";

			while(temp) {
				FbxVector4 translation = temp->mGlobalTransform.GetT();
				FbxVector4 rotation = temp->mGlobalTransform.GetR();
				translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
				rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
				temp->mGlobalTransform.SetT(translation);
				temp->mGlobalTransform.SetR(rotation);
		
				file << "[FrameNum]: " << temp->mFrameNum - 1 << " " <<
					temp->mGlobalTransform.Transpose().Get(0, 0) << "," <<
					temp->mGlobalTransform.Transpose().Get(0, 1) << "," <<
					temp->mGlobalTransform.Transpose().Get(0, 2) << "," <<
					temp->mGlobalTransform.Transpose().Get(0, 3) << "," <<
					temp->mGlobalTransform.Transpose().Get(1, 0) << "," <<
					temp->mGlobalTransform.Transpose().Get(1, 1) << "," <<
					temp->mGlobalTransform.Transpose().Get(1, 2) << "," <<
					temp->mGlobalTransform.Transpose().Get(1, 3) << "," <<
					temp->mGlobalTransform.Transpose().Get(2, 0) << "," <<
					temp->mGlobalTransform.Transpose().Get(2, 1) << "," <<
					temp->mGlobalTransform.Transpose().Get(2, 2) << "," <<
					temp->mGlobalTransform.Transpose().Get(2, 3) << "," <<
					temp->mGlobalTransform.Transpose().Get(3, 0) << "," <<
					temp->mGlobalTransform.Transpose().Get(3, 1) << "," <<
					temp->mGlobalTransform.Transpose().Get(3, 2) << "," <<
					temp->mGlobalTransform.Transpose().Get(3, 3) << "\n";

				temp = temp->mNext;
			}
		}
	}
	*/
	file.close();
}
