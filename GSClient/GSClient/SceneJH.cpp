#include "stdafx.h"
#include "SceneJH.h"
#include "stdafx.h" 
#include "GameFramework.h"
#include "Shader.h"
#include "UI.h"
#include "Camera.h"

#define ROOT_PARAMETER_OBJECT			0
#define ROOT_PARAMETER_CAMERA			1
#define ROOT_PARAMETER_MATERIAL			2
#define ROOT_PARAMETER_LIGHT			3
#define ROOT_PARAMETER_TEXTURE			4

CSceneJH::CSceneJH()
{
	m_pd3dGraphicsRootSignature = NULL;
}

CSceneJH::~CSceneJH()
{
}

void CSceneJH::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height)
{
	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	BuildMaterials(pd3dDevice, pd3dCommandList);
	BuildCamera(pd3dDevice, pd3dCommandList, width, height);
	BuildLights(pd3dDevice, pd3dCommandList);
	BuildObjects(pd3dDevice, pd3dCommandList);
}

void CSceneJH::BuildCamera(ID3D12Device* pd3dDevice,
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
	//m_Cameras[0]->SetPosition(0.0f, 300.0f, 0.0f);
	////m_Cameras[0]->RotateY(XMConvertToRadians(180));
	//m_Cameras[0]->Pitch(XMConvertToRadians(90));
	m_Cameras[0]->SetPosition(125.0f, 250.0f, 140.0f);
	m_Cameras[0]->RotateY(XMConvertToRadians(45));
	m_Cameras[0]->Pitch(XMConvertToRadians(65));
	m_Cameras[1]->SetPosition(1000.0f, 10.0f, -150.0f);
	m_Cameras[2]->SetPosition(-1000.0f, 10.0f, -150.0f);
	m_Cameras[3]->SetPosition(0.0f, 1010.0f, -150.0f);
	m_Cameras[4]->SetPosition(0.0f, -1010.0f, -150.0f);

	m_CurrentCamera = m_Cameras[0];
}

void CSceneJH::BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void**)&m_pcbMappedMaterials);

	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CSceneJH::BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 1000.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	if (m_CurrentCamera)
	{
		m_pLights->m_pLights[0].m_xmf3Position = m_CurrentCamera->GetPosition3f();
		m_pLights->m_pLights[0].m_xmf3Direction = m_CurrentCamera->GetLook3f();
		//m_CurrentCamera->SetLight(&m_pLights->m_pLights[0]);
	}
	else
	{
		m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 300.0f, -150.0f);
		m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 500.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	if (m_CurrentCamera)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_CurrentCamera->GetPosition3f();
		m_pLights->m_pLights[1].m_xmf3Direction = m_CurrentCamera->GetLook3f();
		m_CurrentCamera->SetLight(&m_pLights->m_pLights[1]);
	}
	else
	{
		m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(0.0f, 300.0f, -150.0f);
		m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 16.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);

	//m_pLights->m_pLights[3].m_bEnable = true;
	//m_pLights->m_pLights[3].m_nType = DIRECTIONAL_LIGHT;
	//m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	//m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	//m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	//m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 600.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.577903390, -0.577350020, 0.576796055);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}

void CSceneJH::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 지형 메쉬 
	CTerrainMesh* pPlaneMeshTex = new CTerrainMesh(pd3dDevice, pd3dCommandList, 0, 0, 1000, 1000);
	CTerrainWayMesh* pEdgeMeshTex = new CTerrainWayMesh(pd3dDevice, pd3dCommandList, 0, 0, 100, 100);

	m_nObjects = 10;
	m_ppObjects = new CGameObject * [m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CGameObject* pObject = new CGameObject();

		m_ppObjects[i] = pObject;
	}

	CShader* pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateVertexShader(L"Shaders\\JHTestShader.hlsl", "VSTextured");
	pSkyBoxShader->CreatePixelShader(L"Shaders\\JHTestShader.hlsl", "PSTextured");
	pSkyBoxShader->CreateInputLayout(ShaderTypes::Textured);
	pSkyBoxShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	m_Skybox = new CSkyBox(pd3dDevice, pd3dCommandList, pSkyBoxShader);

	CShader* pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders\\JHTestShader.hlsl", "VSTextured");
	pShader->CreatePixelShader(L"Shaders\\JHTestShader.hlsl", "PSTextured");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

