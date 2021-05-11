#include "stdafx.h"
#include "SceneYJ.h"

#include "stdafx.h" 
#include "GameFramework.h"
#include "Shader.h"
#include "UI.h"
#include "Camera.h"
#include "Player.h"
#include "Bridge.h"
#include "Wall.h"
#include "Communicates.h"
#include "Terrain.h"
#include "Sign.h"
#include "Puzzle.h"

#define ROOT_PARAMETER_OBJECT				0
#define ROOT_PARAMETER_SCENE_FRAME_DATA		1
#define ROOT_PARAMETER_CAMERA				2
#define ROOT_PARAMETER_LIGHT_CAMERA			3
#define ROOT_PARAMETER_MATERIAL				4
#define ROOT_PARAMETER_LIGHT				5
#define ROOT_PARAMETER_TEXTURE				6


D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor;
D3D12_GPU_DESCRIPTOR_HANDLE srvGpuStart;
D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuStart;
XMFLOAT3 LightPos;
float lensize=60000.0f;

CSceneYJ::CSceneYJ()
{
	cout << "Enter CSceneYJ \n";
	m_pd3dGraphicsRootSignature = NULL;
	m_isPlayerSelected = false;
}

CSceneYJ::~CSceneYJ()
{
}

void CSceneYJ::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height)
{
	BuildMirrorResource(pd3dDevice);
	BuildMinimapResource(pd3dDevice);
	BuildShadowResource(pd3dDevice);
	
	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CShaderHandler::GetInstance().SetUserID(ShaderHandlerUser::YJ);
	CShaderHandler::GetInstance().CreateAllShaders(pd3dDevice, m_pd3dGraphicsRootSignature);

	BuildMaterials(pd3dDevice, pd3dCommandList);
	CreateLightCamera(pd3dDevice, pd3dCommandList, width, height);
	BuildCamera(pd3dDevice, pd3dCommandList, width, height);
	BuildLights(pd3dDevice, pd3dCommandList);
	BuildSceneFrameData(pd3dDevice, pd3dCommandList);
	BuildObjects(pd3dDevice, pd3dCommandList);
	BuildUIs(pd3dDevice, pd3dCommandList);
}

void CSceneYJ::BuildCamera(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	int width, int height)
{
	int nCameras = 6;
	m_Cameras = new CCamera * [nCameras];
	for (int i = 0; i < nCameras; ++i)
	{
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, width, height, 1.0f, 60000.0f);
		pCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
		pCamera->SetScissorRect(0, 0, width, height);
		pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_Cameras[i] = pCamera;
	}
	m_Cameras[0]->SetPosition({ 0,  0, 0 }
		
	);
	m_Cameras[0]->Pitch(XMConvertToRadians(15));
	m_Cameras[0]->SetOffset(XMFLOAT3(0.0f, 70.0f, -300.0f));

	m_Cameras[1]->SetPosition({ 0,  0, 0 });
	m_Cameras[1]->Pitch(XMConvertToRadians(90));

	m_Cameras[2]->SetPosition({ 0,  0, 0 }); 
	m_Cameras[3]->SetPosition({ 0, 0, 0 });
	//m_Cameras[3]->Pitch(XMConvertToRadians(90));
	m_Cameras[4]->SetPosition({ 0,0,0 });

	XMFLOAT3 xmf3Look = XMFLOAT3(1.0f, -1.0f, 0.0f);

	XMFLOAT3 xmf3Right = XMFLOAT3(0.0f, 0.0f, -1.0f);

	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_Cameras[5]->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_Cameras[5]->SetLens(0.25f * PI, width, height, 1.0f, 60000.0f);
	m_Cameras[5]->SetRight(xmf3Right);
	m_Cameras[5]->SetUp(xmf3Up);
	m_Cameras[5]->SetLook(xmf3Look);
	
	//m_Cameras[5]->SetPosition({ -500, 2000, 2000 });
	m_Cameras[5]->SetPosition({ 00, 00, 0 });
	m_Cameras[5]->SetViewport(0, 0, width, height, 0.0f, 1.0f);
	m_Cameras[5]->SetScissorRect(0, 0, width, height);
	

	m_ShadowCamera = m_Cameras[4];
	m_MirrorCamera = m_Cameras[3];
	m_CurrentCamera = m_Cameras[5];
	
	m_MinimapCamera = m_Cameras[1];
}
void CSceneYJ::CreateLightCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nHeight)
{
	XMFLOAT3 xmf3Look = XMFLOAT3(1.0f, -1.0f, 0.0f);

	XMFLOAT3 xmf3Right = XMFLOAT3(0.0f, 0.0f, -1.0f);

	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_pLightCamera = new CLightCamera();

	m_pLightCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pLightCamera->SetLens(0.25f * PI, nWidth, nHeight, 1.0f, lensize);
	m_pLightCamera->SetRight(xmf3Right);
	m_pLightCamera->SetUp(xmf3Up);
	m_pLightCamera->SetLook(xmf3Look);
	//m_pLightCamera->SetPosition(XMFLOAT3(10000.0f, 1300.0f, 0.0f));
	m_pLightCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pLightCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pLightCamera->SetScissorRect(0, 0, nWidth, nHeight);

	
	m_pLightCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CSceneYJ::BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

void CSceneYJ::BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);

	m_pLights->m_pLights[1].m_bEnable = false;
	m_pLights->m_pLights[1].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 500.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}

void CSceneYJ::BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbMaterialBytes = ((sizeof(CB_GAMESCENE_FRAME_DATA) + 255) & ~255); //256의 배수
	m_pd3dcbSceneInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		NULL, ncbMaterialBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbSceneInfo->Map(0, NULL, (void**)&m_pcbMappedSceneFrameData);
}

