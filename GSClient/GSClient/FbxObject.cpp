#include "stdafx.h"
#include "FbxObject.h"
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

void MatrixAddToDiagonal(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	fbxmtxSrcMatrix[0][0] += pValue;
	fbxmtxSrcMatrix[1][1] += pValue;
	fbxmtxSrcMatrix[2][2] += pValue;
	fbxmtxSrcMatrix[3][3] += pValue;
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

CFbxObject::CFbxObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };
	m_time = 0;

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CFbxObject::CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	LoadFile();
	m_time = 0;


	/*LoadScene(pstrFbxFileName, pfbxSdkManager);
	
	if (m_pfbxScene == NULL)
		cout << "fbx 메쉬 로드 실패" << endl;
	else {
		LoadSkeletonHierarchy(m_pfbxScene->GetRootNode());
		LoadFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pfbxScene->GetRootNode());
		m_pAnimationController = new CAnimationController(m_pfbxScene);
		cout << "fbx 메쉬 로드 성공!" << endl;
	}

	cout << "nSkeleton: " << mSkeleton.size() << " |==========================" << endl;
	for (int i = 0; i < mSkeleton.size(); i++) {
		cout << "[" << i << "] " << mSkeleton[i].name << endl;
	}

	m_time = 0;*/
}

CFbxObject::~CFbxObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
	if (m_pAnimationController) delete m_pAnimationController;
}

void CFbxObject::LoadFile()
{
	ifstream file;
	file.open("FbxExportedFile.bin", ios::in | ios::binary);
	string temp;

	int numVertex, numPolygon, numBone;

	file >> temp >> numVertex;
	file >> temp >> numPolygon;
	file >> temp >> numBone;
	file >> temp >> temp;
	cout << "파일 로드: " << numVertex << " " << numPolygon << " " << numBone << endl;

	for (int i = 0; i < numVertex; i++) {
		Vertex tempVertex;

		file >> tempVertex.pos.x >> tempVertex.pos.y >> tempVertex.pos.z >>
				tempVertex.uv.x >> tempVertex.uv.y >>
				tempVertex.normal.x >> tempVertex.normal.y >> tempVertex.normal.z;
		file >> tempVertex.indices[0] >> tempVertex.indices[1] >> tempVertex.indices[2] >> tempVertex.indices[3] >>
				tempVertex.weights.x >> tempVertex.weights.y >> tempVertex.weights.z >> temp;

		vertices.push_back(tempVertex);
		//cout << tempVertex.pos.x << " " << tempVertex.uv.x << " " << tempVertex.weights.y << endl;
	}
	file >> temp >> temp;

	for (int i = 0; i < numPolygon*3; i++) {
		int tempInt;
		file >> tempInt;
		indices.push_back(tempInt);
	}

	file.close();
}