#pragma region Create Terrain
	// 지형
	m_ppObjects[0]->SetMesh(pPlaneMeshTex);
	m_ppObjects[0]->SetPosition({ 0,  0,  0 });
	m_ppObjects[0]->SetTextureIndex(0x01);
	m_ppObjects[0]->SetShader(pShader);

	//m_ppObjects[1]->SetMesh(pPlaneMeshTex);
	//m_ppObjects[1]->SetPosition({ 500,  0,  0 });
	//m_ppObjects[1]->SetTextureIndex(0x01);
	//m_ppObjects[1]->SetShader(pShader);
	//
	//m_ppObjects[2]->SetMesh(pPlaneMeshTex);
	//m_ppObjects[2]->SetPosition({ 0,  0,  500 });
	//m_ppObjects[2]->SetTextureIndex(0x01);
	//m_ppObjects[2]->SetShader(pShader);
	//
	//m_ppObjects[3]->SetMesh(pPlaneMeshTex);
	//m_ppObjects[3]->SetPosition({ 500,  0,  500 });
	//m_ppObjects[3]->SetTextureIndex(0x01);
	//m_ppObjects[3]->SetShader(pShader);
	//
	////// 아래
	//m_ppObjects[4]->SetMesh(pEdgeMeshTex);
	//m_ppObjects[4]->SetPosition({ -800,  -20,  0 });
	//m_ppObjects[4]->Rotate(XMFLOAT3(0, 1, 0), 90);
	//m_ppObjects[4]->Rotate(XMFLOAT3(0, 0, 1), 30);
	//m_ppObjects[4]->SetTextureIndex(0x01);
	//m_ppObjects[4]->SetShader(pShader);
	//
	//// 왼쪽
	//m_ppObjects[6]->SetMesh(pEdgeMeshTex);
	//m_ppObjects[6]->SetPosition({ 0,  -20,  1700 });
	//m_ppObjects[6]->Rotate(XMFLOAT3(0, 1, 0), 180);
	//m_ppObjects[6]->Rotate(XMFLOAT3(0, 0, 1), 30);
	//m_ppObjects[6]->SetTextureIndex(0x01);
	//m_ppObjects[6]->SetShader(pShader);
	//
	////// 오른쪽
	//m_ppObjects[7]->SetMesh(pEdgeMeshTex);
	//m_ppObjects[7]->SetPosition({ 1810,  470,  1700 });
	//m_ppObjects[7]->Rotate(XMFLOAT3(0, 1, 0), 180);
	//m_ppObjects[7]->Rotate(XMFLOAT3(0, 0, 1), -30);
	//m_ppObjects[7]->SetTextureIndex(0x01);
	//m_ppObjects[7]->SetShader(pShader);
	//
	////// 모서리 위
	//m_ppObjects[8]->SetMesh(pEdgeMeshTex);
	//m_ppObjects[8]->SetPosition({ -1200,  470,  1820 });
	//m_ppObjects[8]->Rotate(XMFLOAT3(0, 1, 0), 90);
	//m_ppObjects[8]->Rotate(XMFLOAT3(0, 0, 1), -30);
	//m_ppObjects[8]->SetTextureIndex(0x01);
	//m_ppObjects[8]->SetShader(pShader);

#pragma endregion  

	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders\\TerrainAndLight.hlsl", "VSTexturedLighting");
	pShader->CreatePixelShader(L"Shaders\\TerrainAndLight.hlsl", "PSTexturedLighting");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	CBox* pBox = new CBox(pd3dDevice, pd3dCommandList, 50.0f, 50.0f, 50.0f);
	pBox->SetShader(pShader);
	pBox->SetObjectName(OBJ_NAME::Box);

	m_ppObjects[5] = pBox;
	m_ppObjects[5]->SetPosition({ 250,  25, 250 });
	m_ppObjects[5]->SetTextureIndex(0x80); 
}