void CSceneYJ::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pfbxManager = FbxManager::Create();
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	m_pfbxManager->SetIOSettings(m_pfbxIOs);

	m_Objects.reserve(30);

	m_Skybox = new CSkyBox(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("SkyBox"));
	m_Terrain = new CTerrain(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("Terrain"));
	 
	//BuildMapSector1(pd3dDevice, pd3dCommandList);  
	//BuildMapSector2(pd3dDevice, pd3dCommandList);
	//BuildMapSector3(pd3dDevice, pd3dCommandList);
	//BuildMapSector4(pd3dDevice, pd3dCommandList);
	//BuildMapSector5(pd3dDevice, pd3dCommandList);

	BuildBridges(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("Bridge"));

	BuildDoorWall(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("DoorWall"));
	BuildPuzzles(pd3dDevice, pd3dCommandList);
	BuildEnemys(pd3dDevice, pd3dCommandList);
	BuildSigns(pd3dDevice, pd3dCommandList);
	BuildMirror(pd3dDevice, pd3dCommandList);
	

	CMeshFbx* fbxMesh;

	fbxMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/Golem.fbx");
	m_Player = new CPlayer(pd3dDevice, pd3dCommandList);

	m_Cameras[0]->SetOffset(XMFLOAT3(0.0f, 450.0f, -500.0f));
	//m_Cameras[0]->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_Cameras[0]->SetTarget(m_Player);

	
	m_Player->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	
	m_Player->Scale(50, 50, 50);
	m_Player->SetObjectName(OBJ_NAME::Player);
	//m_Player->SetPosition({ 750,  230, 1850 });
	m_Player->SetPosition({ 0, 0, 0 });
	m_Player->SetCamera(m_Cameras[0]);
	
	//m_Player->SetCamera(light_cam)
	m_Player->SetTextureIndex(0x80);
	m_Player->SetMesh(fbxMesh);
	m_Player->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 10, 10, 10, XMFLOAT3{ 0,0,0 });

	LightPos = m_Player->GetPosition();

	m_MinimapCamera->SetTarget(m_Player);
	
}

void CSceneYJ::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	const char* keyNames[] =
	{
		"Forest","Dry_Forest","Desert","Dry_Desert","Rocky_Terrain","BossWall",
		"Sky_Front","Sky_Back", "Sky_Left", "Sky_Right","Sky_Top","Sky_Bottom", 
		"Box","Wood", "WoodSignBoard",
		"GrassWall", "SandWall","RockyWall",
		"Door",
		"HP_SP","Minimap","WeaponUI",
		"HP_SP_PER",
		"Flower_Red","Flower_White","Grass_1","Grass_2","Tree","NoLeafTree","Leaves","Moss_Rock",
		"PuzzleBoard",
		"HelpText",
		"Dry_Tree","Stump","Dead_Tree",
		"Desert_Rock" 
	};

	const wchar_t* address[] =
	{
		L"resources/OBJ/Forest.dds",L"resources/OBJ/Dry_Forest.dds",L"resources/OBJ/Desert.dds",
		L"resources/OBJ/Dry_Desert.dds",L"resources/OBJ/Rocky_Terrain.dds",L"resources/OBJ/bossWall.dds",
		L"resources/skybox/front.dds",L"resources/skybox/back.dds", L"resources/skybox/left.dds",L"resources/skybox/right.dds",L"resources/skybox/top.dds", L"resources/skybox/bottom.dds", 
		L"resources/OBJ/Box.dds",
		L"resources/OBJ/Wood.dds", L"resources/OBJ/WoodSignBoard.dds",
		L"resources/OBJ/GrassWallTexture.dds", L"resources/OBJ/StoneWallTexture.dds",L"resources/OBJ/RockyWall.dds",
		L"resources/OBJ/Door.dds",
		L"resources/UI/HP_SP.dds", L"resources/UI/Minimap.dds", L"resources/UI/Weapon.dds",L"resources/UI/SmallICons.dds",
		L"resources/Billboard/Flower01.dds",L"resources/Billboard/Flower02.dds",L"resources/Billboard/Grass01.dds",L"resources/Billboard/Grass02.dds",
		L"resources/Billboard/Tree02.dds",L"resources/Billboard/NoLeafTree2.dds",L"resources/OBJ/Leaves.dds",L"resources/OBJ/ROck_Texture_Surface2.dds",
		L"resources/OBJ/Board.dds",
		L"resources/UI/HelpText.dds",
		L"resources/OBJ/Dry_Tree.dds",L"resources/OBJ/Stump.dds",L"resources/OBJ/Dead_Tree.dds",
		L"resources/OBJ/Desert_Rock.dds"
	};

	for (int i = 0; i < _countof(keyNames); ++i)
	{
		unique_ptr<CTexture> tempTex = make_unique<CTexture>();
		MakeTexture(pd3dDevice, pd3dCommandList, tempTex.get(), keyNames[i], address[i]);
		m_Textures[tempTex->m_Name] = std::move(tempTex);
	}
}

