#include "stdafx.h"
#include "GameScene.h"
#include "Shader.h"
#include "GameObject.h"
#include "Camera.h"

CGameScene::CGameScene()
{
	m_pd3dGraphicsRootSignature = NULL;
}

CGameScene::~CGameScene()
{
}

void CGameScene::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	BuildObjects(pd3dDevice, pd3dCommandList);
}

void CGameScene::BuildCamera(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, 
	int width, int height)
{
	int nCameras = 5;
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
	m_Cameras[0]->SetPosition(0.0f, 10.0f, -150.0f);
	m_Cameras[1]->SetPosition(1000.0f, 10.0f, -150.0f);
	m_Cameras[2]->SetPosition(-1000.0f, 10.0f, -150.0f);
	m_Cameras[3]->SetPosition(0.0f, 1010.0f, -150.0f);
	m_Cameras[4]->SetPosition(0.0f, -1010.0f, -150.0f);

	m_CurrentCamera = m_Cameras[0];
}

void CGameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{ 
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 200.0f,10.0f,200.0f );
	
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 200.0f, 10.0f, 200.0f);

	m_nObjects = 8;
	m_ppObjects = new CGameObject * [m_nObjects];
	 
	CShader* pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders.hlsl", "VSTextured");
	pShader->CreatePixelShader(L"Shaders.hlsl", "PSTextured");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	
	// 추가 스카이박스 
	CTexturedRectMesh* pSkyBoxMesh_Front = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Back = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Left = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Right = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, 10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Top = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f, +10.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Bottom = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f,-10.0f, +0.0f);

	/*pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders.hlsl", "VSTextured");
	pShader->CreatePixelShader(L"Shaders.hlsl", "PSTextured");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);*/

	for (int i = 0; i < m_nObjects; ++i)
	{
		CRotatingObject* pObject = new CRotatingObject();

		//pObject->SetMesh(pCubeMeshTex);
		//pObject->SetShader(pShader);
	
		m_ppObjects[i] = pObject;
		
	}
	// 스카이박스
	m_ppObjects[0]->SetMesh(pSkyBoxMesh_Front);
	m_ppObjects[0]->SetTextureIndex(0x01);
	m_ppObjects[0]->SetShader(pShader);
	
	m_ppObjects[1]->SetMesh(pSkyBoxMesh_Back);
	m_ppObjects[1]->SetTextureIndex(0x02);
	m_ppObjects[1]->SetShader(pShader);

	m_ppObjects[2]->SetMesh(pSkyBoxMesh_Left);
	m_ppObjects[2]->SetTextureIndex(0x03);
	m_ppObjects[2]->SetShader(pShader);

	m_ppObjects[3]->SetMesh(pSkyBoxMesh_Right);
	m_ppObjects[3]->SetTextureIndex(0x04);
	m_ppObjects[3]->SetShader(pShader);

	m_ppObjects[4]->SetMesh(pSkyBoxMesh_Top);
	m_ppObjects[4]->SetTextureIndex(0x05);
	m_ppObjects[4]->SetShader(pShader);

	m_ppObjects[5]->SetMesh(pSkyBoxMesh_Bottom);
	m_ppObjects[5]->SetTextureIndex(0x06);
	m_ppObjects[5]->SetShader(pShader);

	// 지형
	m_ppObjects[6]->SetPosition({ 0,  0,  0 }); 
	m_ppObjects[6]->SetTextureIndex(0x00);
	m_ppObjects[6]->SetShader(pShader);

}