void CFbxObject::LoadFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pNode)
{
	//FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	//if ((pfbxNodeAttribute != NULL) &&
	//	(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	//{
	//	cout << "메쉬 발견: ";
	//	FbxMesh* pfbxMesh = pNode->GetMesh();

	//	MeshInfo* pMeshinfo = new MeshInfo();

	//	int numCP = pfbxMesh->GetControlPointsCount();
	//	int numPG = pfbxMesh->GetPolygonCount();
	//	int numDC = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	//	pMeshinfo->numCP = numCP; pMeshinfo->numPG = numPG; pMeshinfo->numDC = numDC;

	//	int* pnIndices = new int[numPG*3];

	//	int indicecount = 0;

	//	for (int i = 0; i < numPG; i++) {
	//		for (int j = 0; j < 3; j++) {
	//			int indexCP = pfbxMesh->GetPolygonVertex(i, j);

	//			pnIndices[indicecount++] = indexCP;
	//		}
	//	}
	//	pMeshinfo->indexCP = pnIndices;

	//	pMeshinfo->pMesh = new CMeshFbxTextured(pd3dDevice, pd3dCommandList, numCP, numPG * 3, pnIndices);

	//	CShader* tempShader = new CShader();
	//	tempShader = CShaderHandler::GetInstance().GetData("Object");
	//	pMeshinfo->pShader = tempShader;

	//	FbxVector4* pfbxv4Vertices = new FbxVector4[numCP];
	//	::memcpy(pfbxv4Vertices, pfbxMesh->GetControlPoints(), numCP * sizeof(FbxVector4));
	//	pMeshinfo->pfbxv4Vertices = pfbxv4Vertices;

	//	pMeshinfo->mCtrPoint = new CtrPoint[numCP];
	//	for (int i = 0; i < numCP; i++) {
	//		pMeshinfo->mCtrPoint[i].pos.x = (float)pfbxv4Vertices[i][0];
	//		pMeshinfo->mCtrPoint[i].pos.y = (float)pfbxv4Vertices[i][2];
	//		pMeshinfo->mCtrPoint[i].pos.z = (float)pfbxv4Vertices[i][1];

	//		pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Position = pMeshinfo->mCtrPoint[i].pos;
	//		pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf2TexCoord = XMFLOAT2(0, 0);
	//		pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Normal = XMFLOAT3(0, 0, 0);
	//	}

	//	if (numDC > 0) {
	//		FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());

	//		FbxAMatrix* pfbxmtxClusterDeformations = new FbxAMatrix[numCP];
	//		::memset(pfbxmtxClusterDeformations, 0, numCP * sizeof(FbxAMatrix));

	//		double* pfSumOfClusterWeights = new double[numCP];
	//		::memset(pfSumOfClusterWeights, 0, numCP * sizeof(double));

	//		for (int i = 0; i < numDC; i++) {
	//			FbxSkin* pfbxSkin = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);

	//			int numCS = pfbxSkin->GetClusterCount();

	//			for (int cindex = 0; cindex < numCS; cindex++) {
	//				FbxCluster* curCluster = pfbxSkin->GetCluster(cindex);

	//				FbxAMatrix fbxTransformMTX;
	//				curCluster->GetTransformMatrix(fbxTransformMTX);
	//				FbxAMatrix fbxTransformLinkMTX;
	//				curCluster->GetTransformLinkMatrix(fbxTransformLinkMTX);
	//				FbxAMatrix fbxBindPoseInvMTX = fbxmtxGeometryOffset * fbxTransformMTX * fbxTransformLinkMTX.Inverse();
	//				mSkeleton[cindex].globalBindpose = fbxBindPoseInvMTX;

	//				FbxAnimStack* pCurAnimStack = m_pfbxScene->GetSrcObject<FbxAnimStack>(0);
	//				FbxString curAnimName = pCurAnimStack->GetName();
	//				FbxTakeInfo* pTakeInfo = m_pfbxScene->GetTakeInfo(curAnimName);
	//				FbxTime start = pTakeInfo->mLocalTimeSpan.GetStart();
	//				FbxTime end = pTakeInfo->mLocalTimeSpan.GetStop();
	//				double animLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
	//				
	//				AnimationFrameInfo tempAFinfo;
	//				for (double k = start.GetFrameCount(FbxTime::eFrames24); k <= end.GetFrameCount(FbxTime::eFrames24); k++) {
	//					FbxTime curTime;
	//					curTime.SetFrame(k, FbxTime::eFrames24);

	//					FbxAMatrix tempMtx = pNode->EvaluateGlobalTransform(curTime).Inverse() *
	//						curCluster->GetLink()->EvaluateGlobalTransform(curTime) * fbxmtxGeometryOffset;
	//					// 각 조인트에 변환행렬 저장 

	//					tempAFinfo.matrix.push_back(tempMtx);
	//				}
	//				pMeshinfo->animframe.push_back(tempAFinfo);
	//			}
	//		}
	//	}

	//	vMesh.push_back(*pMeshinfo);

	//	pfbxMesh->SetUserDataPtr(pMeshinfo);

	//	if (numDC > 0) cout << "애니메이션 있음 | [Polygon]: " << numPG << " | [ControlPoint]: " << numCP << " | [DeformerCount]: " << numDC << endl;
	//	else cout << "애니메이션 없음 | [Polygon]: " << numPG << " | [ControlPoint]: " << numCP << endl;

	//	if (pnIndices) delete[] pnIndices;
	//}

	//int nChild = pNode->GetChildCount();
	//for (int i = 0; i < nChild; i++) LoadFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pNode->GetChild(i));
}