void CSceneYJ::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Create the SRV heap. 
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size() + 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dSrvDescriptorHeap));

	// Add +1 DSV for shadow map.
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(&m_pd3dDsvDescriptorHeap));

	// Fill out the heap with actual descriptors. 
	hDescriptor = m_pd3dSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	srvGpuStart = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	dsvCpuStart = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	const char* keyNames[] =
	{
		"Forest","Dry_Forest","Desert","Dry_Desert","Rocky_Terrain","BossWall",
		"Sky_Front", "Sky_Back", "Sky_Left", "Sky_Right", "Sky_Top","Sky_Bottom",
		"Box","Wood", "WoodSignBoard",
		"GrassWall", "SandWall","RockyWall",
		"Door",
		"HP_SP","HP_SP_PER",
		"Minimap",
		"WeaponUI",
		"Flower_Red","Flower_White","Grass_1","Grass_2","Tree","NoLeafTree","Leaves","Moss_Rock",
		"PuzzleBoard",
		"HelpText",
		"Dry_Tree","Stump","Dead_Tree",
		"Desert_Rock"
	};

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	for (int i = 0; i < _countof(keyNames); ++i)
	{
		if (i != 0)	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
		ID3D12Resource* texResource = m_Textures[keyNames[i]]->m_pd3dResource;
		srvDesc.Format = texResource->GetDesc().Format;

		pd3dDevice->CreateShaderResourceView(texResource, &srvDesc, hDescriptor);
	}
	 
	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = m_pd3dMinimapTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(m_pd3dMinimapTex, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = m_pd3dMirrorTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(m_pd3dMirrorTex, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	pd3dDevice->CreateShaderResourceView(m_pd3dShadowMap, &srvDesc, hDescriptor);

	// 그림자 깊이값
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	pd3dDevice->CreateDepthStencilView(m_pd3dShadowMap, &dsvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, gnDsvDescriptorIncrementSize));

	m_d3dDsvShadowMapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, gnDsvDescriptorIncrementSize);
	m_d3dSrvShadowMapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_Textures.size() + 3, gnCbvSrvDescriptorIncrementSize);

}

void CSceneYJ::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	m_Objects.clear();
}



void CSceneYJ::Update(float elapsedTime)
{
	ProcessInput();

	m_Skybox->Rotate(XMFLOAT3(0, 1, 0), 0.3 * elapsedTime);
	
	if (m_pLights)
	{
		m_pLights->m_pLights[0].m_xmf3Position = m_CurrentCamera->GetPosition3f();
		m_pLights->m_pLights->m_xmf3Direction = m_CurrentCamera->GetLook3f();
	}
	
	for (auto pObject : m_Objects)
	{
		pObject->Update(elapsedTime);
	}
	m_HelpTextUI->Update(elapsedTime);

	m_Player->Update(elapsedTime);
	
	
	if (m_CurrentCamera) m_CurrentCamera->Update(elapsedTime);


	if (m_MirrorCamera)
	{
		m_MirrorCamera->UpdateViewMatrix();
	}

	if (m_pLightCamera)
	{
		
		//LightPos = m_Player->GetPosition();
		//LightPos.y = 2200.0f;
		//LightPos.z -= 100.0f;

		/*m_pLightCamera->LookAt(LightPos,
			m_Player->GetPosition(),
			m_Player->GetUp());*/

		m_pLightCamera->LookAt(LightPos,
			{ -10000.0f,0.0f,0.0f },
			m_Player->GetUp());

		m_pLightCamera->UpdateViewMatrix();
	}

	if (m_MinimapCamera)
	{
		XMFLOAT3 pos = m_Player->GetPosition();
		pos.y = m_MinimapCamera->GetPosition3f().y;
		pos.z += 1;

		m_MinimapCamera->LookAt(pos,
			m_Player->GetPosition(),
			m_Player->GetUp());
		m_MinimapCamera->UpdateViewMatrix();
	}
}

void CSceneYJ::AnimateObjects(float fTimeElapsed)
{
}

void CSceneYJ::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE, tex);

	m_pcbMappedSceneFrameData->m_PlayerHP = m_Player->GetHP();
	m_pcbMappedSceneFrameData->m_PlayerSP = m_Player->GetSP();
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights

	m_Skybox->Draw(pd3dCommandList, m_CurrentCamera);
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);
	m_Player->Draw_Shadow(pd3dCommandList, m_CurrentCamera);
	m_Player->Draw(pd3dCommandList, m_CurrentCamera);
	
	//m_Mirror->Draw(pd3dCommandList, m_CurrentCamera);
	

	/*for (auto pObject : m_BillboardObjects)
	{
		pObject->Draw(pd3dCommandList, m_CurrentCamera);
	}*/

	for (auto pObject : m_Objects)
	{
		pObject->Draw(pd3dCommandList, m_CurrentCamera);
	}


}

void CSceneYJ::DrawUI(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (UI* pUI : m_UIs)
	{
		pUI->Draw(pd3dCommandList, m_CurrentCamera);
	}

	for (int i = 0; i < m_Player->GetHP() / 5; ++i)
	{
		m_HPGauges[i]->Draw(pd3dCommandList, m_CurrentCamera);
	}

	for (int i = 0; i < m_Player->GetSP() / 5; ++i)
	{
		m_SPGauges[i]->Draw(pd3dCommandList, m_CurrentCamera);
	}
}

void CSceneYJ::DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_Player)
	{
		m_Player->Draw(pd3dCommandList, m_CurrentCamera);
	}
}

void CSceneYJ::DrawPlayer_Shadow(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_Player)
	{
		m_Player->Draw_Shadow(pd3dCommandList, m_CurrentCamera);
	}
}

