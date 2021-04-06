#include "stdafx.h"
#include "FbxObject.h"
#include "Shader.h"
#include "Camera.h"

// FbxUtils
//////////////////////////////////////////////////////////////////////////////////////

class MeshInfo
{
public:
	MeshInfo() { }
	~MeshInfo() { };

public:
	CMesh*	pMesh;
	CShader*	pShader;
	bool animation = false;
};

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

FbxAMatrix ComputeClusterDeformation(FbxMesh* pfbxMesh, FbxCluster* pfbxCluster, FbxCluster::ELinkMode nClusterMode, FbxTime& fbxCurrentTime)
{
	FbxAMatrix fbxmtxVertexTransform;

	if (nClusterMode == FbxCluster::eNormalize)
	{
		FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());

		FbxAMatrix fbxmtxBindPoseMeshToRoot; //Cluster Transform
		pfbxCluster->GetTransformMatrix(fbxmtxBindPoseMeshToRoot);

		FbxAMatrix fbxmtxBindPoseBoneToRoot; //Cluster Link Transform
		pfbxCluster->GetTransformLinkMatrix(fbxmtxBindPoseBoneToRoot);

		FbxAMatrix fbxmtxAnimatedBoneToRoot = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime); //Cluster Link Node Global Transform

		fbxmtxVertexTransform = fbxmtxAnimatedBoneToRoot * fbxmtxBindPoseBoneToRoot.Inverse() * fbxmtxBindPoseMeshToRoot * fbxmtxGeometryOffset;
	}
	else
	{ //FbxCluster::eAdditive
		if (pfbxCluster->GetAssociateModel())
		{
			FbxAMatrix fbxmtxAssociateModel;
			pfbxCluster->GetTransformAssociateModelMatrix(fbxmtxAssociateModel);

			FbxAMatrix fbxmtxAssociateGeometryOffset = GetGeometricOffsetTransform(pfbxCluster->GetAssociateModel());
			fbxmtxAssociateModel *= fbxmtxAssociateGeometryOffset;

			FbxAMatrix fbxmtxAssociateModelGlobal = pfbxCluster->GetAssociateModel()->EvaluateGlobalTransform(fbxCurrentTime);

			FbxAMatrix fbxmtxClusterTransform;
			pfbxCluster->GetTransformMatrix(fbxmtxClusterTransform);

			FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxMesh->GetNode());
			fbxmtxClusterTransform *= fbxmtxGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkTransform;
			pfbxCluster->GetTransformLinkMatrix(fbxmtxClusterLinkTransform);

			FbxAMatrix fbxmtxLinkGeometryOffset = GetGeometricOffsetTransform(pfbxCluster->GetLink());
			fbxmtxClusterLinkTransform *= fbxmtxLinkGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkToRoot = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime);

			fbxmtxVertexTransform = fbxmtxClusterTransform.Inverse() * fbxmtxAssociateModel * fbxmtxAssociateModelGlobal.Inverse() * fbxmtxClusterLinkToRoot * fbxmtxClusterLinkTransform.Inverse() * fbxmtxClusterTransform;
		}
	}
	return(fbxmtxVertexTransform);
}