void CFbxObject::AnimateFbxMesh(FbxNode* pNode, FbxTime& fbxCurrentTime)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pNode->GetMesh();
		
		int numCP = pfbxMesh->GetControlPointsCount();
		int numDC = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);

		FbxVector4* pfbxv4Vertices = new FbxVector4[numCP];
		::memcpy(pfbxv4Vertices, pfbxMesh->GetControlPoints(), numCP * sizeof(FbxVector4));

		if (numDC > 0)
		{
			FbxAMatrix* pfbxmtxClusterDeformations = new FbxAMatrix[numCP];
			::memset(pfbxmtxClusterDeformations, 0, numCP * sizeof(FbxAMatrix));

			double* pfSumOfClusterWeights = new double[numCP];
			::memset(pfSumOfClusterWeights, 0, numCP * sizeof(double));

			for (int i = 0; i < numDC; i++)
			{
				FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
				int nClusters = pfbxSkinDeformer->GetClusterCount();	// nClusters = mSkeleton size
				for (int j = 0; j < nClusters; j++)
				{
					FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);
					if (!pfbxCluster->GetLink()) continue;

					FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());

					FbxAMatrix fbxTransformMTX;
					pfbxCluster->GetTransformMatrix(fbxTransformMTX);
					FbxAMatrix fbxTransformLinkMTX;
					pfbxCluster->GetTransformLinkMatrix(fbxTransformLinkMTX);
					FbxAMatrix fbxAnimatedBoneMTX = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime);

					FbxAMatrix fbxmtxClusterDeformation = fbxmtxGeometryOffset * fbxAnimatedBoneMTX * fbxTransformLinkMTX.Inverse() * fbxTransformMTX;

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

			for (int i = 0; i < numCP; i++)
			{
				FbxVector4 fbxv4Vertex = pfbxv4Vertices[i];
				pfbxv4Vertices[i] = pfbxmtxClusterDeformations[i].MultT(fbxv4Vertex) / pfSumOfClusterWeights[i];
			}

			delete[] pfbxmtxClusterDeformations;
			delete[] pfSumOfClusterWeights;
		}

		/*MeshInfo* pMeshinfo = (MeshInfo*)pfbxMesh->GetUserDataPtr();

		for (int i = 0; i < numCP; i++) {
			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Position = XMFLOAT3(	(float)pfbxv4Vertices[i][0],
																					(float)pfbxv4Vertices[i][2],
																					(float)pfbxv4Vertices[i][1]);
			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf2TexCoord = XMFLOAT2(0, 0);
			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Normal = XMFLOAT3(0, 0, 0);
		}*/

		delete[] pfbxv4Vertices;
	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) AnimateFbxMesh(pNode->GetChild(i), fbxCurrentTime);
}

void CFbxObject::DrawFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld)
{
	//FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	//if ((pfbxNodeAttribute != NULL) &&
	//	(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	//{
	//	FbxMesh* pfbxMesh = pNode->GetMesh();

	//	if (m_pAnimationController->m_nAnimationStack == 0)
	//	{
	//		FbxAMatrix fbxmtxTransform = fbxmtxWorld;
	//		//CFbxObject::UpdateShaderVariable(pd3dCommandList, &fbxmtxTransform);

	//		MeshInfo* pMeshinfo = (MeshInfo*)pfbxMesh->GetUserDataPtr();
	//		if (pMeshinfo->pShader) pMeshinfo->pShader->UpdateShaderVariable(pd3dCommandList, &FbxMatrixToXmFloat4x4Matrix(&fbxmtxTransform));
	//		if (pMeshinfo->pShader) pMeshinfo->pShader->Render(pd3dCommandList, NULL);
	//		if (pMeshinfo->pMesh) pMeshinfo->pMesh->Render(pd3dCommandList);
	//	}
	//	else
	//	{
	//		FbxAMatrix fbxmtxNodeToRoot = pNode->EvaluateGlobalTransform(fbxCurrentTime);
	//		FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pNode);

	//		int numCP = pfbxMesh->GetControlPointsCount();

	//		if (numCP > 0)
	//		{
	//			FbxAMatrix fbxmtxTransform = fbxmtxWorld;
	//			int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	//			if (nSkinDeformers == 0) fbxmtxTransform = fbxmtxWorld * fbxmtxNodeToRoot * fbxmtxGeometryOffset;

	//			//CFbxObject::UpdateShaderVariable(pd3dCommandList, &fbxmtxTransform);
	//			//&FbxMatrixToXmFloat4x4Matrix(&fbxmtxTransform)

	//			MeshInfo* pMeshinfo = (MeshInfo*)pfbxMesh->GetUserDataPtr();
	//			if (pMeshinfo->pShader) pMeshinfo->pShader->UpdateShaderVariable(pd3dCommandList, &FbxMatrixToXmFloat4x4Matrix(&fbxmtxTransform));
	//			if (pMeshinfo->pShader) pMeshinfo->pShader->Render(pd3dCommandList, NULL);
	//			if (pMeshinfo->pMesh) pMeshinfo->pMesh->Render(pd3dCommandList);
	//		}
	//	}
	//}

	//int nChild = pNode->GetChildCount();
	//for (int i = 0; i < nChild; i++) DrawFbxMesh(pd3dCommandList, pNode->GetChild(i), fbxCurrentTime, fbxmtxWorld);
}

void CFbxObject::Animate(float fTimeElapsed)
{
	if (m_pAnimationController)
	{
		m_pAnimationController->AdvanceTime(fTimeElapsed);
		FbxTime fbxCurrentTime = m_pAnimationController->GetCurrentTime();

		AnimateFbxMesh(m_pfbxScene->GetRootNode(), fbxCurrentTime);
	}
}

void CFbxObject::Update(double fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	static float Friction = 50.0f;

	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
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

	//cout << fTimeElapsed << endl;

	/*m_time++;
	if (m_time > 4) {
		Animate(fTimeElapsed*4);
		m_time = 0;
	}*/

	/*if (m_time > 40) {
		m_time = 0;
	}
	AnimateAlter(m_time);
	m_time++;*/
}