void CSceneYJ::FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CSceneYJ::DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_MinimapCamera)
	{
		m_MinimapCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_MinimapCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE, tex);

	m_pcbMappedSceneFrameData->m_PlayerHP = m_Player->GetHP();
	m_pcbMappedSceneFrameData->m_PlayerSP = m_Player->GetSP();
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights

	m_Skybox->Draw(pd3dCommandList, m_MinimapCamera);
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);

	for (auto pObject : m_BillboardObjects)
	{
		pObject->Draw(pd3dCommandList, m_CurrentCamera);
	}
	for (auto pObject : m_Objects)
	{
		pObject->Draw(pd3dCommandList, m_CurrentCamera);
	}
	  
	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pd3dRTV,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dMinimapTex,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	// Copy the input (back-buffer) to MimapTexture
	pd3dCommandList->CopyResource(m_pd3dMinimapTex, pd3dRTV);

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dMinimapTex,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON));

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pd3dRTV,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}
}

void CSceneYJ::DrawMirror(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_MirrorCamera)
	{
		m_MirrorCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_MirrorCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE, tex);

	m_pcbMappedSceneFrameData->m_PlayerHP = m_Player->GetHP();
	m_pcbMappedSceneFrameData->m_PlayerSP = m_Player->GetSP();
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights


	m_Skybox->Draw(pd3dCommandList, m_MirrorCamera);
	m_Terrain->Draw(pd3dCommandList, m_MirrorCamera);
	m_Player->Draw(pd3dCommandList, m_MirrorCamera);

	for (auto pObject : m_BillboardObjects)
	{
		pObject->Draw(pd3dCommandList, m_MirrorCamera);
	}
	for (auto pObject : m_Objects)
	{
		pObject->Draw(pd3dCommandList, m_MirrorCamera);
	}

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pd3dRTV,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dMirrorTex,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	// Copy the input (back-buffer) to MimapTexture
	pd3dCommandList->CopyResource(m_pd3dMirrorTex, pd3dRTV);

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dMirrorTex,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON));

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pd3dRTV,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

	if (m_MirrorCamera)
	{
		m_MirrorCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_MirrorCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}
}

void CSceneYJ::DrawShadow(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_pLightCamera)
	{
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
		m_pLightCamera->GenerateViewMatrix();
		m_pLightCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_LIGHT_CAMERA);
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
	}

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dShadowMap,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	pd3dCommandList->ClearDepthStencilView(m_d3dDsvShadowMapCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	pd3dCommandList->OMSetRenderTargets(0, NULL, FALSE, &m_d3dDsvShadowMapCPUHandle);

	//m_Player->Draw(pd3dCommandList, m_pLightCamera);
	m_Player->Draw_Shadow(pd3dCommandList, m_pLightCamera);

	/*for (auto pObject : m_Objects)
	{
		pObject->Draw(pd3dCommandList, m_pLightCamera);
	}*/

	pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dShadowMap,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

	/*if (m_CurrentCamera)
	{
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER__CAMERA);
	}*/
	
}

void CSceneYJ::Communicate(SOCKET& sock)
{
	
}

void CSceneYJ::ProcessInput()
{
	if (m_CurrentCamera == nullptr) return;

	float cameraSpeed = m_CurrentCamera->GetSpeed();
	XMFLOAT3 velocity = m_Player->GetVelocity(); 

	auto keyInput = GAME_INPUT;
	if (keyInput.KEY_W)
	{
		if (m_isPlayerSelected)
			m_Player->SetVelocity(OBJ_DIRECTION::Front);
		else
			m_CurrentCamera->Walk(cameraSpeed);
	}
	if (keyInput.KEY_A)
	{
		if (m_isPlayerSelected)
			m_Player->SetVelocity(OBJ_DIRECTION::Left);
		else
			m_CurrentCamera->Strafe(-cameraSpeed);
	}
	if (keyInput.KEY_S)
	{
		if (m_isPlayerSelected)
			m_Player->SetVelocity(OBJ_DIRECTION::Back);
		else
			m_CurrentCamera->Walk(-cameraSpeed);
	}
	if (keyInput.KEY_D)
	{
		if (m_isPlayerSelected)
			m_Player->SetVelocity(OBJ_DIRECTION::Right);
		else
			m_CurrentCamera->Strafe(cameraSpeed);
	}
	if (keyInput.KEY_B)
	{
		m_CurrentCamera->SetShake(true, 0.5f, 5.0f);
	}
	if (keyInput.KEY_1)
	{
		m_Player->SetWeapon(PlayerWeaponType::Sword);
		//m_CurrentCamera = m_Cameras[0];
	}
	if (keyInput.KEY_2)
	{
		m_Player->SetWeapon(PlayerWeaponType::Bow);
		//m_CurrentCamera = m_Cameras[1];
	}
	if (keyInput.KEY_3)
	{
		m_isPlayerSelected = true;
		m_CurrentCamera = m_Cameras[0];
	
	}
	if (keyInput.KEY_4)
	{
		m_isPlayerSelected = false;
		m_CurrentCamera = m_Cameras[2];
	}
	if (keyInput.KEY_5)
	{
		m_isPlayerSelected = false;
		//m_CurrentCamera = m_Cameras[3];
	}

	////////////////////////////////////////////////////////// 
	if (keyInput.KEY_ADD)
	{
		//m_CurrentCamera->SetSpeed(min(cameraSpeed + 1.0f, 15.0f));
		LightPos.y += 10.0f;
		cout << LightPos.y<<endl;
	}
	if (keyInput.KEY_SUBTRACT)
	{
		//m_CurrentCamera->SetSpeed(max(cameraSpeed - 1.0f, 1.0f));
		LightPos.y -= 10.0f;
		cout << LightPos.y<<endl;
	}


	if (keyInput.KEY_F1)
	{
		//m_Player->SetPosition({ 2500,  0, 2500 });
		LightPos.z += 10.0f;
		cout << LightPos.z << endl;
	}
	if (keyInput.KEY_F2)
	{
		//m_Player->SetPosition({ 2500,  -1000, 17500 });
		LightPos.z -= 10.0f;
		cout << LightPos.z << endl;
	}



	if (keyInput.KEY_F3)
	{
		//m_Player->SetPosition({ 10500,  -2000, 17500 });

		lensize += 10.0f;
		m_pLightCamera->SetLens(0.25f * PI, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.0f, lensize);
		cout << lensize << endl;
	}
	if (keyInput.KEY_F4)
	{
		//m_Player->SetPosition({ 12500,  -3000, 2500 });

		lensize -= 10.0f;
		m_pLightCamera->SetLens(0.25f * PI, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.0f, lensize);
		cout << lensize << endl;
	}
	if (keyInput.KEY_F5)
	{
		m_Player->SetPosition({ 17500,  -6000, 17500 });
	}

	if (keyInput.KEY_U)
	{
	}
	if (keyInput.KEY_I)
	{
	}
	if (keyInput.KEY_O)
	{
		gbBoundaryOn = true;
	}
	if (keyInput.KEY_P)
	{
		gbBoundaryOn = false;
	}
	if (keyInput.KEY_J)
	{
	}
	if (keyInput.KEY_K)
	{
		gbShadowOn = true;
	}
	if (keyInput.KEY_L)
	{
		gbShadowOn = false;
	}
	//DisplayVector3(m_CurrentCamera->GetPosition3f());
	
	m_CurrentCamera->UpdateViewMatrix();
}