void ComputeLinearDeformation(FbxMesh* pfbxMesh, FbxTime& fbxCurrentTime, FbxVector4* pfbxv4Vertices, int nVertices)
{
	FbxAMatrix* pfbxmtxClusterDeformations = new FbxAMatrix[nVertices];
	::memset(pfbxmtxClusterDeformations, 0, nVertices * sizeof(FbxAMatrix));

	double* pfSumOfClusterWeights = new double[nVertices];
	::memset(pfSumOfClusterWeights, 0, nVertices * sizeof(double));

	FbxCluster::ELinkMode nClusterMode = ((FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	if (nClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < nVertices; ++i) pfbxmtxClusterDeformations[i].SetIdentity();
	}

	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < nSkinDeformers; i++)
	{
		FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
		int nClusters = pfbxSkinDeformer->GetClusterCount();

		for (int j = 0; j < nClusters; j++)
		{
			FbxCluster* pfbxCluster = pfbxSkinDeformer->GetCluster(j);
			if (!pfbxCluster->GetLink()) continue;

			FbxAMatrix fbxmtxClusterDeformation = ComputeClusterDeformation(pfbxMesh, pfbxCluster, nClusterMode, fbxCurrentTime);

			int* pnIndices = pfbxCluster->GetControlPointIndices();
			double* pfWeights = pfbxCluster->GetControlPointWeights();

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			for (int k = 0; k < nIndices; k++)
			{
				int nIndex = pnIndices[k];
				double fWeight = pfWeights[k];
				if ((nIndex >= nVertices) || (fWeight == 0.0)) continue;

				FbxAMatrix fbxmtxInfluence = fbxmtxClusterDeformation;
				MatrixScale(fbxmtxInfluence, fWeight);

				if (nClusterMode == FbxCluster::eAdditive)
				{
					MatrixAddToDiagonal(fbxmtxInfluence, 1.0 - fWeight);
					pfbxmtxClusterDeformations[nIndex] = fbxmtxInfluence * pfbxmtxClusterDeformations[nIndex];
					pfSumOfClusterWeights[nIndex] = 1.0;
				}
				else
				{
					MatrixAdd(pfbxmtxClusterDeformations[nIndex], fbxmtxInfluence);
					pfSumOfClusterWeights[nIndex] += fWeight;
				}
			}
		}
	}

	for (int i = 0; i < nVertices; i++)
	{
		if (pfSumOfClusterWeights[i] != 0.0)
		{
			FbxVector4 fbxv4Vertex = pfbxv4Vertices[i];
			pfbxv4Vertices[i] = pfbxmtxClusterDeformations[i].MultT(fbxv4Vertex);
			if (nClusterMode == FbxCluster::eNormalize)
			{
				pfbxv4Vertices[i] /= pfSumOfClusterWeights[i];
			}
			else if (nClusterMode == FbxCluster::eTotalOne)
			{
				fbxv4Vertex *= (1.0 - pfSumOfClusterWeights[i]);
				pfbxv4Vertices[i] += fbxv4Vertex;
			}
		}
	}

	delete[] pfbxmtxClusterDeformations;
	delete[] pfSumOfClusterWeights;
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

	cout << "�ִϸ��̼� ���� ������: " << m_nAnimationStacks << endl;

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

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CFbxObject::CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	//FbxScene* m_pfbxScene = FbxScene::Create(pfbxSdkManager, "");

	LoadScene(pstrFbxFileName, pfbxSdkManager);
	
	if (m_pfbxScene == NULL)
		cout << "fbx �޽� �ε� ����" << endl;
	else {
		LoadFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pfbxScene->GetRootNode());
		m_pAnimationController = new CAnimationController(m_pfbxScene);
		cout << "fbx �޽� �ε� ����!" << endl;
	}
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

void CFbxObject::LoadScene(char* pstrFbxFileName, FbxManager* pfbxSdkManager)
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

void CFbxObject::LoadFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		cout << "�޽� �߰�: ";
		FbxMesh* pfbxMesh = pNode->GetMesh();

		int numCP = pfbxMesh->GetControlPointsCount();
		int numPG = pfbxMesh->GetPolygonCount();
		int numDC = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);

		int* pnIndices = new int[numPG*3];

		int indicecount = 0;

		for (int i = 0; i < numPG; i++) {
			for (int j = 0; j < 3; j++) 
				pnIndices[indicecount++] = pfbxMesh->GetPolygonVertex(i, j);
		}

		MeshInfo* pMeshinfo = new MeshInfo();
		//pMeshinfo->pMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, numCP, numPG * 3, pnIndices);
		pMeshinfo->pMesh = new CMeshFbxTextured(pd3dDevice, pd3dCommandList, numCP, numPG * 3, pnIndices);

		CShader* tempShader = new CShader();

		if (numDC > 0) {
			tempShader->CreateVertexShader(L"Shaders\\Shaders.hlsl", "VSTextured");
			tempShader->CreatePixelShader(L"Shaders\\Shaders.hlsl", "PSTextured");
			tempShader->CreateInputLayout(ShaderTypes::Textured);
			cout << "�ִϸ��̼� ���� | [Polygon]: " << numPG << " | [ControlPoint]: " << numCP << " | [DeformerCount]: " << numDC << endl;
		}
		else {
			tempShader->CreateVertexShader(L"Shaders\\Shaders.hlsl", "VSTextured");
			tempShader->CreatePixelShader(L"Shaders\\Shaders.hlsl", "PSTextured");
			tempShader->CreateInputLayout(ShaderTypes::Textured);
			cout << "�ִϸ��̼� ���� | [Polygon]: " << numPG << " | [ControlPoint]: " << numCP << endl;
		}

		pMeshinfo->pShader = tempShader;

		pMeshinfo->pShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature);


		pfbxMesh->SetUserDataPtr(pMeshinfo);

		if (pnIndices) delete[] pnIndices;
	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) LoadFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pNode->GetChild(i));
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
			FbxSkin* pfbxSkinDeformer = (FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
			FbxSkin::EType nSkinningType = pfbxSkinDeformer->GetSkinningType();

			if ((nSkinningType == FbxSkin::eLinear) || (nSkinningType == FbxSkin::eRigid))
			{
				//cout << "SkinningType: eLinear or eRigid" << endl;
				ComputeLinearDeformation(pfbxMesh, fbxCurrentTime, pfbxv4Vertices, numCP);
			}
		}

		MeshInfo* pMeshinfo = (MeshInfo*)pfbxMesh->GetUserDataPtr();

		for (int i = 0; i < numCP; i++) {
			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf3Position = XMFLOAT3(	(float)pfbxv4Vertices[i][0],
																	(float)pfbxv4Vertices[i][2],
																	(float)pfbxv4Vertices[i][1]);
			pMeshinfo->pMesh->m_pxmf4MappedPositions[i].m_xmf2TexCoord = XMFLOAT2(0, 0);
		}

		delete[] pfbxv4Vertices;
	}

	int nChild = pNode->GetChildCount();
	for (int i = 0; i < nChild; i++) AnimateFbxMesh(pNode->GetChild(i), fbxCurrentTime);
}