void CSceneJH::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto terrainTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, terrainTex.get(), "Terrain", L"resources/OBJ/Terrain.dds");

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

	auto boxTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, boxTex.get(), "Box", L"resources/OBJ/Box.dds");

	m_Textures[terrainTex->m_Name] = std::move(terrainTex);
	m_Textures[SkyTex_Back->m_Name] = std::move(SkyTex_Back);
	m_Textures[SkyTex_Front->m_Name] = std::move(SkyTex_Front);
	m_Textures[SkyTex_Left->m_Name] = std::move(SkyTex_Left);
	m_Textures[SkyTex_Right->m_Name] = std::move(SkyTex_Right);
	m_Textures[SkyTex_Top->m_Name] = std::move(SkyTex_Top);
	m_Textures[SkyTex_Bottom->m_Name] = std::move(SkyTex_Bottom);
	m_Textures[boxTex->m_Name] = std::move(boxTex);
}

void CSceneJH::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	auto terrainTex = m_Textures["Terrain"]->m_pd3dResource;
	auto SkyTex_Front = m_Textures["Sky_Front"]->m_pd3dResource;
	auto SkyTex_Back = m_Textures["Sky_Back"]->m_pd3dResource;
	auto SkyTex_Left = m_Textures["Sky_Left"]->m_pd3dResource;
	auto SkyTex_Right = m_Textures["Sky_Right"]->m_pd3dResource;
	auto SkyTex_Top = m_Textures["Sky_Top"]->m_pd3dResource;
	auto SkyTex_Bottom = m_Textures["Sky_Bottom"]->m_pd3dResource;
	auto boxTex = m_Textures["Box"]->m_pd3dResource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = terrainTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	pd3dDevice->CreateShaderResourceView(terrainTex, &srvDesc, hDescriptor);

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

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = boxTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(boxTex, &srvDesc, hDescriptor);
}

void CSceneJH::ReleaseObjects()
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

void CSceneJH::Update(double elapsedTime)
{
	ProcessInput();

	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->Update(elapsedTime);
	}

	if (m_CurrentCamera) m_CurrentCamera->Update(elapsedTime);
}

void CSceneJH::AnimateObjects(float fTimeElapsed)
{
}

void CSceneJH::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE, tex);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights

	m_Skybox->Draw(pd3dCommandList, m_CurrentCamera);

	//씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다.
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
			m_ppObjects[j]->Draw(pd3dCommandList, m_CurrentCamera);
	}
}

void CSceneJH::ProcessInput()
{
	if (m_CurrentCamera == nullptr) return;

	float cameraSpeed = m_CurrentCamera->GetSpeed();

	auto keyInput = GAME_INPUT;
	if (keyInput.KEY_W)
	{
		m_CurrentCamera->Walk(cameraSpeed);
	}
	if (keyInput.KEY_A)
	{
		m_CurrentCamera->Strafe(-cameraSpeed);
	}
	if (keyInput.KEY_S)
	{
		m_CurrentCamera->Walk(-cameraSpeed);
	}
	if (keyInput.KEY_D)
	{
		m_CurrentCamera->Strafe(cameraSpeed);
	}
	if (keyInput.KEY_B)
	{
		m_CurrentCamera->SetShake(true, 0.5f, 5.0f);
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
	if (keyInput.KEY_ADD)
	{
		m_CurrentCamera->SetSpeed(min(cameraSpeed + 1.0f, 15.0f));
	}
	if (keyInput.KEY_SUBTRACT)
	{
		m_CurrentCamera->SetSpeed(max(cameraSpeed - 1.0f, 1.0f));
	}

	m_CurrentCamera->UpdateViewMatrix();
}

void CSceneJH::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(CFramework::GetInstance().GetHWND());
}

void CSceneJH::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CSceneJH::OnMouseMove(WPARAM btnState, int x, int y)
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

void CSceneJH::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* CSceneJH::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// Ground
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = m_Textures.size();
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 18;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; //Material
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Light
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

#pragma region sampler
	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

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

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
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