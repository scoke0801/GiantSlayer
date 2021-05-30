#include "stdafx.h"
#include "FbxObject2.h"
#include "Shader.h"
#include "ShaderHandler.h"
#include "Camera.h"

// FbxUtils
//////////////////////////////////////////////////////////////////////////////////////

XMFLOAT4X4 FbxMatrixToXmFloat4x4Matrix(FbxAMatrix* pfbxmtxSource)
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

FbxAMatrix XmFloat4x4MatrixToFbxMatrix(XMFLOAT4X4& xmf4x4Source)
{
	FbxAMatrix fbxmtxResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxResult[i][j] = xmf4x4Source.m[i][j];
	}
	return(fbxmtxResult);
}

void MatrixScale(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxSrcMatrix[i][j] *= pValue;
	}
}

void MatrixAdd(FbxAMatrix& fbxmtxDstMatrix, FbxAMatrix& fbxmtxSrcMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxDstMatrix[i][j] += fbxmtxSrcMatrix[i][j];
	}
}

FbxAMatrix GetGeometricOffsetTransform(FbxNode* pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CAnimationController::CAnimationController(FbxScene* pfbxScene)
{
	FbxArray<FbxString*> fbxAnimationStackNames;
	pfbxScene->FillAnimStackNameArray(fbxAnimationStackNames);

	m_nAnimationStacks = fbxAnimationStackNames.Size();

	m_ppfbxAnimationStacks = new FbxAnimStack * [m_nAnimationStacks];
	m_pfbxStartTimes = new FbxTime[m_nAnimationStacks];
	m_pfbxStopTimes = new FbxTime[m_nAnimationStacks];
	m_pfbxCurrentTimes = new FbxTime[m_nAnimationStacks];

	cout << "애니메이션 스택 사이즈: " << m_nAnimationStacks << endl;

	for (int i = 0; i < m_nAnimationStacks; i++)
	{
		FbxString* pfbxStackName = fbxAnimationStackNames[i];

		FbxAnimStack* pfbxAnimationStack = pfbxScene->FindMember<FbxAnimStack>(pfbxStackName->Buffer());
		m_ppfbxAnimationStacks[i] = pfbxAnimationStack;

		FbxTakeInfo* pfbxTakeInfo = pfbxScene->GetTakeInfo(*pfbxStackName);
		FbxTime fbxStartTime, fbxStopTime;
		if (pfbxTakeInfo)
		{
			fbxStartTime = pfbxTakeInfo->mLocalTimeSpan.GetStart();
			fbxStopTime = pfbxTakeInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			FbxTimeSpan fbxTimeLineTimeSpan;
			pfbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(fbxTimeLineTimeSpan);
			fbxStartTime = fbxTimeLineTimeSpan.GetStart();
			fbxStopTime = fbxTimeLineTimeSpan.GetStop();
		}

		m_pfbxStartTimes[i] = fbxStartTime;
		m_pfbxStopTimes[i] = fbxStopTime;
		m_pfbxCurrentTimes[i] = FbxTime(0);
	}

	FbxArrayDelete(fbxAnimationStackNames);
}

CAnimationController::~CAnimationController()
{
	if (m_ppfbxAnimationStacks) delete[] m_ppfbxAnimationStacks;
	if (m_pfbxStartTimes) delete[] m_pfbxStartTimes;
	if (m_pfbxStopTimes) delete[] m_pfbxStopTimes;
	if (m_pfbxCurrentTimes) delete[] m_pfbxCurrentTimes;
}

void CAnimationController::SetAnimationStack(FbxScene* pfbxScene, int nAnimationStack)
{
	m_nAnimationStack = nAnimationStack;
	pfbxScene->SetCurrentAnimationStack(m_ppfbxAnimationStacks[nAnimationStack]);
}

void CAnimationController::SetPosition(int nAnimationStack, float fPosition)
{
	m_pfbxCurrentTimes[nAnimationStack].SetSecondDouble(fPosition);
}

void CAnimationController::AdvanceTime(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;

	FbxTime fbxElapsedTime;
	fbxElapsedTime.SetSecondDouble(fTimeElapsed);

	m_pfbxCurrentTimes[m_nAnimationStack] += fbxElapsedTime;
	if (m_pfbxCurrentTimes[m_nAnimationStack] > m_pfbxStopTimes[m_nAnimationStack]) m_pfbxCurrentTimes[m_nAnimationStack] = m_pfbxStartTimes[m_nAnimationStack];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CFbxObject2::CFbxObject2()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 }; 

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CFbxObject2::CFbxObject2(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	//FbxScene* m_pfbxScene = FbxScene::Create(pfbxSdkManager, "");

	LoadScene(pstrFbxFileName, pfbxSdkManager);
	
	if (m_pfbxScene == NULL)
		cout << "fbx 메쉬 로드 실패" << endl;
	else {
		LoadSkeletonHierarchy(m_pfbxScene->GetRootNode());

		if (skeleton.size() != 0) 
			hasAnimation = true;

		ExploreFbxHierarchy(m_pfbxScene->GetRootNode());

		LoadFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pfbxScene->GetRootNode());
		m_pAnimationController = new CAnimationController(m_pfbxScene);
		
		cout << "애니메이션 길이: " << renderinfo[0].animations[0].animBones[1].keyframes.size() << endl;
		
		cout << "fbx 메쉬 로드 성공!" << endl;
	}
}