void CSceneYJ::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(CFramework::GetInstance().GetHWND());
}

void CSceneYJ::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CSceneYJ::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

		if (m_isPlayerSelected)
		{
			//m_CurrentCamera->RotateAroundTarget(XMFLOAT3(1, 0, 0), dy * 30);
			m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 75);

			if (m_Player->IsMoving())
			{
				m_Player->Rotate(XMFLOAT3(0, 1, 0), dx * 150);
				m_MinimapArrow->Rotate(-dx * 150);
			}
		}
		else {
			m_CurrentCamera->Pitch(dy);
			m_CurrentCamera->RotateY(dx);
		}
	}

	if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = static_cast<float>(x - m_LastMousePos.x);
		float dy = static_cast<float>(y - m_LastMousePos.y);

		m_CurrentCamera->MoveOffset(XMFLOAT3(0, 0, dy));
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void CSceneYJ::ReleaseUploadBuffers()
{
	for (auto pObject : m_Objects)
	{
		pObject->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* CSceneYJ::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// Ground
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = m_Textures.size() + 3;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[7];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 18; // GameData
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //SceneFrameData
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; //Camera
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Light Camera
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Light
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 5; //Material
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region sampler
	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[3];

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

	pd3dSamplerDescs[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	pd3dSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].MipLODBias = 0;
	pd3dSamplerDescs[2].MaxAnisotropy = 16;
	pd3dSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	pd3dSamplerDescs[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	pd3dSamplerDescs[2].MinLOD = 0;
	pd3dSamplerDescs[2].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[2].ShaderRegister = 2;
	pd3dSamplerDescs[2].RegisterSpace = 0;
	pd3dSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags
		= d3dRootSignatureFlags;

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

void CSceneYJ::BuildBridges(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	CBridge* pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90);
	pBridge->SetPosition({ 8200,  -1301,  17800 });
	m_Objects.push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90);
	pBridge->SetPosition({ 10200,  -1301,  17800 });
	m_Objects.push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90);
	pBridge->SetPosition({ 9200,  -1301,  17800 });
	m_Objects.push_back(pBridge);
}

void CSceneYJ::BuildDoorWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	CDoorWall* pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 4000, 1000, 500, pShader);
	pDoorWall->SetPosition({ 0,0, 7500 });
	m_Objects.push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 3300, 1000, 500, pShader);
	pDoorWall->SetPosition({ 10300, -2000, 7500 });
	pDoorWall->SetTextureIndexes(0x02);
	m_Objects.push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 4000, 2500, 500, true, pShader);
	pDoorWall->SetTextureIndexes(0x04);
	//pDoorWall->RotateAll({ 0,1,0 }, 90);
	pDoorWall->SetPosition({ 13500, -3500, 00 });
	m_Objects.push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 5500, 2000, 500, pShader);
	pDoorWall->SetPosition({ 14000,-4500, 8000 });
	pDoorWall->SetTextureIndexes(0x08);
	m_Objects.push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 5700, 4500, 800, pShader);
	pDoorWall->SetPosition({ 14000, -7050, 13650 });
	pDoorWall->SetTextureIndexes(0x08);
	m_Objects.push_back(pDoorWall);
}