void CGameScene::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto boxTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, boxTex.get(), "Box", L"resources/OBJ/rockygrass00.dds");

	auto SkyTex_Front = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Front.get(), "Sky_Front", L"resources/OBJ/SkyBox_Front_0.dds");

	auto SkyTex_Back = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Back.get(), "Sky_Back", L"resources/OBJ/SkyBox_Back_0.dds");

	auto SkyTex_Left = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Left.get(), "Sky_Left", L"resources/OBJ/SkyBox_Left_0.dds");

	auto SkyTex_Right = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Right.get(), "Sky_Right", L"resources/OBJ/SkyBox_Right_0.dds");

	auto SkyTex_Top = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Top.get(), "Sky_Top", L"resources/OBJ/SkyBox_Top_0.dds");

	auto SkyTex_Bottom = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Bottom.get(), "Sky_Bottom", L"resources/OBJ/SkyBox_Bottom_0.dds");

	m_Textures[boxTex->m_Name] = std::move(boxTex);
	m_Textures[SkyTex_Back->m_Name] = std::move(SkyTex_Back);
	m_Textures[SkyTex_Front->m_Name] = std::move(SkyTex_Front);
	m_Textures[SkyTex_Left->m_Name] = std::move(SkyTex_Left);
	m_Textures[SkyTex_Right->m_Name] = std::move(SkyTex_Right);
	m_Textures[SkyTex_Top->m_Name] = std::move(SkyTex_Top);
	m_Textures[SkyTex_Bottom->m_Name] = std::move(SkyTex_Bottom);
}

void CGameScene::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dSrvDescriptorHeap));

	//
	// Fill out the heap with actual descriptors.
	//
	D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = m_pd3dSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	auto boxTex = m_Textures["Box"]->m_pd3dResource;
	auto SkyTex_Front = m_Textures["Sky_Front"]->m_pd3dResource;
	auto SkyTex_Back = m_Textures["Sky_Back"]->m_pd3dResource;
	auto SkyTex_Left = m_Textures["Sky_Left"]->m_pd3dResource;
	auto SkyTex_Right = m_Textures["Sky_Right"]->m_pd3dResource;
	auto SkyTex_Top = m_Textures["Sky_Top"]->m_pd3dResource;
	auto SkyTex_Bottom = m_Textures["Sky_Bottom"]->m_pd3dResource;

	
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = boxTex->GetDesc().Format;//boxTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	pd3dDevice->CreateShaderResourceView(boxTex, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Front->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Front, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Back->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Back, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Left->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Left, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Right->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Right, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Top->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Top, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Bottom->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Bottom, &srvDesc, hDescriptor);
	
}

void CGameScene::ReleaseObjects()
{
	m_ppObjects[1];

	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void CGameScene::Update(double elapsedTime)
{
	ProcessInput();
}

void CGameScene::AnimateObjects(float fTimeElapsed)
{
}

void CGameScene::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT3 xmf3CameraPos = m_CurrentCamera->GetPosition3f();
	
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, tex);
	
	for (int i = 0; i < 6; i++)
	{
		m_ppObjects[i]->SetPosition(xmf3CameraPos);
	}
	//씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다.
	for (int j = 0; j < m_nObjects; j++)
	{

		if (m_ppObjects[j])
			m_ppObjects[j]->Draw(pd3dCommandList, m_CurrentCamera);
	}
}

void CGameScene::ProcessInput()
{
	if (m_CurrentCamera == nullptr) return;

	auto keyInput = GAME_INPUT;
	if (keyInput.KEY_W)
	{
		m_CurrentCamera->Walk(1.0f);
	}
	if (keyInput.KEY_A)
	{
		m_CurrentCamera->Strafe(-1.0f);
	}
	if (keyInput.KEY_S)
	{
		m_CurrentCamera->Walk(-1.0f);
	}
	if (keyInput.KEY_D)
	{
		m_CurrentCamera->Strafe(1.0f);
	}

	if (keyInput.KEY_1)
	{
		m_CurrentCamera = m_Cameras[0];
	}
	if (keyInput.KEY_2)
	{
		m_CurrentCamera = m_Cameras[1];
	}
	if (keyInput.KEY_3)
	{
		m_CurrentCamera = m_Cameras[2];
	}
	if (keyInput.KEY_4)
	{
		m_CurrentCamera = m_Cameras[3];
	}
	if (keyInput.KEY_5)
	{
		m_CurrentCamera = m_Cameras[4];
	}

	m_CurrentCamera->UpdateViewMatrix();
}

void CGameScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(CFramework::GetInstance().GetHWND());
}

void CGameScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CGameScene::OnMouseMove(WPARAM btnState, int x, int y)
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

void CGameScene::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* CGameScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// Ground
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1]; 
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 7;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 22;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

#pragma region sampler
	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[1];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

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