CFbxObject2::~CFbxObject2()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
	if (m_pAnimationController) delete m_pAnimationController;
}

void CFbxObject2::LoadScene(char* pstrFbxFileName, FbxManager* pfbxSdkManager)
{
	int nSDKMajor, nSDKMinor, nSDKRevision;
	FbxManager::GetFileFormatVersion(nSDKMajor, nSDKMinor, nSDKRevision);

	FbxIOSettings* pfbxIOSettings = pfbxSdkManager->GetIOSettings();
	FbxImporter* pfbxImporter = FbxImporter::Create(pfbxSdkManager, " ");
	bool bImportStatus = pfbxImporter->Initialize(pstrFbxFileName, -1, pfbxIOSettings);

	int nFileFormatMajor, nFileFormatMinor, nFileFormatRevision;
	pfbxImporter->GetFileVersion(nFileFormatMajor, nFileFormatMinor, nFileFormatRevision);

	m_pfbxScene = FbxScene::Create(pfbxSdkManager, " ");
	bool bStatus = pfbxImporter->Import(m_pfbxScene);

	FbxGeometryConverter fbxGeomConverter(pfbxSdkManager);
	fbxGeomConverter.Triangulate(m_pfbxScene, true);

	FbxSystemUnit fbxSceneSystemUnit = m_pfbxScene->GetGlobalSettings().GetSystemUnit();
	if (fbxSceneSystemUnit.GetScaleFactor() != 1.0) FbxSystemUnit::cm.ConvertScene(m_pfbxScene);

	pfbxImporter->Destroy();
}

void CFbxObject2::LoadSkeletonHierarchy(FbxNode* inRootNode)
{

	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1);
	}
}

void CFbxObject2::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Bone2 currBone;
		currBone.pIndex = inParentIndex;
		currBone.name = inNode->GetName();
		skeleton.push_back(currBone);
	}

	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, skeleton.size(), myIndex);
	}
}

void CFbxObject2::ExploreFbxHierarchy(FbxNode* inNode)
{
	if (inNode->GetNodeAttribute())
	{
		switch (inNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:

			MeshInfo tempMesh;
			MeshInfo* ptMesh = &tempMesh;

			FbxMesh* pfbxMesh = inNode->GetMesh();
			tempMesh.numCP = pfbxMesh->GetControlPointsCount();
			tempMesh.numPG = pfbxMesh->GetPolygonCount();
			tempMesh.numDC = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);

			LoadControlPoints(inNode, ptMesh);

			if ((tempMesh.numCP > 0) && 
				(tempMesh.numDC > 0) &&
				(hasAnimation == true)) {
				LoadAnimations(inNode, ptMesh);
			}

			LoadMesh(inNode, ptMesh);

			renderinfo.push_back(tempMesh);
			break;
		}
	}

	for (int i = 0; i < inNode->GetChildCount(); ++i)
		ExploreFbxHierarchy(inNode->GetChild(i));
}

void CFbxObject2::LoadControlPoints(FbxNode* inNode, MeshInfo* tempMesh)
{
	FbxMesh* currMesh = inNode->GetMesh();

	tempMesh->mControlPoint.reserve(tempMesh->numCP);

	for (unsigned int i = 0; i < tempMesh->numCP; ++i)
	{
		ControlPoint2 temp;
		temp.pos.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		temp.pos.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		temp.pos.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		tempMesh->mControlPoint[i] = temp;
	}
}