void CSceneYJ::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CShader* pShader = new CShader();
	UI* pUI = new UI(pd3dDevice, pd3dCommandList, 0.4f, 0.09f, 0.0f, true);
	pUI->SetPosition({ -0.50, 0.88,  0.92 });		// HP, SP
	pUI->SetTextureIndex(0x01);
	pUI->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
	m_UIs.push_back(pUI);

	pUI = new UI(pd3dDevice, pd3dCommandList, 0.3f, 0.09f, 0.0f, true);
	pUI->SetPosition({ -0.50, 0.79,  0.92 });		// HP, SP
	pUI->SetTextureIndex(0x02);
	pUI->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
	m_UIs.push_back(pUI);

	for (int i = 0; i < 20; ++i)
	{
		pUI = new HpSpPercentUI(pd3dDevice, pd3dCommandList, 0.015, 0.053f, 0.0f, true);
		pUI->SetPosition({ float(-0.615 + 0.015 * i), 0.885,  0.91 });
		pUI->SetTextureIndex(0x04);
		pUI->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
		m_HPGauges.push_back(pUI);
	}
	for (int i = 0; i < 20; ++i)
	{
		pUI = new HpSpPercentUI(pd3dDevice, pd3dCommandList, 0.011, 0.053f, 0.0f, false);
		pUI->SetPosition({ float(-0.575 + 0.011 * i), 0.795,  0.91 });
		pUI->SetTextureIndex(0x04);
		pUI->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
		m_SPGauges.push_back(pUI);
	}

	m_MinimapArrow = new MinimapArrow(pd3dDevice, pd3dCommandList, 0.05, 0.05, 0.0f);
	m_MinimapArrow->SetPosition({ -0.78f, 0.78f,  0.8 });	// MinimapArrow
	m_MinimapArrow->SetTextureIndex(0x04);
	m_MinimapArrow->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
	m_UIs.push_back(m_MinimapArrow);

	pUI = new UI(pd3dDevice, pd3dCommandList, 0.1f, 0.1f, 0.0f, true);
	pUI->SetPosition({ -0.53, 0.65,  0 });		// WeaponUI
	pUI->SetTextureIndex(0x10);
	pUI->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
	m_UIs.push_back(pUI);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
	m_HelpTextUI = new HelpTextUI(pd3dDevice, pd3dCommandList, 1.5, 0.25f, 0.8f, HELP_TEXT_INFO::QuestAccept);
	m_HelpTextUI->SetPosition({ 0.0f, -0.8,  0 });
	m_HelpTextUI->SetTextureIndex(0x01);
	m_HelpTextUI->SetShader(CShaderHandler::GetInstance().GetData("UiHelpText"));
	m_UIs.push_back(m_HelpTextUI);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	pUI = new Minimap(pd3dDevice, pd3dCommandList, 0.20f);
	pUI->SetPosition({ -0.78f, 0.78f, 0.9 });	// Minmap Background
	pUI->SetTextureIndex(0x01);
	pUI->SetShader(CShaderHandler::GetInstance().GetData("Minimap"));
	m_UIs.push_back(pUI);

	pUI = new Minimap(pd3dDevice, pd3dCommandList, 0.165f);
	pUI->SetPosition({ -0.78f, 0.78f,  0.8 });	// Minimap
	pUI->SetTextureIndex(0x02);
	pUI->SetShader(CShaderHandler::GetInstance().GetData("Minimap"));
	pUI->Rotate(180);
	m_UIs.push_back(pUI);
}

void CSceneYJ::BuildPuzzles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CPlate* pPuzzlePlate = new CPlate(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("Puzzle"));
	pPuzzlePlate->SetPosition({ 10600.0f,  0.0f - 2000.0f, 1500.0f + 8000.0f });
	//pPuzzlePlate->RotateAll ({ 0,1,0 }, 180);
	m_Objects.push_back(std::move(pPuzzlePlate));

	CGameObject* pObject = new CPuzzle(pd3dDevice, pd3dCommandList, PuzzleType::Holding, CShaderHandler::GetInstance().GetData("Puzzle"));
	pObject->SetPosition({ 10500.0f,  0.0f - 2000.0f, 1500.0f + 8000.0f });
	m_Objects.push_back(std::move(pObject));

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			pObject = new CBox(pd3dDevice, pd3dCommandList, 150, 100, 150);
			pObject->SetPosition({ 10900.0f + i * 1800.0f,  300 - 2000.0f, 1800.0f + j * 300.0f + 8000.0f });
			pObject->SetTextureIndex(0x80);
			pObject->SetShader(CShaderHandler::GetInstance().GetData("Object"));
			m_Objects.push_back(std::move(pObject));
		}
	}
}

void CSceneYJ::BuildSigns(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{ 
	// 첫번째 지형 표지판
	CSign* pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::Scroll,
		false, true, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 2700, 200,7000 });
	m_Objects.push_back(pSign);

	// 퍼즐 벽 표지판
	pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::NumPuzzle,
		false, false, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 11200.0f, -1800.0f, 8200.0f });
	m_Objects.push_back(pSign);

	// 메두사 벽 표지판
	pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::Medusa,
		true, true, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 13000.0f, -3250.0f, 1300.0f });
	pSign->RotateAll({ 0,1,0 }, 90.0f);
	m_Objects.push_back(pSign);
}

void CSceneYJ::BuildEnemys(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CMeshFbx* fbxMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/babymos.fbx", true);
	CGameObject* pObject = new CGameObject();
	pObject->SetMesh(fbxMesh);
	pObject->SetPosition({ 16800,  -6070, 16500 });
	pObject->SetTextureIndex(0x01);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	pObject->SetTextureIndex(0x80);
	pObject->Scale(35, 35, 35);
	m_Objects.push_back(std::move(pObject));
}

void CSceneYJ::BuildMirror(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Mirror = new CGameObject();

	CPlaneMeshTextured* pMirrorMesh = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList, 20000.0f, 20000.0f, 1.0f);

	m_MirrorCamera->SetPosition({ 17000, -3000, 210 });

	m_Mirror->SetMesh(pMirrorMesh);
	m_Mirror->SetShader(CShaderHandler::GetInstance().GetData("Mirror"));
	m_Mirror->SetPosition({ 17000, -2300, 200 });
	m_Mirror->SetTextureIndex(0x01);
}

void CSceneYJ::BuildMinimapResource(ID3D12Device* pd3dDevice)
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = FRAME_BUFFER_WIDTH;
	texDesc.Height = FRAME_BUFFER_HEIGHT;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	pd3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_pd3dMinimapTex));
}

