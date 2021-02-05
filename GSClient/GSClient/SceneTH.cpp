#include "stdafx.h"
#include "SceneTH.h"
#include "Shader.h"
#include "Enemy.h"
#include "Player.h"

CSceneTH::CSceneTH()
{
	m_pd3dGraphicsRootSignature = NULL;
}

CSceneTH::~CSceneTH()
{

}

void CSceneTH::Update(double elapsedTime)
{
	ProcessInput();

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
			m_ppObjects[j]->Update();
	}
}

void CSceneTH::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
			m_ppObjects[j]->Draw(pd3dCommandList, m_CurrentCamera);
	}

	for (int j = 0; j < m_nPlayers; j++)
	{
		if (m_ppPlayers[j])
			m_ppPlayers[j]->Draw(pd3dCommandList, m_CurrentCamera);
	}
}

void CSceneTH::ProcessInput()
{
	auto gameInput = GAME_INPUT;
	if (gameInput.KEY_W)
	{
		m_ppPlayers[0]->Move({ 0, 0, +0.3 });
	}
	if (gameInput.KEY_A)
	{
		m_ppPlayers[0]->Move({ -0.3, 0, 0 });
	}
	if (gameInput.KEY_S)
	{
		m_ppPlayers[0]->Move({ 0, 0, -0.3 });
	}
	if (gameInput.KEY_D)
	{
		m_ppPlayers[0]->Move({ +0.3, 0, 0 });
	}


	if (gameInput.KEY_1)
	{
		m_CurrentCamera = m_Cameras[0];
	}
	if (gameInput.KEY_2)
	{
		m_CurrentCamera = m_Cameras[1];
	}

	m_CurrentCamera->UpdateViewMatrix();
}

void CSceneTH::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(CFramework::GetInstance().GetHWND());
}

void CSceneTH::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CSceneTH::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

		m_CurrentCamera->Pitch(dy);
		m_CurrentCamera->RotateY(dx);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}
void CSceneTH::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_nObjects = 5;
	m_ppObjects = new CGameObject * [m_nObjects];

	m_nPlayers = 1;
	m_ppPlayers = new CPlayer * [m_nPlayers];

	m_pfbxManager = FbxManager::Create();
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	m_pfbxManager->SetIOSettings(m_pfbxIOs);

	//메쉬 =============================================================================
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		5.0f, 5.0f, 5.0f);

	CCubeMeshDiffused* pPlatformMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		500.0f, 0.0f, 500.0f);

	//CMeshFbx* treeMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/TestTree.fbx");

	//셰이더 ============================================================================
	CDiffusedShader* pDiffusedShader = new CDiffusedShader();
	pDiffusedShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pDiffusedShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CPlayerShader* pPlayerShader = new CPlayerShader();
	pPlayerShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pPlayerShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//플레이어 ==========================================================================
	CPlayer* player1 = new CPlayer();
	player1->SetMesh(pCubeMesh);
	player1->SetShader(pPlayerShader);

	m_ppPlayers[0] = player1;
	m_ppPlayers[0]->SetPosition({ 0, 0, -15 });

	//오브젝트 ==========================================================================
	CGameObject* platform = new CGameObject();
	platform->SetMesh(pPlatformMesh);
	platform->SetShader(pDiffusedShader);

	CEnemy* emey1 = new CEnemy(player1);
	emey1->SetMesh(pCubeMesh);
	emey1->SetShader(pDiffusedShader);

	CEnemy* emey2 = new CEnemy(player1);
	emey2->SetMesh(pCubeMesh);
	emey2->SetShader(pDiffusedShader);

	CTree* tree1 = new CTree(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pfbxManager, m_pfbxScene);

	CAngrybotObject* bot1 = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pfbxManager, m_pfbxScene);


	m_ppObjects[0] = platform;
	m_ppObjects[0]->SetPosition({ 0, 200000.5, 0 });
	m_ppObjects[1] = emey1;
	m_ppObjects[1]->SetPosition({ -10, 0, 20 });
	m_ppObjects[2] = emey2;
	m_ppObjects[2]->SetPosition({ +10, 0, 20 });
	m_ppObjects[3] = tree1;
	m_ppObjects[3]->SetPosition({ 0, 0, 0 });
	m_ppObjects[3]->SetAnimationStack(0);
	m_ppObjects[3]->m_pAnimationController->SetPosition(0, 0.0f);
	m_ppObjects[4] = bot1;
	m_ppObjects[4]->SetPosition({ 0, 0, 0 });
	m_ppObjects[4]->SetAnimationStack(0);
	m_ppObjects[4]->m_pAnimationController->SetPosition(0, 0.0f);

}

void CSceneTH::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				m_ppObjects[j]->ReleaseUploadBuffers();
	}

	if (m_ppPlayers)
	{
		for (int j = 0; j < m_nPlayers; j++)
			if (m_ppPlayers[j])
				m_ppPlayers[j]->ReleaseUploadBuffers();
	}
}

void CSceneTH::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
	if (m_ppPlayers)
	{
		for (int j = 0; j < m_nPlayers; j++)
			if (m_ppPlayers[j])
				delete m_ppPlayers[j];
		delete[] m_ppPlayers;
	}

	m_pfbxIOs->Destroy();
	m_pfbxScene->Destroy();
	m_pfbxManager->Destroy();
}

void CSceneTH::BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	int width, int height)
{
	int nCameras = 2;
	m_Cameras = new CCamera * [nCameras];
	for (int i = 0; i < nCameras; ++i)
	{
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, width, height, 1.0f, 5000.0f);
		pCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
		pCamera->SetScissorRect(0, 0, width, height);
		pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_Cameras[i] = pCamera;
	}
	m_Cameras[0]->SetPosition(0.0f, 10.0f, -50.0f);
	m_Cameras[1]->SetPosition(0.0f, 100.0f, -100.0f);

	m_CurrentCamera = m_Cameras[0];
}

ID3D12RootSignature* CSceneTH::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	/*pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;*/
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
		(void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CSceneTH::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}