void CFbxObject2::LoadAnimations(FbxNode* inNode, MeshInfo* tempMesh)
{
	FbxMesh* pfbxMesh = inNode->GetMesh();
	FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());

	FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	int numCS = pfbxSkinDeformer->GetClusterCount();

	Animation2 tempAnim;
	tempAnim.animBones.resize(skeleton.size());

	for (int i = 0; i < numCS; i++)	// numCS == BoneSize
	{
		FbxCluster* curCluster = pfbxSkinDeformer->GetCluster(i);

		string jName = curCluster->GetLink()->GetName();
		int jIndex;
		for (int k = 0; k < skeleton.size(); k++) {
			if (skeleton[k].name == jName) {
				jIndex = k;
				break;
			}
		} // find bone index by name

		FbxAMatrix fbxTransformMTX;
		curCluster->GetTransformMatrix(fbxTransformMTX);
		FbxAMatrix fbxTransformLinkMTX;
		curCluster->GetTransformLinkMatrix(fbxTransformLinkMTX);
		FbxAMatrix fbxGBindPoseInvMTX = fbxTransformLinkMTX.Inverse() * fbxTransformMTX * fbxmtxGeometryOffset;
		skeleton[jIndex].offset = fbxGBindPoseInvMTX;

		int numIDX = curCluster->GetControlPointIndicesCount();
		for (int j = 0; j < numIDX; j++) {
			int tempIndex = jIndex;
			double tempWeight = curCluster->GetControlPointWeights()[j];

			tempMesh->mControlPoint[curCluster->GetControlPointIndices()[j]].indices.push_back(tempIndex);
			tempMesh->mControlPoint[curCluster->GetControlPointIndices()[j]].weights.push_back(tempWeight);
		}

		FbxAnimEvaluator* pSceneEvaluator = m_pfbxScene->GetAnimationEvaluator();
		for (FbxLongLong k = 0; k < 100; k++) {
			FbxTime currTime;
			currTime.SetFrame(k, FbxTime::eCustom);

			Keyframe2 tempKey;
			tempKey.frameTime = static_cast<float>(k) / 8.0f;

			FbxAMatrix tempMTX = pSceneEvaluator->GetNodeGlobalTransform(curCluster->GetLink(), currTime);
			tempKey.aniMatrix = tempMTX;

			if (k != 0 && tempAnim.animBones[jIndex].keyframes.back() == tempKey)
				break;
			tempAnim.animBones[jIndex].keyframes.push_back(tempKey);
		}
	}

	tempMesh->animations.push_back(tempAnim);

	// less than 4, empty -> set 0 / 0
	for (auto itr = tempMesh->mControlPoint.begin(); itr != tempMesh->mControlPoint.end(); itr++)
	{
		for (int i = itr->second.indices.size(); i <= 4; i++)
		{
			itr->second.indices.push_back(0);
			itr->second.weights.push_back(0);
		}
	}
}

void CFbxObject2::LoadMesh(FbxNode* inNode, MeshInfo* tempMesh)
{
	FbxMesh* pfbxMesh = inNode->GetMesh();

	tempMesh->vertices.reserve(tempMesh->numPG * 3);

	for (unsigned int i = 0; i < tempMesh->numPG; ++i) {

		for (unsigned int j = 0; j < 3; ++j) {
			int cpIndex = pfbxMesh->GetPolygonVertex(i, j);

			Vertex2 tempVertex;

			// position
			tempVertex.pos = tempMesh->mControlPoint[cpIndex].pos;

			// uv
			int uvindex = pfbxMesh->GetTextureUVIndex(i, j, FbxLayerElement::eTextureDiffuse);
			FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
			FbxLayerElementUV* fbxLayerUV = pfbxMesh->GetLayer(0)->GetUVs();
			fbxUV = fbxLayerUV->GetDirectArray().GetAt(uvindex);
			tempVertex.uv.x = static_cast<float>(fbxUV.mData[0]);
			tempVertex.uv.y = static_cast<float>(1.0f - fbxUV.mData[1]);

			// normal
			FbxVector4 tempNormal;
			pfbxMesh->GetPolygonVertexNormal(i, j, tempNormal);
			tempVertex.normal.x = static_cast<float>(tempNormal.mData[0]);
			tempVertex.normal.y = static_cast<float>(tempNormal.mData[2]);
			tempVertex.normal.z = static_cast<float>(tempNormal.mData[1]);
			
			tempMesh->vertices.push_back(tempVertex);
		}
	}

	//tempMesh->mControlPoint.clear();
}

