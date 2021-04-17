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

	//LoadSkeletonHierarchy(mFbxScene->GetRootNode());
	//if (!mSkeleton.empty()) Animation = true;

	ExploreFbxHierarchy(mFbxScene->GetRootNode());

	cout << "구성 모델 수: " << mFbxMesh.size() << endl;

	ExportFbxFile();

	cout << "파일 생성 끝" << endl;
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
		
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		//LoadSkeletonRecursively(pNode->GetChild(i), inDepth + 1, mSkeleton.size(), myIndex);
	}
}

void FbxLoader::ExploreFbxHierarchy(FbxNode* pNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pNode->GetMesh();

		FbxSubMesh tempSubMesh;

		// Mesh ===============================================================
		int numCP = pfbxMesh->GetControlPointsCount();
		int numPG = pfbxMesh->GetPolygonCount();

		for (int pindex = 0; pindex < numPG; pindex++) {
			for (int vindex = 0; vindex < 3; vindex++) {
				int pvindex = pfbxMesh->GetPolygonVertex(pindex, vindex);
				int uvindex = pfbxMesh->GetTextureUVIndex(pindex, vindex, FbxLayerElement::eTextureDiffuse);

				CTexturedVertex tempVertex;

				// Position ==========================================================
				tempVertex.m_xmf3Position.x = pfbxMesh->GetControlPointAt(pvindex).mData[0];
				tempVertex.m_xmf3Position.y = pfbxMesh->GetControlPointAt(pvindex).mData[2];
				tempVertex.m_xmf3Position.z = pfbxMesh->GetControlPointAt(pvindex).mData[1];

				// Uv ================================================================
				FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
				FbxLayerElementUV* fbxLayerUV = pfbxMesh->GetLayer(0)->GetUVs();
				fbxUV = fbxLayerUV->GetDirectArray().GetAt(uvindex);

				tempVertex.m_xmf2TexCoord.x = fbxUV[0];
				tempVertex.m_xmf2TexCoord.y = 1.0f - fbxUV[1];

				// Normal ============================================================
				FbxGeometryElementNormal* pnormal = pfbxMesh->GetElementNormal(0);

				tempVertex.m_xmf3Normal.x = pnormal->GetDirectArray().GetAt(pvindex).mData[0];
				tempVertex.m_xmf3Normal.y = pnormal->GetDirectArray().GetAt(pvindex).mData[2];
				tempVertex.m_xmf3Normal.z = pnormal->GetDirectArray().GetAt(pvindex).mData[1];
				
				tempSubMesh.mIndex.push_back(pvindex);
				tempSubMesh.mVertex.push_back(tempVertex);
			}
		}

		// Animation ==========================================================
		int numDC = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);

		FbxVector4* tempDefVertex;
		FbxAMatrix* tempClusterDef;
		double* tempClusterWeight;
		//tempSubMesh.gTransform = XMFLOAT4X4(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);


		//numDC = 0;
		if (numDC != 0) {
			tempDefVertex = new FbxVector4[numCP];
			::memcpy(tempDefVertex, pfbxMesh->GetControlPoints(), numCP * sizeof(FbxVector4));

			tempClusterDef = new FbxAMatrix[numCP];
			::memset(tempClusterDef, 0, numCP * sizeof(FbxAMatrix));

			tempClusterWeight = new double[numCP];
			::memset(tempClusterWeight, 0, numCP * sizeof(double));

			tempSubMesh.mClusterWeight = new double[numCP];
			::memset(tempSubMesh.mClusterWeight, 0, numCP * sizeof(double));
			tempSubMesh.mClusterDef = new XMFLOAT3[numCP];
			::memset(tempSubMesh.mClusterDef, 0, numCP * sizeof(XMFLOAT3));

			FbxAMatrix geometryTransform = GeometricOffsetTransform(pfbxMesh->GetNode());
			tempSubMesh.gTransform = FbxMatrixToXmFloat4x4(&geometryTransform);

			FbxCluster::ELinkMode nClusterMode =
				((FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

			for (int i = 0; i < numDC; i++) {
				FbxSkin* pSkinDef = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
				int nClusters = pSkinDef->GetClusterCount();	// 뼈 개수

				for (int j = 0; j < nClusters; j++) {
					FbxCluster* pCluster = pSkinDef->GetCluster(j);
					if (!pCluster->GetLink()) continue;

					FbxAMatrix clusterDefMatrix;

					if (nClusterMode == FbxCluster::eNormalize) {

						FbxAMatrix transformMatrix;
						pCluster->GetTransformMatrix(transformMatrix);

						FbxAMatrix transformLinkMatrix;
						pCluster->GetTransformLinkMatrix(transformLinkMatrix);

						clusterDefMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;
					}

					int* pnIndices = pCluster->GetControlPointIndices();
					double* pfWeights = pCluster->GetControlPointWeights();

					int nIndices = pCluster->GetControlPointIndicesCount();
					for (int k = 0; k < nIndices; k++) {
						int nIndex = pnIndices[k];
						double fWeight = pfWeights[k];

						if ((nIndex >= numCP) || (fWeight == 0.0)) continue;

						SetMatrixScale(clusterDefMatrix, fWeight);
						SetMatrixAdd(tempClusterDef[nIndex], clusterDefMatrix);
						tempClusterWeight[nIndex] = fWeight;

						FbxVector4 tempFVec4 = tempClusterDef[nIndex].GetT();
						XMFLOAT3 tempXF3 = XMFLOAT3((float)tempFVec4.mData[0], (float)tempFVec4.mData[1], (float)tempFVec4.mData[2]);

						tempSubMesh.mClusterWeight[nIndex] = tempClusterWeight[nIndex];
						tempSubMesh.mClusterDef[nIndex] = tempXF3;
					}
				}
			}

			//for (int i = 0; i < numCP; i++) cout << tempSubMesh.mClusterDef[i].x << endl;
		}

		tempSubMesh.nControlPoint = numCP;
		tempSubMesh.nPolygon = numPG;
		tempSubMesh.nDeformer = numDC;

		cout << "[메쉬 발견] CP:" << numCP << ", PG:" << numPG << ", ID:" << tempSubMesh.mIndex.size() << ", VT:" << tempSubMesh.mVertex.size();
		if (numDC != 0) cout << ", AM:[O]" << endl;
		else cout << ", AM:[X]" << endl;

		mFbxMesh.push_back(tempSubMesh);
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
		ExploreFbxHierarchy(pNode->GetChild(i));
}

void FbxLoader::ExportFbxFile()
{
	ofstream file;
	file.open("FbxExportedFile.bin", ios::out | ios::binary);

	file << "[MeshCount]" << endl;
	file << mFbxMesh.size() << endl;

	for (int i = 0; i < mFbxMesh.size(); i++) {
		file << "[MeshInfo]" << endl;
		file << mFbxMesh[i].nControlPoint << " " << mFbxMesh[i].nPolygon << " " << mFbxMesh[i].nDeformer << endl;

		file << "[Vertex]" << endl;
		for (int j = 0; j < mFbxMesh[i].mVertex.size(); j++) {
			file << mFbxMesh[i].mIndex[j] << " " <<
				mFbxMesh[i].mVertex[j].m_xmf3Position.x << " " <<
				mFbxMesh[i].mVertex[j].m_xmf3Position.y << " " <<
				mFbxMesh[i].mVertex[j].m_xmf3Position.z << " " <<
				mFbxMesh[i].mVertex[j].m_xmf3Normal.x << " " <<
				mFbxMesh[i].mVertex[j].m_xmf3Normal.y << " " <<
				mFbxMesh[i].mVertex[j].m_xmf3Normal.z << " " <<
				mFbxMesh[i].mVertex[j].m_xmf2TexCoord.x << " " <<
				mFbxMesh[i].mVertex[j].m_xmf2TexCoord.y << endl;
		}

		if (mFbxMesh[i].nDeformer != 0) {
			file << "[Animation]" << endl;
			for (int j = 0; j < mFbxMesh[i].nControlPoint; j++) {
				file << mFbxMesh[i].mClusterWeight[j] << " " <<
					mFbxMesh[i].mClusterDef[j].x << " " <<
					mFbxMesh[i].mClusterDef[j].y << " " <<
					mFbxMesh[i].mClusterDef[j].z << endl;
			}
		}
	}

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

	//file << "[Vertex]" << endl;
	//file << mControlPoint.size() << endl;

	//for (int i = 0; i < mControlPoint.size(); i++) {
	//	file << mControlPoint[i]->pos.x << " " << mControlPoint[i]->pos.y << " " << mControlPoint[i]->pos.z << " " <<
	//		mControlPoint[i]->uv.x << " " << mControlPoint[i]->uv.y << endl;
	//}

	//file << "[VertexEnd]" << endl;
	//
	//file << "\n";

	////Animation = false;

	//if (Animation == true) {
	//	file << "[JointCount]: " << mSkeleton.size() << "\n";
	//	for (int i = 0; i < mSkeleton.size(); i++) {
	//		file << "[" << i << "]: " <<
	//			"Name " << mSkeleton[i].mName << "ParentIndex " << mSkeleton[i].mParentIndex <<
	//			" BindPose " << mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 0) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 1) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 2) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(0, 3) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 0) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 1) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 2) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(1, 3) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 0) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 1) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 2) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(2, 3) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 0) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 1) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 2) << "," <<
	//			mSkeleton[i].mGlobalBindposeInverse.Transpose().Get(3, 3) << "\n";
	//	}

	//	file << "\n";

	//	file << "[Animation]" << "\n";
	//	for (int i = 0; i < mSkeleton.size(); i++) {
	//		Keyframe* temp = mSkeleton[i].mAnimation;

	//		file << "[" << i << "]: " << "Name " << mSkeleton[i].mName << "\n";

	//		while(temp) {
	//			FbxVector4 translation = temp->mGlobalTransform.GetT();
	//			FbxVector4 rotation = temp->mGlobalTransform.GetR();
	//			translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
	//			rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
	//			temp->mGlobalTransform.SetT(translation);
	//			temp->mGlobalTransform.SetR(rotation);
	//	
	//			file << "[FrameNum]: " << temp->mFrameNum - 1 << " " <<
	//				temp->mGlobalTransform.Transpose().Get(0, 0) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(0, 1) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(0, 2) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(0, 3) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(1, 0) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(1, 1) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(1, 2) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(1, 3) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(2, 0) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(2, 1) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(2, 2) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(2, 3) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(3, 0) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(3, 1) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(3, 2) << "," <<
	//				temp->mGlobalTransform.Transpose().Get(3, 3) << "\n";

	//			temp = temp->mNext;
	//		}
	//	}
	//}
	
	file.close();
}