void CFbxObject::SetAnimationStack(int nAnimationStack)
{
	m_pAnimationController->SetAnimationStack(m_pfbxScene, nAnimationStack);
}

void CFbxObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	//FbxAMatrix fbxf4x4World = XmFloat4x4MatrixToFbxMatrix(m_xmf4x4World);

	//if (m_pfbxScene) DrawFbxMesh(pd3dCommandList, m_pfbxScene->GetRootNode(), m_pAnimationController->GetCurrentTime(), fbxf4x4World);

	/*for (int i = 0; i < vMesh.size(); i++) {
		if (vMesh[i].pShader) vMesh[i].pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		if (vMesh[i].pShader) vMesh[i].pShader->Render(pd3dCommandList, NULL);
		if (vMesh[i].pMesh) vMesh[i].pMesh->Render(pd3dCommandList);
	}*/
}

void CFbxObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CFbxObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, FbxAMatrix* pfbxf4x4World)
{
	XMFLOAT4X4 xmf4x4World = ::FbxMatrixToXmFloat4x4Matrix(pfbxf4x4World);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CFbxObject::ReleaseUploadBuffers()
{
	//if (m_pfbxScene) ::ReleaseUploadBufferFromFbxNodeHierarchy(m_pfbxScene->GetRootNode());
}

void CFbxObject::LoadFbxModelFromFile(char* pstrFbxFileName)
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//void CFbxObjectFileLoadVer::LoadFbxModelFromFile(char* pstrFbxFileName)
//{
//	ifstream file(pstrFbxFileName);
//
//	string buffer;
//	//int num = 0;
//	//vector<CTexturedVertex> tempVector;
//
//	int nMesh = 0;
//
//	getline(file, buffer);
//	if (buffer == "[MeshCount]") {
//		getline(file, buffer);
//		nMesh = atoi(buffer.c_str());
//	}
//
//	for (int i = 0; i < nMesh; i++) {
//		FbxSubMesh tempMesh;
//
//		getline(file, buffer);
//		if (buffer == "[MeshInfo]") {
//			getline(file, buffer, ' '); tempMesh.nControlPoint = atoi(buffer.c_str());
//			getline(file, buffer, ' '); tempMesh.nPolygon = atoi(buffer.c_str());
//			getline(file, buffer); tempMesh.nDeformer = atoi(buffer.c_str());
//		}
//
//		getline(file, buffer);
//		if (buffer == "[Vertex]") {
//			for (int j = 0; j < tempMesh.nPolygon * 3; j++) {
//				int tempIndex;
//				XMFLOAT3 tempPos, tempNormal;
//				XMFLOAT2 tempUv;
//
//				getline(file, buffer, ' '); tempIndex = atoi(buffer.c_str());
//				getline(file, buffer, ' '); tempPos.x = atof(buffer.c_str());
//				getline(file, buffer, ' '); tempPos.y = atof(buffer.c_str());
//				getline(file, buffer, ' '); tempPos.z = atof(buffer.c_str());
//				getline(file, buffer, ' '); tempNormal.x = atof(buffer.c_str());
//				getline(file, buffer, ' '); tempNormal.y = atof(buffer.c_str());
//				getline(file, buffer, ' '); tempNormal.z = atof(buffer.c_str());
//				getline(file, buffer, ' '); tempUv.x = atof(buffer.c_str());
//				getline(file, buffer);		tempUv.y = atof(buffer.c_str());
//
//				CTexturedVertex tempVertex = { tempPos, tempUv, tempNormal };
//
//				//cout << tempIndex << " " << tempPos.x << endl;
//
//				tempMesh.mIndex.push_back(tempIndex);
//				tempMesh.mVertex.push_back(tempVertex);
//			}
//		}
//
//		tempMesh.mCP = new XMFLOAT3[tempMesh.nControlPoint];
//
//		getline(file, buffer);
//		if (buffer == "[ControlPoint]") {
//			for (int j = 0; j < tempMesh.nControlPoint; j++) {
//				XMFLOAT3 tempPos;
//
//				getline(file, buffer, ' '); tempPos.x = atof(buffer.c_str());
//				getline(file, buffer, ' '); tempPos.y = atof(buffer.c_str());
//				getline(file, buffer);		tempPos.z = atof(buffer.c_str());
//
//				tempMesh.mCP[j] = tempPos;
//			}
//		}
//
//		cout << "[메쉬" << i << "로드]" << tempMesh.nControlPoint << " " << tempMesh.nPolygon << " " << tempMesh.nDeformer << endl;
//
//		mFbxMesh.push_back(tempMesh);
//	}
//
//	file.close();
//}