void CFbxObject::DrawFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld)
{
	FbxNodeAttribute* pfbxNodeAttribute = pNode->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pNode->GetMesh();

		if (m_pAnimationController->m_nAnimationStack == 0)
		{
			FbxAMatrix fbxmtxTransform = fbxmtxWorld;
			//CFbxObject::UpdateShaderVariable(pd3dCommandList, &fbxmtxTransform);

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

				//CFbxObject::UpdateShaderVariable(pd3dCommandList, &fbxmtxTransform);
				//&FbxMatrixToXmFloat4x4Matrix(&fbxmtxTransform)

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
}

void CFbxObject::SetAnimationStack(int nAnimationStack)
{
	m_pAnimationController->SetAnimationStack(m_pfbxScene, nAnimationStack);
}

void CFbxObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	FbxAMatrix fbxf4x4World = XmFloat4x4MatrixToFbxMatrix(m_xmf4x4World);
	if (m_pfbxScene) DrawFbxMesh(pd3dCommandList, m_pfbxScene->GetRootNode(), m_pAnimationController->GetCurrentTime(), fbxf4x4World);
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
	ifstream file(pstrFbxFileName);

	string buffer;
	//int num = 0;
	//vector<CTexturedVertex> tempVector;
	CTexturedVertex tempvertex;

	while (file.peek() != EOF) {
		getline(file, buffer);

		if (buffer == "[Vertex]") {
			getline(file, buffer);
			vertexnum = atoi(buffer.c_str());

			for (int i = 0; i < vertexnum; i++) {
				getline(file, buffer, ' ');
				tempvertex.m_xmf3Position.x = atof(buffer.c_str());
				getline(file, buffer, ' ');
				tempvertex.m_xmf3Position.y = atof(buffer.c_str());
				getline(file, buffer, ' ');
				tempvertex.m_xmf3Position.z = atof(buffer.c_str());
				getline(file, buffer, ' ');
				tempvertex.m_xmf2TexCoord.x = atof(buffer.c_str());
				getline(file, buffer);
				tempvertex.m_xmf2TexCoord.y = atof(buffer.c_str());


				//cout << tempvertex.m_xmf3Position.x << " " << tempvertex.m_xmf3Position.y << " " << tempvertex.m_xmf3Position.z << " "
				//	<< tempvertex.m_xmf2TexCoord.x << " " << tempvertex.m_xmf2TexCoord.y << endl;

				m_vertex.push_back(tempvertex);
			}

			//cout << "�޽� �ε� �Ϸ�: " << m_vertex.size() << " ��" << endl;
		}

		if (buffer == "[Joint]") {

		}

		if (buffer == "[Animation]") {

		}
	}

	file.close();
}