void CSceneYJ::BuildMirrorResource(ID3D12Device* pd3dDevice)
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = FRAME_BUFFER_WIDTH;
	texDesc.Height = FRAME_BUFFER_HEIGHT;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	pd3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_pd3dMirrorTex));
}

void CSceneYJ::BuildShadowResource(ID3D12Device* pd3dDevice)
{
	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));

	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = FRAME_BUFFER_WIDTH;
	d3dResourceDesc.Height = FRAME_BUFFER_HEIGHT;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE d3dClear;
	d3dClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClear.DepthStencil.Depth = 1.0f;
	d3dClear.DepthStencil.Stencil = 0;

	pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, 
		D3D12_HEAP_FLAG_NONE, 
		&d3dResourceDesc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		&d3dClear, 
		__uuidof(ID3D12Resource), 
		(void**)&m_pd3dShadowMap
	);

	
}


void CSceneYJ::BuildMapSector1(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
#pragma region Create Tree
	// 지나가지 못하는 첫번째 지형쪽의 나무 빌보드
	CBillboardMesh* pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 160.0f, 160.0f);

	CGameObject* pBillboardObject = new CGameObject();

	float x_Tree, z_Tree;
	 
	for (int i = 0; i < 2; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 3000.0f + 500.0f * i;
		z_Tree = 4500.0f;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 400.0f , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}

	for (int i = 0; i < 5; i++)
	{
		pBillboardObject = new CGameObject();

		pBillboardObject->SetMesh(pBillboardMesh);

		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 4000 + 100.0f * i;
		z_Tree = 4500 + 700.0f * i;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f, z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}

	for (int i = 0; i < 2; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 700.0f + (600 * i);
		z_Tree = 4000.0f + (600 * i);
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 400.0f , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}

	for (int i = 0; i < 2; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 4000.0f + (600 * i);
		z_Tree = 3200.0f;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 400.0f , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}

	for (int i = 0; i < 3; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 5500;
		z_Tree = 4200.0f + (1000 * i);
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}

	for (int i = 0; i < 4; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 4000.0f;
		z_Tree = 10500.0f + (500 * i);
		if (i == 2)
		{
			x_Tree = 5500.0f;
			z_Tree = 10500.0f;
		}
		if (i == 3)
		{
			x_Tree = 5500.0f;
			z_Tree = 11000.0f;
		}
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}

	for (int i = 0; i < 4; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 4000;
		z_Tree = 12600.0f + 2100.0f * i;

		if (i == 2)
		{
			x_Tree = 5500.0f;
			z_Tree = 12600.0f;
		}
		if (i == 3)
		{
			x_Tree = 5500.0f;
			z_Tree = 14700.0f;
		}

		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}

	for (int i = 0; i < 7; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 8200;
		z_Tree = 300.0f + 1650.0f * i;

		if (i == 5)
		{
			x_Tree = 6500.0f;
			z_Tree = 8500.0f;
		}
		if (i == 6)
		{
			x_Tree = 6500.0f;
			z_Tree = 9500.0f;
		}

		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	} 
#pragma endregion 

	CMeshFbx* fbx_bush_Mesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/bush-01.fbx", true);

	CGameObject* pObject;
	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->Rotate({ 1,0,0 }, 90);
		pObject->SetMesh(fbx_bush_Mesh);
		pObject->SetPosition({ 1000.0f + i * 1000.0f, 100, 850 });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		m_Objects.push_back(std::move(pObject));
	}

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->Rotate({ 1,0,0 }, 90);
		pObject->SetMesh(fbx_bush_Mesh);
		pObject->SetPosition({ 1200.0f + i * 1000.0f, 0, 3150 });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		m_Objects.push_back(std::move(pObject));
	}

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(fbx_bush_Mesh);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 500.0f + i * 2700.0f, 0, 2150 });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);

		pObject->Scale(50, 50, 50);
		m_Objects.push_back(std::move(pObject));
	}

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(fbx_bush_Mesh);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 2600.0f, 0, 4650 + 500.0f * i });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);

		pObject->Scale(50, 50, 50);
		m_Objects.push_back(std::move(pObject));
	}

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(fbx_bush_Mesh);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 2600.0f, -200.0f, 4650 + 500.0f * i });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);

		pObject->Scale(50, 50, 50);
		m_Objects.push_back(std::move(pObject));
	}

	for (int i = 0; i < 9; i++)
	{
		float x_bush = 1000.0f;
		float z_bush = 8050.0f;

		if (i == 0) {
			x_bush += 2000.0f;
		}

		if (i == 2) {
			x_bush -= 1000.0f;
		}

		if (i == 4) {
			x_bush -= 1500.0f;
		}

		if (i == 6) {
			x_bush -= 2000.0f;
		}

		if (i == 8) {
			x_bush -= 2000.0f;
		}
		pObject = new CGameObject();
		pObject->SetMesh(fbx_bush_Mesh);
		pObject->Rotate({ 0,0,1 }, 90);

		pObject->SetPosition({ x_bush + 300 * i, 0, z_bush + 700.0f * i });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);

		pObject->Scale(50, 50, 50);
		m_Objects.push_back(std::move(pObject));
	}

	pObject = new CGameObject();
	pObject->SetMesh(fbx_bush_Mesh);
	pObject->SetPosition({ 2500.0f, 100.0f, 250.0f });
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->SetTextureIndex(0x01);
	pObject->Scale(50.0f, 50.0f, 50.0f);
	m_Objects.push_back(std::move(pObject));
}