void CFbxObject2::LoadFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		cout << "메쉬 발견: ";
		FbxMesh* pfbxMesh = pNode->GetMesh();

		int numCP = pfbxMesh->GetControlPointsCount();
		int numPG = pfbxMesh->GetPolygonCount();
		int numDC = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);

		int* pnIndices = new int[numPG*3];

		int indicecount = 0;

		for (int i = 0; i < numPG; i++) {
			for (int j = 0; j < 3; j++) {
				int indexCP = pfbxMesh->GetPolygonVertex(i, j);

				pnIndices[indicecount++] = indexCP;
			}
		}

		MeshInfo* pMeshinfo = new MeshInfo();
		pMeshinfo->numCP = numCP; pMeshinfo->numPG = numPG; pMeshinfo->numDC = numDC;
		pMeshinfo->pMesh = new CMeshFbxTextured(pd3dDevice, pd3dCommandList, numCP, numPG * 3, pnIndices);

		CShader* tempShader = new CShader();
		tempShader = CShaderHandler::GetInstance().GetData("Object");
		pMeshinfo->pShader = tempShader;

		FbxVector4* pfbxv4Vertices = new FbxVector4[numCP];
		::memcpy(pfbxv4Vertices, pfbxMesh->GetControlPoints(), numCP * sizeof(FbxVector4));

		for (int i = 0; i < numCP; i++) {
			ControlPoint2 temp;
			temp.pos.x = (float)pfbxv4Vertices[i][0];
			temp.pos.y = (float)pfbxv4Vertices[i][2];
			temp.pos.z = (float)pfbxv4Vertices[i][1];
			pMeshinfo->mControlPoint[i] = temp;

			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Position = pMeshinfo->mControlPoint[i].pos;
			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf2TexCoord = XMFLOAT2(0, 0);
			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Normal = XMFLOAT3(0, 0, 0);
		}

		vMesh.push_back(*pMeshinfo);

		pfbxMesh->SetUserDataPtr(pMeshinfo);

		if (pnIndices) delete[] pnIndices;
	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) LoadFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pNode->GetChild(i));
}

void CFbxObject2::AnimateFbxMesh(FbxNode* pNode, FbxTime& fbxCurrentTime)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pNode->GetMesh();
		
		int numCP = pfbxMesh->GetControlPointsCount();
		int numDC = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);

		if (numCP > 0)
		{
			FbxVector4* pfbxv4Vertices = new FbxVector4[numCP];
			::memcpy(pfbxv4Vertices, pfbxMesh->GetControlPoints(), numCP * sizeof(FbxVector4));

			if (numDC > 0) {
				FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
				FbxSkin::EType nSkinningType = pfbxSkinDeformer->GetSkinningType();

				if ((nSkinningType == FbxSkin::eLinear) || (nSkinningType == FbxSkin::eRigid)) {

					FbxAMatrix* pfbxmtxClusterDeformations = new FbxAMatrix[numCP];
					::memset(pfbxmtxClusterDeformations, 0, numCP * sizeof(FbxAMatrix));
					double* pfSumOfClusterWeights = new double[numCP];
					::memset(pfSumOfClusterWeights, 0, numCP * sizeof(double));

					int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
					for (int i = 0; i < nSkinDeformers; i++)
					{
						FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
						int nClusters = pfbxSkinDeformer->GetClusterCount();

						for (int j = 0; j < nClusters; j++)
						{
							FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);
							if (!pfbxCluster->GetLink()) continue;

							FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());
							FbxAMatrix fbxmtxBindPoseMeshToRoot; //Cluster Transform
							pfbxCluster->GetTransformMatrix(fbxmtxBindPoseMeshToRoot);
							FbxAMatrix fbxmtxBindPoseBoneToRoot; //Cluster Link Transform
							pfbxCluster->GetTransformLinkMatrix(fbxmtxBindPoseBoneToRoot);
							FbxAMatrix fbxmtxAnimatedBoneToRoot = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime); //Cluster Link Node Global Transform
							// ㄴ애니메이션 변경시마다 달라지는값

							FbxAMatrix temp = fbxmtxBindPoseBoneToRoot.Inverse() * fbxmtxBindPoseMeshToRoot * fbxmtxGeometryOffset;

							FbxAMatrix fbxmtxClusterDeformation = fbxmtxAnimatedBoneToRoot * temp;

							int* pnIndices = pfbxCluster->GetControlPointIndices();
							double* pfWeights = pfbxCluster->GetControlPointWeights();

							int nIndices = pfbxCluster->GetControlPointIndicesCount();
							for (int k = 0; k < nIndices; k++)
							{
								int nIndex = pnIndices[k];
								double fWeight = pfWeights[k];
								if ((nIndex >= numCP) || (fWeight == 0.0)) continue;

								FbxAMatrix fbxmtxInfluence = fbxmtxClusterDeformation;
								MatrixScale(fbxmtxInfluence, fWeight);

								MatrixAdd(pfbxmtxClusterDeformations[nIndex], fbxmtxInfluence);
								pfSumOfClusterWeights[nIndex] += fWeight;
							}
						}
					}

					MeshInfo* pMeshinfo = (MeshInfo*)pfbxMesh->GetUserDataPtr();

					for (int i = 0; i < numCP; i++)
					{
						FbxVector4 fbxv4Vertex = pfbxv4Vertices[i];
						pfbxv4Vertices[i] = pfbxmtxClusterDeformations[i].MultT(fbxv4Vertex) / pfSumOfClusterWeights[i];

						pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Position = XMFLOAT3(	(float)pfbxv4Vertices[i][0],
																								(float)pfbxv4Vertices[i][2],
																								(float)pfbxv4Vertices[i][1]);
						pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf2TexCoord = XMFLOAT2(0, 0);
						pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Normal = XMFLOAT3(0, 0, 0);
					}

					delete[] pfbxmtxClusterDeformations;
					delete[] pfSumOfClusterWeights;
				}
			}
			delete[] pfbxv4Vertices;
		}

	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) AnimateFbxMesh(pNode->GetChild(i), fbxCurrentTime);
}

