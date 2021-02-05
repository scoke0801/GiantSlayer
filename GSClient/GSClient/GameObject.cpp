#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

CAnimationController::CAnimationController(FbxScene* pfbxScene)
{
	FbxArray<FbxString*> fbxAnimationStackNames;
	pfbxScene->FillAnimStackNameArray(fbxAnimationStackNames);

	m_nAnimationStacks = fbxAnimationStackNames.Size();

	m_ppfbxAnimationStacks = new FbxAnimStack * [m_nAnimationStacks];
	m_pfbxStartTimes = new FbxTime[m_nAnimationStacks];
	m_pfbxStopTimes = new FbxTime[m_nAnimationStacks];
	m_pfbxCurrentTimes = new FbxTime[m_nAnimationStacks];

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
	m_pfbxCurrentTimes[nAnimationStack].SetSecondDouble(fPosition);;
}

void CAnimationController::AdvanceTime(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;

	FbxTime fbxElapsedTime;
	fbxElapsedTime.SetSecondDouble(fTimeElapsed);

	m_pfbxCurrentTimes[m_nAnimationStack] += fbxElapsedTime;
	if (m_pfbxCurrentTimes[m_nAnimationStack] > m_pfbxStopTimes[m_nAnimationStack]) m_pfbxCurrentTimes[m_nAnimationStack] = m_pfbxStartTimes[m_nAnimationStack];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CGameObject::CGameObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}

	if (m_pfbxScene) ::ReleaseMeshFromFbxNodeHierarchy(m_pfbxScene->GetRootNode());
	if (m_pfbxScene) m_pfbxScene->Destroy();

	if (m_pAnimationController) delete m_pAnimationController;
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();

	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();

}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();

	m_pMesh = pMesh;

	if (m_pMesh) m_pMesh->AddRef();
}
void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{

}

void CGameObject::OnPrepareRender()
{

}

void CGameObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_pfbxScene) {
		FbxAMatrix fbxf4x4World = ::XmFloat4x4MatrixToFbxMatrix(m_xmf4x4World);
		::RenderFbxNodeHierarchy(pd3dCommandList, m_pfbxScene->GetRootNode(), m_pAnimationController->GetCurrentTime(), fbxf4x4World);
	}
	else {
		if (m_pShader)
		{
			//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
			m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
			m_pShader->Render(pd3dCommandList, pCamera);
		}
		if (m_pMesh) m_pMesh->Render(pd3dCommandList);
	}

	//if (m_pfbxScene) RenderFbxNodeHierarchy(pd3dCommandList, m_pfbxScene->GetRootNode(), m_pAnimationController->GetCurrentTime(), fbxf4x4World);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, FbxAMatrix* pfbxf4x4World)
{
	XMFLOAT4X4 xmf4x4World = ::FbxMatrixToXmFloat4x4Matrix(pfbxf4x4World);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3Position = pos;

	m_xmf4x4World._41 = pos.x;
	m_xmf4x4World._42 = pos.y;
	m_xmf4x4World._43 = pos.z;
}

void CGameObject::SetVelocity(XMFLOAT3 pos)
{
	m_xmf3Velocity = pos;
}

void CGameObject::SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents)
{	
	//m_pMesh->
}

void CGameObject::Move(XMFLOAT3 pos)
{
	SetPosition(Vector3::Add(GetPosition(), pos));
}

void CGameObject::Move()
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{

}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}
  
CBox::CBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);

	SetMesh(pCubeMeshTex);
}

CBox::~CBox()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//

CTree::CTree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,  FbxManager* pfbxSdkManager, FbxScene* pfbxScene)
{
	m_pfbxScene = pfbxScene;
	if (!m_pfbxScene)
	{
		m_pfbxScene = ::LoadFbxSceneFromFile(pd3dDevice, pd3dCommandList, pfbxSdkManager, "resources/Fbx/lowpolytree.fbx");
		::CreateMeshFromFbxNodeHierarchy(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pfbxScene->GetRootNode());
	}
	m_pAnimationController = new CAnimationController(m_pfbxScene);

}

CTree::~CTree()
{

}

CAngrybotObject::CAngrybotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, FbxScene* pfbxScene)
{
	m_pfbxScene = pfbxScene;
	if (!m_pfbxScene)
	{
		m_pfbxScene = ::LoadFbxSceneFromFile(pd3dDevice, pd3dCommandList, pfbxSdkManager, "resources/Fbx/Angrybot.fbx");
		::CreateMeshFromFbxNodeHierarchy(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pfbxScene->GetRootNode());
	}
	m_pAnimationController = new CAnimationController(m_pfbxScene);
}

CAngrybotObject::~CAngrybotObject()
{
}