void CSceneYJ::BuildMapSector2(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CMeshFbx* fbx_Rock_Mesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/rock.fbx", true);
	CGameObject* pObject;

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(fbx_Rock_Mesh);
		pObject->SetPosition({ 500.0f + i * 1000.0f, -1000, 19500 - 500.0f * i });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->SetTextureIndex(0x02);
		pObject->Scale(50.0f, 50.0f, 50.0f);
		m_Objects.push_back(std::move(pObject));
	}

	CMeshFbx* fbx_Dry_Mesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/Dry_Tree.fbx", true);
	CMeshFbx* fbx_Stump_Mesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/Stump_01.fbx", true);
	CMeshFbx* fbx_Dead_Tree_Mesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/Dead_Tree.fbx", true);

	float x_Tree, z_Tree;

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(fbx_Dry_Mesh);

		x_Tree = 900+6200*i;
		z_Tree = 18800;
		pObject->Scale(0.5f + 0.5f * i, 0.5f, 0.5f + 0.5f * i);
		pObject->Rotate({ 0,1,0 }, 60.0f + 30.0f * i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) , z_Tree });

		pObject->SetTextureIndex(0x04);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
		m_Objects.push_back(std::move(pObject));
	}


	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(fbx_Dry_Mesh);

		x_Tree = 200 + 3000 * i;
		z_Tree = 17000;
		pObject->Scale(0.5f+0.5*i, 0.5f, 0.5f + 0.5 * i);
		pObject->Rotate({ 0,1,0 }, 0 + 15* i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree)   , z_Tree });
		pObject->SetTextureIndex(0x04);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
		m_Objects.push_back(std::move(pObject));
	}

	pObject = new CGameObject();
	pObject->SetMesh(fbx_Stump_Mesh);

	x_Tree = 200;
	z_Tree = 18000;

	pObject->Scale(20.0f, 20.0f, 20.0f);
	pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree), z_Tree });
	pObject->SetTextureIndex(0x08);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
	m_BillboardObjects.push_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetMesh(fbx_Dead_Tree_Mesh);

	x_Tree = 3700;
	z_Tree = 19000;
	pObject->Scale(150.0f, 150.0f, 150.0f);

	pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 1000.0f,z_Tree });
	pObject->SetTextureIndex(0x10);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
	m_BillboardObjects.push_back(std::move(pObject));

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(fbx_Dead_Tree_Mesh);

		x_Tree = 1500 + 5000 * i;
		z_Tree = 17500;
		pObject->Scale(150.0f+50*i, 150.0f+50*i, 150.0f + 50 * i);
		pObject->Rotate({ 0,1,0 }, 30+30*i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 1500.0f, z_Tree });

		pObject->SetTextureIndex(0x10);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		m_BillboardObjects.push_back(std::move(pObject));
	}
}

void CSceneYJ::BuildMapSector3(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CMeshFbx* fbx_Desert_Rock_Mesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/Desert_Rock.fbx", true);
	CGameObject* pObject;

	float x_Tree, z_Tree;

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(fbx_Desert_Rock_Mesh);

		x_Tree = 11000.0f+300.0f*i;
		z_Tree = 18500.0f;

		if (i == 0)
		{
			z_Tree = 19500;
			pObject->Scale(4.0f, 4.0f, 4.0f);
		}
		else if (i == 1)
		{
			z_Tree = 18700;
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(2.0f, 2.0f, 2.0f);
		}
		else if (i == 4)
		{
			pObject->Scale(1.0f, 1.0f, 1.0f);
			x_Tree = 10700.0f;
			z_Tree = 20000.0f - 300.0f * i;
		}
		else
		{
			x_Tree = 10700.0f-100.0f*i;
			z_Tree = 20000.0f-300.0f*i;
		}

		pObject->Scale(0.5f, 0.5f, 0.5f);
		
		pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree), z_Tree });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));

		m_Objects.push_back(std::move(pObject));
	}

	for (int i = 0; i < 6; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(fbx_Desert_Rock_Mesh);

		x_Tree = 11000.0f;
		z_Tree = 19000 - 2000.0f * i;
		if (i == 0)
		{
			pObject->Scale(3.0f, 3.0f, 3.0f);
		}

		else if (i == 3)
		{
			z_Tree += 500.0f;
			pObject->Rotate({ 0,1,0 }, 270);
			pObject->Scale(3.0f, 3.0f, 3.0f);
			x_Tree += 700.0f;
		}
		else if (i == 1)
		{
			x_Tree = 11000 + 500 * i;
			z_Tree = 13900;
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 5)
		{
			x_Tree = 12300;
			z_Tree = 13300;
			pObject->Rotate({ 0,1,0 }, 135);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 4)
		{
			x_Tree = 13000;
			z_Tree = 15300;
		}
		else
		{
			x_Tree = 11000 + 500 * i;
			z_Tree = 13900;
		}


		pObject->Scale(0.5f, 0.5f, 0.5f);
		pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) , z_Tree });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));

		m_Objects.push_back(std::move(pObject));
	}

	for (int i = 0; i < 4; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(fbx_Desert_Rock_Mesh);
		
		x_Tree = 11000.0f+100*i;
		z_Tree = 19000 - 2000.0f * i;

		if (i % 2 == 0)
		{
			x_Tree += 2000;
		}
		
		pObject->Scale(0.5f, 0.5f, 0.5f);
		pObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) , z_Tree });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		
		m_Objects.push_back(std::move(pObject));
	}
}

void CSceneYJ::BuildMapSector4(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CSceneYJ::BuildMapSector5(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