void CFbxObject2::DrawFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pNode->GetMesh();

		if (m_pAnimationController->m_nAnimationStack == 0)
		{
			FbxAMatrix fbxmtxTransform = fbxmtxWorld;

			MeshInfo* pMeshinfo = (MeshInfo*)pfbxMesh->GetUserDataPtr();
			if (pMeshinfo->pShader) pMeshinfo->pShader->UpdateShaderVariable(pd3dCommandList, &FbxMatrixToXmFloat4x4Matrix(&fbxmtxTransform));
			if (pMeshinfo->pShader) pMeshinfo->pShader->Render(pd3dCommandList, NULL);
			if (pMeshinfo->pMesh) pMeshinfo->pMesh->Render(pd3dCommandList);
		}
		else
		{
			FbxAMatrix fbxmtxNodeToRoot = pNode->EvaluateGlobalTransform(fbxCurrentTime);
			FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pNode);

			int numCP = pfbxMesh->GetControlPointsCount();

			if (numCP > 0)
			{
				FbxAMatrix fbxmtxTransform = fbxmtxWorld;
				int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
				if (nSkinDeformers == 0) fbxmtxTransform = fbxmtxWorld * fbxmtxNodeToRoot * fbxmtxGeometryOffset;

				MeshInfo* pMeshinfo = (MeshInfo*)pfbxMesh->GetUserDataPtr();
				if (pMeshinfo->pShader) pMeshinfo->pShader->UpdateShaderVariable(pd3dCommandList, &FbxMatrixToXmFloat4x4Matrix(&fbxmtxTransform));
				if (pMeshinfo->pShader) pMeshinfo->pShader->Render(pd3dCommandList, NULL);
				if (pMeshinfo->pMesh) pMeshinfo->pMesh->Render(pd3dCommandList);
			}
		}
	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) DrawFbxMesh(pd3dCommandList, pNode->GetChild(i), fbxCurrentTime, fbxmtxWorld);
}

void CFbxObject2::Animate(float fTimeElapsed)
{
	if (m_pAnimationController)
	{
		m_pAnimationController->AdvanceTime(fTimeElapsed);
		FbxTime fbxCurrentTime = m_pAnimationController->GetCurrentTime();

		AnimateFbxMesh(m_pfbxScene->GetRootNode(), fbxCurrentTime);
	}
}

void CFbxObject2::Update(float fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	static float Friction = 50.0f;

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = MaxVelocityXZ * fTimeElapsed;
	if (fLength > MaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = MaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > MaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity);

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

	m_AnimationTime += fTimeElapsed;

	if (m_AnimationTime > 0.1f) {
		Animate(m_AnimationTime);
		m_AnimationTime = 0.0f;
	}
}

void CFbxObject2::SetAnimationStack(int nAnimationStack)
{
	m_pAnimationController->SetAnimationStack(m_pfbxScene, nAnimationStack);
}

void CFbxObject2::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	for (int i = 0; i < vMesh.size(); i++) {
		if (vMesh[i].pShader) vMesh[i].pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		if (vMesh[i].pShader) vMesh[i].pShader->Render(pd3dCommandList, NULL);
		if (vMesh[i].pMesh) vMesh[i].pMesh->Render(pd3dCommandList);
	}
}

void CFbxObject2::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CFbxObject2::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, FbxAMatrix* pfbxf4x4World)
{
	XMFLOAT4X4 xmf4x4World = ::FbxMatrixToXmFloat4x4Matrix(pfbxf4x4World);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CFbxObject2::ReleaseUploadBuffers()
{

}

void CFbxObject2::LoadFbxModelFromFile(char* pstrFbxFileName)
{
	
}