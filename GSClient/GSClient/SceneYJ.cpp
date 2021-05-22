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
#include "Terrain.h"
#include "Sign.h"
#include "Puzzle.h" 
#include "Particle.h"
#include "Arrow.h"
#include "Enemy.h"

#include "FbxObject.h"
#include "FbxLoader.h"
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
float lensize = 60000.0f;

CSceneYJ::CSceneYJ()
{
	cout << "Enter CSceneYJ \n";
	m_pd3dGraphicsRootSignature = NULL;
	m_isPlayerSelected = false;

	for (int i = 0; i < (int)FBX_MESH_TYPE::COUNT; ++i) {
		m_LoadedFbxMesh[i] = nullptr;
	}
	if (nullptr == m_LoadedFbxMesh[0]) {
		int stop = 3;
	}
	else {
		int stop = 4;
	}
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

	m_CreatedTime = chrono::high_resolution_clock::now();
}

void CSceneYJ::BuildCamera(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	int width, int height)
{
	int nCameras = 5;
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
	m_Cameras[0]->SetPosition({ 500,  250 + 150, 1200 });
	m_Cameras[0]->Pitch(XMConvertToRadians(15));
	m_Cameras[0]->SetOffset(XMFLOAT3(0.0f, 450.0f, -500.0f));

	m_Cameras[1]->SetPosition({ 500,  1500, 1500 });
	m_Cameras[1]->Pitch(XMConvertToRadians(90));

	m_Cameras[2]->SetPosition({ 500,  500, 500 });
	m_Cameras[3]->SetPosition({ 2000, 1000, 8000 });
	//m_Cameras[3]->Pitch(XMConvertToRadians(90));
	m_Cameras[4]->SetPosition({ 0,0,0 });

	if (CFramework::GetInstance().IsOnConntected())
	{
		m_CurrentCamera = m_Cameras[0];
		m_isPlayerSelected = true;
	}
	else {
		m_CurrentCamera = m_Cameras[2];
	}
	m_MirrorCamera = m_Cameras[3];
	m_MinimapCamera = m_Cameras[1];
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
	auto start_t = chrono::high_resolution_clock::now();
	m_pfbxManager = FbxManager::Create();
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	m_pfbxManager->SetIOSettings(m_pfbxIOs);

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		m_ObjectLayers[i].reserve(50);
	}

	m_Skybox = new CSkyBox(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("SkyBox"));
	m_Terrain = new CTerrain(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("Terrain"));

	CTerrainWater* pTerrainWater = new CTerrainWater(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, 257 * 35, 257 * 32);
	pTerrainWater->SetPosition(XMFLOAT3(5450.0f, -1300.0f, 16500.0f));
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainWater].push_back(pTerrainWater);

	LoadFbxMeshes(pd3dDevice, pd3dCommandList);

	BuildMapSector1(pd3dDevice, pd3dCommandList);
	BuildMapSector2(pd3dDevice, pd3dCommandList);
	BuildMapSector3(pd3dDevice, pd3dCommandList);
	BuildMapSector4(pd3dDevice, pd3dCommandList);
	BuildMapSector5(pd3dDevice, pd3dCommandList);

	BuildBridges(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("Bridge"));

	BuildDoorWall(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("DoorWall"));
	BuildPuzzles(pd3dDevice, pd3dCommandList);

	BuildSigns(pd3dDevice, pd3dCommandList);
	BuildMirror(pd3dDevice, pd3dCommandList);
	BuildPlayers(pd3dDevice, pd3dCommandList);

	BuildParticles(pd3dDevice, pd3dCommandList);
	BuildArrows(pd3dDevice, pd3dCommandList);

	BuildEnemys(pd3dDevice, pd3dCommandList);
	BuildBoundingRegions(pd3dDevice, pd3dCommandList);

	//CMeshFbx* fbxMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/babymos.fbx", true);
	//CGameObject* pObject = new CGameObject();
	//CMeshFbx* fbxMesh;
	//fbxMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/Arrow.fbx");
	//CArrow* pObject = new CArrow(); 
	//pObject->SetMesh(fbxMesh);
	//pObject->SetPosition({ 500.0f,  100.0f, 1500.0f });
	//pObject->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
	//pObject->SetTextureIndex(0x20);
	//pObject->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	//pObject->Scale(15.0f, 15.0f, 15.0f);
	//pObject->SetUseable(false);
	////pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 30, 10, 30, XMFLOAT3{ 0,0,0 });
	////pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(30, 10, 30)));
	//m_Objects.push_back(reinterpret_cast<CGameObject*>(std::move(pObject)));

	//int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::ArrowParticle);
	//if (-1 != idx) {
	//	m_Particles->UseParticle(idx, pObject->GetPosition(), XMFLOAT3(0.0f, 0.0f, -1.0f));
	//	pObject->ConnectParticle(m_Particles->GetParticleObj(idx));
	//}

	auto end_t = chrono::high_resolution_clock::now();

	auto elasepsed_t = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_t);
	cout << "InitElapsed Time : " << elasepsed_t.count() * 0.000001f << "\n";
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
		"HP_SP", "Minimap", "WeaponUI",
		"HP_SP_PER",
		"Flower_Red", "Flower_White", "Grass_1", "Grass_2", "Tree", "NoLeafTree", "Leaves", "Moss_Rock",
		"PuzzleBoard",
		"HelpText",
		"Dry_Tree", "Stump", "Dead_Tree",
		"Desert_Rock",
		"TerrainWater",
		"Rain",
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
		L"resources/OBJ/Desert_Rock.dds",
		L"resources/OBJ/Water.dds",
		L"resources/OBJ/Rain.dds"
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
		"Desert_Rock",
		"TerrainWater",
		"Rain"
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
	//m_Objects.clear();
}

void CSceneYJ::Update(float elapsedTime)
{
	ProcessInput();

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->Update(elapsedTime);
			pObject->UpdateColliders();
		}
	}
	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy]) {
		pEnemy->FixPositionByTerrain(m_Terrain);
	}
	m_Particles->Update(elapsedTime);

	m_HelpTextUI->Update(elapsedTime);

	for (auto player : m_Players) {
		if (!player->IsDrawable()) continue;
		player->Update(elapsedTime);
		player->UpdateColliders();
		player->FixPositionByTerrain(m_Terrain);
		player->FixCameraByTerrain(m_Terrain);
	}
	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Obstacle]) {
		if (pObstacle->CollisionCheck(m_Player)) {
			m_Player->FixCollision();
			cout << "충돌 : 플레이어 - 장애물";
		}
	}

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy]) {
		if (pEnemy->CollisionCheck(m_Player)) {
			m_Player->FixCollision();
			cout << "충돌 : 플레이어 - 적";
		}
	}

	for (auto pPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::Puzzle]) {
		if (pPuzzle->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pPuzzle);
			//cout << "충돌 : 플레이어 - 퍼즐";
		}
	}

	for (auto pPuzzleBox : m_ObjectLayers[(int)OBJECT_LAYER::PuzzleBox]) {
		if (pPuzzleBox->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pPuzzleBox);
			m_isBoxSelect = TRUE;
			m_Player->IsCanPickPuzzle(m_isBoxSelect);
			
			cout << "충돌 : 플레이어 - 퍼즐박스";
		}
	}

	if (m_CurrentCamera) m_CurrentCamera->Update(elapsedTime);

	if (m_MirrorCamera)
	{
		m_MirrorCamera->UpdateViewMatrix();
	}

	if (m_pLightCamera)
	{
		LightPos = m_Player->GetPosition();
		LightPos.y = 3000.0f;
		LightPos.z += 10000.0f;

		m_pLightCamera->LookAt({ LightPos },
			{ m_Player->GetPosition().x,m_Player->GetPosition().y,m_Player->GetPosition().z },
			m_Player->GetUp());

		/*m_pLightCamera->LookAt(LightPos,
			{ 20000.0f,0.0f,20000.0f },
			m_Player->GetUp());*/

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

	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dBasicSrvDescriptorHeap };
	//pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_pcbMappedSceneFrameData->m_PlayerHP = m_Player->GetHP();
	m_pcbMappedSceneFrameData->m_PlayerSP = m_Player->GetSP();
	m_pcbMappedSceneFrameData->m_PlayerWeapon = m_Player->GetSelectedWeapon();
	auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		chrono::high_resolution_clock::now() - m_CreatedTime);
	m_pcbMappedSceneFrameData->m_Time = timeElapsed.count() * 0.001f;
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData

	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials
	//
	//::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights
	//
	//D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dBasicSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE, tex);

	m_Skybox->Draw(pd3dCommandList, m_CurrentCamera);
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);
	m_Mirror->Draw(pd3dCommandList, m_CurrentCamera);

	m_Particles->Draw(pd3dCommandList, m_CurrentCamera);

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		//if (i == (int)OBJECT_LAYER::Enemy) {
		//	continue;
		//}
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->Draw(pd3dCommandList, m_CurrentCamera);
		}
	}
	for (auto player : m_Players) {
		if (player->IsDrawable()) {
			player->Draw(pd3dCommandList, m_CurrentCamera);
		}
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
	if (m_Player) m_Player->Draw(pd3dCommandList, m_CurrentCamera);
}

void CSceneYJ::FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CSceneYJ::DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV)
{
	//return;
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
	m_pcbMappedSceneFrameData->m_PlayerWeapon = m_Player->GetSelectedWeapon();
	auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		chrono::high_resolution_clock::now() - m_CreatedTime);
	m_pcbMappedSceneFrameData->m_Time = timeElapsed.count() * 0.001f;
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights

	m_Skybox->Draw(pd3dCommandList, m_MinimapCamera);
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);
	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->Draw(pd3dCommandList, m_CurrentCamera);
		}
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
	//return;
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
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);
	m_Player->Draw(pd3dCommandList, m_CurrentCamera);
	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->Draw(pd3dCommandList, m_CurrentCamera);
		}
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

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}
}

void CSceneYJ::DrawShadow(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_isPlayerSelected == true)
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

		for (int i = 0; i < m_ObjectLayers.size(); ++i) {
			for (auto pObject : m_ObjectLayers[i]) {
				pObject->Draw_Shadow(pd3dCommandList, m_CurrentCamera);
			}
		}
		m_Player->Draw_Shadow(pd3dCommandList, m_pLightCamera);


		pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dShadowMap,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

		/*if (m_CurrentCamera)
		{
			m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
			m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER__CAMERA);
		}*/
	}
}

void CSceneYJ::Communicate(SOCKET& sock)
{
	P_C2S_UPDATE_SYNC_REQUEST p_syncUpdateRequest;
	p_syncUpdateRequest.size = sizeof(P_C2S_UPDATE_SYNC_REQUEST);
	p_syncUpdateRequest.type = PACKET_PROTOCOL::C2S_INGAME_UPDATE_SYNC;
	p_syncUpdateRequest.playerNum = m_CurrentPlayerNum;

	int retVal;
	bool haveToRecv = false;
	SendPacket(CFramework::GetInstance().GetSocket(), reinterpret_cast<char*>(&p_syncUpdateRequest), p_syncUpdateRequest.size, retVal);

	char buffer[BUFSIZE + 1] = {};
	RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);
	PACKET_PROTOCOL type = (PACKET_PROTOCOL)buffer[1];

	// 플레이어 추가 혹은 삭제 패킷 수신
	if (type == PACKET_PROTOCOL::S2C_NEW_PLAYER) {
		cout << "Packet::NewPlayer[ServerToClient]\n";
		P_S2C_ADD_PLAYER p_addPlayer = *reinterpret_cast<P_S2C_ADD_PLAYER*>(&buffer);
		XMFLOAT3 pos = { IntToFloat(p_addPlayer.x), IntToFloat(p_addPlayer.y), IntToFloat(p_addPlayer.z) };

		m_Players[p_addPlayer.id]->SetPosition(pos);
		m_Players[p_addPlayer.id]->SetDrawable(true);
		++m_CurrentPlayerNum;
		haveToRecv = true;
	}
	else if (type == PACKET_PROTOCOL::S2C_DELETE_PLAYER) {
		cout << "Packet::DeletePlayer[ServerToClient]\n";
		P_S2C_DELETE_PLAYER p_addPlayer = *reinterpret_cast<P_S2C_DELETE_PLAYER*>(&buffer);
		m_Players[p_addPlayer.id]->SetDrawable(false);
		haveToRecv = true;
	}
	// 갱신 정보를 다시 받아와야 하는 경우.
	if (haveToRecv) {
		ZeroMemory(buffer, sizeof(buffer));
		RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);
		haveToRecv = false;
	}

	// 새롭게 받아온 정보가 갱신 정보가 아닌 기타 정보인 경우.
	if (type == PACKET_PROTOCOL::S2C_INGAME_DOOR_EVENT) {
		cout << "Packet::DoorEvent[ServerToClient]\n";
		haveToRecv = true;
	}
	if (type == PACKET_PROTOCOL::S2C_INGAME_END) {
		cout << "Packet::GameEnd[ServerToClient]\n";
		return;
	}

	// 모든 부가 정보들 갱신을 마치고 플레이어들 정보를 다시 받아와야 하는 경우.
	if (haveToRecv) {
		ZeroMemory(buffer, sizeof(buffer));
		RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);
	}
	P_S2C_UPDATE_SYNC p_syncUpdate = *reinterpret_cast<P_S2C_UPDATE_SYNC*>(&buffer);

	for (int i = 0; i < p_syncUpdate.playerNum; ++i) {
		if (m_Players[p_syncUpdate.id[i]]->IsDrawable() == false) continue;

		XMFLOAT3 pos = { IntToFloat(p_syncUpdate.posX[i]), IntToFloat(p_syncUpdate.posY[i]), IntToFloat(p_syncUpdate.posZ[i]) };

		m_Players[p_syncUpdate.id[i]]->SetPosition(pos);
		m_Players[p_syncUpdate.id[i]]->UpdateCamera();
	}

	if (m_MousePositions.size() > 0) {
		SendMouseInputPacket();
		RecvMouseProcessPacket();
	}
}

void CSceneYJ::LoginToServer()
{
	P_C2S_LOGIN p_login;
	p_login.size = sizeof(p_login);
	p_login.type = PACKET_PROTOCOL::C2S_LOGIN;
	strcpy_s(p_login.name, CFramework::GetInstance().GetPlayerName().c_str());

	int retVal;
	SendPacket(CFramework::GetInstance().GetSocket(), reinterpret_cast<char*>(&p_login), p_login.size, retVal);

	char buffer[BUFSIZE + 1] = {};
	RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);

	P_S2C_PROCESS_LOGIN p_processLogin = *reinterpret_cast<P_S2C_PROCESS_LOGIN*>(&buffer);
	if (p_processLogin.isSuccess)
	{
		XMFLOAT3 pos = XMFLOAT3{ IntToFloat(p_processLogin.x),
			IntToFloat(p_processLogin.y), IntToFloat(p_processLogin.z) };

		CFramework::GetInstance().SetPlayerId(p_processLogin.id);

		cout << "Login id = " << p_processLogin.id << "\n";

		//m_Player->SetPosition(pos); 

		m_Players[p_processLogin.id]->SetPosition(pos);
		m_Player = m_Players[p_processLogin.id];
		m_Player->SetDrawable(true);

		m_MinimapCamera->SetTarget(m_Player);

		for (int i = 0; i < MAX_PLAYER; ++i) {
			m_Players[i]->SetDrawable(p_processLogin.existPlayer[i]);
		}

		// Sync Data
		ZeroMemory(buffer, sizeof(buffer));
		RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);

		P_S2C_UPDATE_SYNC p_syncUpdate = *reinterpret_cast<P_S2C_UPDATE_SYNC*>(&buffer);

		m_CurrentPlayerNum = p_syncUpdate.playerNum;

		for (int i = 0; i < p_syncUpdate.playerNum; ++i) {
			if (m_Players[p_syncUpdate.id[i]]->IsDrawable() == false) continue;

			XMFLOAT3 pos = { IntToFloat(p_syncUpdate.posX[i]), IntToFloat(p_syncUpdate.posY[i]), IntToFloat(p_syncUpdate.posZ[i]) };

			m_Players[p_syncUpdate.id[i]]->SetPosition(pos);

			m_Players[p_syncUpdate.id[i]]->UpdateCamera();
		}
	}
}

void CSceneYJ::LogoutToServer()
{
}

void CSceneYJ::ProcessInput()
{
	if (CFramework::GetInstance().IsOnConntected())
	{
		auto keyInput = GAME_INPUT;
		bool processKey = false;
		P_C2S_KEYBOARD_INPUT p_keyboard;
		p_keyboard.size = sizeof(P_C2S_KEYBOARD_INPUT);
		p_keyboard.type = PACKET_PROTOCOL::C2S_INGAME_KEYBOARD_INPUT;
		p_keyboard.id = CFramework::GetInstance().GetPlayerId();

		if (keyInput.KEY_W) {
			p_keyboard.keyInput = VK_W;
			processKey = true;
		}
		if (keyInput.KEY_A) {
			p_keyboard.keyInput = VK_A;
			processKey = true;
		}
		if (keyInput.KEY_S) {
			p_keyboard.keyInput = VK_S;
			processKey = true;
		}
		if (keyInput.KEY_D) {
			p_keyboard.keyInput = VK_D;
			processKey = true;
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
		if (processKey == false) return;
		int retVal = 0;
		SendPacket(CFramework::GetInstance().GetSocket(),
			reinterpret_cast<char*>(&p_keyboard), p_keyboard.size, retVal);

		char buffer[BUFSIZE + 1] = {};
		RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);

		P_S2C_PROCESS_KEYBOARD p_keyboardProcess = *reinterpret_cast<P_S2C_PROCESS_KEYBOARD*>(&buffer);

		XMFLOAT3 pos = XMFLOAT3{ IntToFloat(p_keyboardProcess.posX),
			IntToFloat(p_keyboardProcess.posY),
			IntToFloat(p_keyboardProcess.posZ) };

		m_Players[p_keyboard.id]->SetPosition(pos);
		m_Players[p_keyboard.id]->FixPositionByTerrain(m_Terrain);

		DisplayVector3(pos, true);
		return;
	}
	if (m_CurrentCamera == nullptr) return;

	float cameraSpeed = m_CurrentCamera->GetSpeed();
	XMFLOAT3 velocity = m_Player->GetVelocity();

	XMFLOAT3 shift = XMFLOAT3(0, 0, 0);
	float distance = PLAYER_RUN_VELOCITY;

	auto keyInput = GAME_INPUT;
	if (keyInput.KEY_W)
	{
		if (m_isPlayerSelected) {
			m_Player->SetVelocity(Vector3::Add(shift, m_CurrentCamera->GetLook3f(), distance));
		}
		else
			m_CurrentCamera->Walk(cameraSpeed);
	}
	if (keyInput.KEY_A)
	{
		if (m_isPlayerSelected) {
			m_Player->SetVelocity(Vector3::Add(shift, m_CurrentCamera->GetRight3f(), -distance));

			//m_Player->SetVelocity(Vector3::Add(shift, m_CurrentCamera->GetRight3f(), -distance));
		}
		else
			m_CurrentCamera->Strafe(-cameraSpeed);
	}
	if (keyInput.KEY_S)
	{
		if (m_isPlayerSelected) {
			m_Player->SetVelocity(Vector3::Add(shift, m_CurrentCamera->GetLook3f(), -distance));
		}
		else
			m_CurrentCamera->Walk(-cameraSpeed);
	}
	if (keyInput.KEY_D)
	{
		if (m_isPlayerSelected) {
			m_Player->SetVelocity(Vector3::Add(shift, m_CurrentCamera->GetRight3f(), distance));
		}
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
	if (keyInput.KEY_SPACE)
	{
		m_Player->Jump();
	}

	////////////////////////////////////////////////////////// 
	if (keyInput.KEY_ADD)
	{
		m_CurrentCamera->SetSpeed(min(cameraSpeed + 1.0f, 15.0f));
	}
	if (keyInput.KEY_SUBTRACT)
	{
		m_CurrentCamera->SetSpeed(max(cameraSpeed - 1.0f, 1.0f));
	}
	if (keyInput.KEY_F1)
	{
		//m_Player->SetPosition({ 2500,  0, 2500 });
	}
	if (keyInput.KEY_F2)
	{
		m_Player->SetPosition({ 2500,  -1000, 17500 });
	}
	if (keyInput.KEY_F3)
	{
		m_Player->SetPosition({ 10500,  -2000, 17500 });
	}
	if (keyInput.KEY_F4)
	{
		m_Player->SetPosition({ 14500,  -3000, 2500 });
	}
	if (keyInput.KEY_F5)
	{
		m_Player->SetPosition({ 17500,  -6000, 17500 });
	}
	if (keyInput.KEY_U)
	{
		for (int i = 0; i < 5; ++i) {
			CDoorWall* p = reinterpret_cast<CDoorWall*>(m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][m_DoorIdx + i]);
			p->OpenDoor();
		}
	}
	if (keyInput.KEY_I)
	{
		for (int i = 0; i < 5; ++i) {
			CDoorWall* p = reinterpret_cast<CDoorWall*>(m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][m_DoorIdx + i]);
			p->CloserDoor();
		}
	}
	if (keyInput.KEY_R && (m_isBoxSelect==TRUE))
	{
		
		cout << "현재 박스를 선택 가능합니다.";
		// 상호작용
		// 오류 뜸
		for (int j = 0; j < 2; ++j)
		{
			for (int i = 0; i < 5; ++i) {
				CBox* p = reinterpret_cast<CBox*>(m_ObjectLayers[(int)OBJECT_LAYER::PuzzleBox][m_PuzzleBoxIdx + i]);
				p->PlayerSelecting();
			}
		}
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
		if (m_Player->IsCanAttack()) {
			m_Player->SetCanAttack(false);
			m_Player->IncreaseAttackWaitingTime(1.5f);
			ShotArrow();
		}
	}
	if (keyInput.KEY_K)
	{
		gbWireframeOn = true;
	}
	if (keyInput.KEY_L)
	{
		gbWireframeOn = false;
	}
	if (keyInput.KEY_T)
	{
		
	}
	if (keyInput.KEY_Y)
	{
		gbShadowOn = false;
	}
	m_CurrentCamera->UpdateViewMatrix();
}

void CSceneYJ::OnMouseDown(WPARAM btnState, int x, int y)
{
	if (CFramework::GetInstance().IsOnConntected())
	{
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
	SetCapture(CFramework::GetInstance().GetHWND());
}

void CSceneYJ::OnMouseUp(WPARAM btnState, int x, int y)
{
	if (CFramework::GetInstance().IsOnConntected())
	{
		if (m_MousePositions.size() > 0) {
			SendMouseInputPacket();
			RecvMouseProcessPacket();
		}
	}
	ReleaseCapture();
}

void CSceneYJ::OnMouseMove(WPARAM btnState, int x, int y)
{
	//
	// 싱글플레이, 멀티 플레이 코드 분리 작업 진행 필요
	//
	if (CFramework::GetInstance().IsOnConntected())
	{
		if ((btnState & MK_LBUTTON) != 0)
		{
			float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
			float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

			m_prevMouseInputType = MOUSE_INPUT_TYPE::M_LMOVE;

			m_MousePositions.emplace_back(POINTF{ dx, dy });
		}
		else if ((btnState & MK_RBUTTON) != 0)
		{
			float dx = static_cast<float>(x - m_LastMousePos.x);
			float dy = static_cast<float>(y - m_LastMousePos.y);
			m_prevMouseInputType = MOUSE_INPUT_TYPE::M_RMOVE;

			m_MousePositions.emplace_back(POINTF{ dx, dy });
		}
		return;
	}
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

		///cout << "offset : " << dy << "\n";

		m_CurrentCamera->MoveOffset(XMFLOAT3(0, 0, dy));
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void CSceneYJ::ReleaseUploadBuffers()
{
	for (int i = 0; i < (int)OBJECT_LAYER::Count; ++i) {
		for (auto pObject : m_ObjectLayers[i])
		{
			pObject->ReleaseUploadBuffers();
		}
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

	D3D12_ROOT_PARAMETER pd3dRootParameters[9];
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

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[7].Descriptor.ShaderRegister = 6; //BoneOffsets
	pd3dRootParameters[7].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[8].Descriptor.ShaderRegister = 7; //BoneTransforms
	pd3dRootParameters[8].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPosition({ 8000.0f,  -1301.0f,  18100.0f });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPosition({ 10000.0f,  -1301.0f,  18100.0f });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPosition({ 9000.0f,  -1301.0f,  18100.0f });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pBridge);
}

void CSceneYJ::BuildDoorWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	CDoorWall* pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 4000, 1000, 500, pShader);
	pDoorWall->SetPosition({ 0,0, 7500 });
	m_DoorIdx = m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].size();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 3300, 1000, 500, pShader);
	pDoorWall->SetPosition({ 10300, -2000, 7500 });
	pDoorWall->SetTextureIndexes(0x02);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 4000, 2500, 500, true, pShader);
	pDoorWall->SetTextureIndexes(0x04);
	//pDoorWall->RotateAll({ 0,1,0 }, 90);
	pDoorWall->SetPosition({ 13500, -3500, 0 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 5500, 2000, 500, pShader);
	pDoorWall->SetPosition({ 14000,-4500, 8000 });
	pDoorWall->SetTextureIndexes(0x08);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 5800, 4500, 800, pShader);
	pDoorWall->SetPosition({ 14000, -7050, 13650 });
	pDoorWall->SetTextureIndexes(0x08);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	CWall* pWall = new CWall(pd3dDevice, pd3dCommandList, 1500, 2500, 500);
	pWall->Rotate({ 0,1,0 }, 90);
	pWall->SetPosition({ 13750, -3500 + 1250, 4750 });
	pWall->SetTextureIndex(0x04);
	pWall->SetShader(pShader);
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1500, 2500, 500, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pWall);

	pWall = new CWall(pd3dDevice, pd3dCommandList, 1500, 2500, 500);
	pWall->Rotate({ 0,1,0 }, 90);
	pWall->SetPosition({ 13750, -3500 + 1250, 6250 });
	pWall->SetTextureIndex(0x04);
	pWall->SetShader(pShader);
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1500, 2500, 500, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pWall);
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
	pPuzzlePlate->SetPosition({ 10600.0f, -2000.0f, 1500.0f + 8000.0f });
	//m_Objects.push_back(std::move(pPuzzlePlate));
	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pPuzzlePlate);

	CGameObject* pObject = new CPuzzle(pd3dDevice, pd3dCommandList, PuzzleType::Holding, CShaderHandler::GetInstance().GetData("Puzzle"));
	pObject->SetPosition({ 10500.0f, -2000.0f, 1500.0f + 8000.0f });
	//m_Objects.push_back(std::move(pObject));
	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pObject);
	
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			pObject = new CBox(pd3dDevice, pd3dCommandList, 150, 100, 150);
			
			pObject->SetPosition({ 10900.0f + i * 1800.0f,  300 - 2000.0f, 1800.0f + j * 300.0f + 8000.0f });
			pObject->SetTextureIndex(0x200);
			pObject->SetShader(CShaderHandler::GetInstance().GetData("Object"));
			pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 150.0f, 100.0f, 150.0f, XMFLOAT3{ 0,0,0 });
			pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 150 * 0.5f, 100*0.5f , 150 * 0.5f }));
			//m_Objects.push_back(std::move(pObject));
			m_PuzzleBoxIdx = m_ObjectLayers[(int)OBJECT_LAYER::PuzzleBox].size();
			m_ObjectLayers[(int)OBJECT_LAYER::PuzzleBox].push_back(pObject);
		}
	}
	
}

void CSceneYJ::BuildSigns(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 첫번째 지형 표지판
	CSign* pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::Scroll,
		false, true, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 2700, 200,7000 });
	//m_Objects.push_back(pSign); 
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pSign);

	// 퍼즐 벽 표지판
	pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::NumPuzzle,
		false, false, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 11200.0f, -1800.0f, 8200.0f });
	//m_Objects.push_back(pSign);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pSign);

	// 메두사 벽 표지판
	pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::Medusa,
		true, true, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 13000.0f, -3250.0f, 1300.0f });
	pSign->RotateAll({ 0,1,0 }, 90.0f);

	//m_Objects.push_back(pSign);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pSign);
}

void CSceneYJ::BuildEnemys(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject* pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Boss]);
	pObject->SetPosition({ 16800,  -5500, 16500 });
	pObject->SetTextureIndex(0x01);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	pObject->SetTextureIndex(0x80);
	pObject->Scale(35, 35, 35);
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 30, 10, 30, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(30 * 0.5f, 10 * 0.5f, 30 * 0.5f)));
	pObject->SetHegithFromTerrain(1000.0f);

	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 30, 10, 30, XMFLOAT3{ 0,-10,20 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, -10, 20), XMFLOAT3(30 * 0.5f, 10 * 0.5f, 30 * 0.5f)));

	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 30, 10, 30, XMFLOAT3{ 0,-10,-20 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, -10, -20), XMFLOAT3(30 * 0.5f, 10 * 0.5f, 30 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(pObject);

	CEnemy* pEnemy = new CEnemy();
	pEnemy->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
	pEnemy->Scale(125.0f, 125.0f, 125.0f);
	pEnemy->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_02]);
	pEnemy->SetTextureIndex(0x200);
	pEnemy->SetPosition({ 2005.0f, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f });
	pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f });
	pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
	pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 5.5f, 3.0f, 6.5f, XMFLOAT3{ 0,-1.5f,0 });
	pEnemy->AddColider(new ColliderBox(XMFLOAT3(0, 0, 5), XMFLOAT3(2.25f, 1.5f, 3.25f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

	pEnemy = new CEnemy();
	pEnemy->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
	pEnemy->Scale(125.0f, 125.0f, 125.0f);
	pEnemy->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_02]);
	pEnemy->SetTextureIndex(0x200);
	pEnemy->SetPosition({ 7800.0f, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f });
	pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f });
	pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
	pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 5.5f, 3.0f, 6.5f, XMFLOAT3{ 0,-1.5f,0 });
	pEnemy->AddColider(new ColliderBox(XMFLOAT3(0, 0, 5), XMFLOAT3(2.25f, 1.5f, 3.25f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

	pEnemy = new CEnemy();
	pEnemy->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
	pEnemy->Scale(125.0f, 125.0f, 125.0f);
	pEnemy->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_02]);
	pEnemy->SetTextureIndex(0x200);
	pEnemy->SetPosition({ 12100.0f, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f });
	pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f });
	pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
	pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 5.5f, 3.0f, 6.5f, XMFLOAT3{ 0,-1.5f,0 });
	pEnemy->AddColider(new ColliderBox(XMFLOAT3(0, 0, 5), XMFLOAT3(2.25f, 1.5f, 3.25f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

	pEnemy = new CEnemy();
	pEnemy->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
	pEnemy->Scale(125.0f, 125.0f, 125.0f);
	pEnemy->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_02]);
	pEnemy->SetTextureIndex(0x200);
	pEnemy->SetPosition({ 11900.0f, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f });
	pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f });
	pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
	pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 5.5f, 3.0f, 6.5f, XMFLOAT3{ 0,-1.5f,0 });
	pEnemy->AddColider(new ColliderBox(XMFLOAT3(0, 0, 5), XMFLOAT3(2.25f, 1.5f, 3.25f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

	pEnemy = new CEnemy();
	pEnemy->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
	pEnemy->Scale(125.0f, 125.0f, 125.0f);
	pEnemy->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_02]);
	pEnemy->SetTextureIndex(0x200);
	pEnemy->SetPosition({ 11900.0f, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f });
	pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f });
	pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
	pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 5.5f, 3.0f, 6.5f, XMFLOAT3{ 0,-1.5f,0 });
	pEnemy->AddColider(new ColliderBox(XMFLOAT3(0, 0, 5), XMFLOAT3(2.25f, 1.5f, 3.25f)));
	m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
}

void CSceneYJ::BuildMirror(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Mirror = new CGameObject();

	CPlaneMeshTextured* pMirrorMesh = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList, 6000.0f, 2600.0f, 1.0f);

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
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
	}

	for (int i = 0; i < 5; i++)
	{
		pBillboardObject = new CGameObject();

		pBillboardObject->SetMesh(pBillboardMesh);

		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 4000 + 100.0f * i;
		z_Tree = 4500 + 700.0f * i;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 200.0f, z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
	}

	for (int i = 0; i < 2; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 700.0f + (600 * i);
		z_Tree = 4000.0f + (600 * i);
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
	}

	for (int i = 0; i < 2; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 4000.0f + (600 * i);
		z_Tree = 3200.0f;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 300.0f , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
	}

	for (int i = 0; i < 3; i++)
	{
		pBillboardObject = new CGameObject();
		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 5.0f);

		x_Tree = 5500;
		z_Tree = 4200.0f + (1000 * i);
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 200.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
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
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 200.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
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

		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 200.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
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

		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 200.0f  , z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
	}
#pragma endregion 

	CGameObject* pObject;
	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->Rotate({ 1,0,0 }, 90);
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->SetPosition({ 1000.0f + i * 1000.0f, 100, 850 });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->Rotate({ 1,0,0 }, 90);
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->SetPosition({ 1200.0f + i * 1000.0f, 0, 3150 });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 500.0f + i * 2700.0f, 0, 2150 });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 2600.0f, 0, 4650 + 500.0f * i });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 2600.0f, -200.0f, 4650 + 500.0f * i });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
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
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->Rotate({ 0,0,1 }, 90);

		pObject->SetPosition({ x_bush + 300 * i, 0, z_bush + 700.0f * i });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);

		pObject->Scale(50, 50, 50);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
	pObject->SetPosition({ 2500.0f, 100, 250 });
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->SetTextureIndex(0x01);
	pObject->Scale(50, 50, 50);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
}

void CSceneYJ::BuildMapSector2(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject* pObject;

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryForestRock]);
		float x = 500.0f + i * 1000.0f;
		float z = 19500 - 500.0f * i;
		float y = m_Terrain->GetDetailHeight(x, z);
		pObject->SetPosition({ x, y, z });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->SetTextureIndex(0x02);
		pObject->Scale(50, 50, 50);
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5 * 0.5f, 7 * 0.5f, 3 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 5, 7, 3, { 0,0,0 });
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	float x_Tree, z_Tree;

	for (int i = 0; i < 1; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01]);

		x_Tree = 900 + 6200 * i;
		z_Tree = 18800;
		pObject->Scale(0.5f + 0.5 * i, 0.5f, 0.5f + 0.5 * i);
		pObject->Rotate({ 0,1,0 }, 60 + 30 * i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) - 30.0f , z_Tree });
		pObject->SetTextureIndex(0x04);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 200, 1500, 150, { 0, 0, 100 });
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01]);

	x_Tree = 900 + 6400;
	z_Tree = 19300;
	pObject->Scale(0.5f + 0.5, 0.5f, 0.5f + 0.5);
	pObject->Rotate({ 0,1,0 }, 60 + 30);
	pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) - 100.0f , z_Tree });
	pObject->SetTextureIndex(0x04);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 200, 1500, 150, { 0, 0, 100 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01]);

		x_Tree = 200 + 3000 * i;
		z_Tree = 17000;
		pObject->Scale(0.5f + 0.5 * i, 0.5f, 0.5f + 0.5 * i);
		pObject->Rotate({ 0,1,0 }, 0 + 15 * i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) - 100.0f, z_Tree });
		pObject->SetTextureIndex(0x04);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 200, 1500, 150, { 0, 0, 100 });
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Stump]);

	x_Tree = 200;
	z_Tree = 18000;

	pObject->Scale(20.0f, 20.0f, 20.0f);
	pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) - 150.0f, z_Tree });
	pObject->SetTextureIndex(0x08);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(15 * 0.5f, 10 * 0.5f, 15 * 0.5f)));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 15, 10, 15, { 0,0,0 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	///////////////////////////////////////////////////////////////////////////////////

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DeadTree_01]);

	x_Tree = 3700;
	z_Tree = 19000;
	pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 1000.0f,z_Tree });
	pObject->SetTextureIndex(0x10);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
	pObject->AddColider(new ColliderBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1, 5, 1, { 1, -5,-2.5 });
	pObject->Scale(150.0f, 150.0f, 150.0f);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DeadTree_01]);

		x_Tree = 1500 + 5000 * i;
		z_Tree = 17500;
		pObject->Scale(150.0f + 50 * i, 150.0f + 50 * i, 150.0f + 50 * i);
		//pObject->Rotate({ 0,1,0 }, 30 + 30 * i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 1000.0f + 400.0f * i, z_Tree });
		pObject->SetTextureIndex(0x10);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1, 5, 1, { 1, -5,-2.5 });
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}
}

void CSceneYJ::BuildMapSector3(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject* pObject;

	float x_Pos, z_Pos;

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DesertRock]);

		x_Pos = 11000.0f + 300.0f * i;
		z_Pos = 18500.0f;

		if (i == 0)
		{
			z_Pos = 19500;
			pObject->Scale(4.0f, 4.0f, 4.0f);
		}
		else if (i == 1)
		{
			z_Pos = 18700;
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(2.0f, 2.0f, 2.0f);
		}
		else if (i == 4)
		{
			pObject->Scale(1.0f, 1.0f, 1.0f);
			x_Pos = 10700.0f;
			z_Pos = 20000.0f - 300.0f * i;
		}
		else
		{
			x_Pos = 10700.0f - 100.0f * i;
			z_Pos = 20000.0f - 300.0f * i;
		}

		pObject->Scale(0.5f, 0.5f, 0.5f);

		pObject->SetPosition({ x_Pos , m_Terrain->GetDetailHeight(x_Pos,z_Pos), z_Pos });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 600, 250, 600, { 0, 220, 0 });
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 6; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DesertRock]);

		x_Pos = 11000.0f;
		z_Pos = 19000 - 2000.0f * i;
		if (i == 0)
		{
			pObject->Scale(3.0f, 3.0f, 3.0f);
		}

		else if (i == 3)
		{
			z_Pos += 500.0f;
			pObject->Rotate({ 0,1,0 }, 270);
			pObject->Scale(3.0f, 3.0f, 3.0f);
			x_Pos += 700.0f;
		}
		else if (i == 1)
		{
			x_Pos = 11000 + 500 * i;
			z_Pos = 13900;
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 5)
		{
			x_Pos = 12300;
			z_Pos = 13300;
			pObject->Rotate({ 0,1,0 }, 135);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 4)
		{
			x_Pos = 13000;
			z_Pos = 15300;
		}
		else
		{
			x_Pos = 11000 + 500 * i;
			z_Pos = 13900;
		}

		pObject->Scale(0.5f, 0.5f, 0.5f);
		pObject->SetPosition({ x_Pos , m_Terrain->GetDetailHeight(x_Pos,z_Pos) , z_Pos });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 600, 250, 600, { 0, 220, 0 });
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 4; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DesertRock]);

		x_Pos = 11000.0f + 100 * i;
		z_Pos = 19000 - 2000.0f * i;

		if (i % 2 == 0)
		{
			x_Pos += 2000;
		}

		pObject->Scale(0.5f, 0.5f, 0.5f);
		pObject->SetPosition({ x_Pos , m_Terrain->GetDetailHeight(x_Pos,z_Pos) - 100.0f, z_Pos });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 600, 250, 600, { 0, 220, 0 });
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}
}

void CSceneYJ::BuildMapSector4(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CSceneYJ::BuildMapSector5(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CSceneYJ::LoadFbxMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pfbxManager = FbxManager::Create();
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	m_pfbxManager->SetIOSettings(m_pfbxIOs);
	 

	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1] = new CFixedMesh(pd3dDevice, pd3dCommandList, "bush-01");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryForestRock] = new CFixedMesh(pd3dDevice, pd3dCommandList, "rock");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Player] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Golem");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Dry_Tree");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Stump] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Stump_01");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DeadTree_01] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Dead_Tree");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DesertRock] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Desert_Rock");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_02] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Enemy_t2");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Boss] = new CFixedMesh(pd3dDevice, pd3dCommandList, "babymos");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Arrow] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Arrow");
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
	m_pLightCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pLightCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pLightCamera->SetScissorRect(0, 0, nWidth, nHeight);

	m_pLightCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CSceneYJ::BuildParticles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Particles = new CParticle();
	for (int i = 0; i < 10; ++i) {
		m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::ArrowParticle);
		//m_Particles->UseParticle(i, XMFLOAT3(500.0f * i, -500.0f, 3000.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	}
	// 안개
	m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::RadialParitcle);

	// 비
	m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 100000, PARTICLE_TYPE::RainParticle);

	// 모래바람
	//m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 100000, PARTICLE_TYPE::SandWindParticle);

	//m_Particles->UseParticle(i, XMFLOAT3(500.0f * i, -500.0f, 3000.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	

	//m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::HitParticleTex);


	//int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::HitParticleTex);
	//if (-1 != idx) {
	//	m_Particles->UseParticle(idx, XMFLOAT3(0, 00.0f, 3000.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	//}
	//idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::ArrowParticle);
	//if (-1 != idx) {
	//	m_Particles->UseParticle(idx, XMFLOAT3(100, 00.0f, 3000.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	//}

	MakingFog();
	MakingRain();
	//MakingSandWind();

}
void CSceneYJ::BuildArrows(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Arrow]);
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->SetTextureIndex(0x20);
		pArrow->SetShader(CShaderHandler::GetInstance().GetData("Object"));
		pArrow->Scale(25.0f, 25.0f, 25.0f);
		pArrow->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 0.5f, 0.5f, 15, XMFLOAT3{ 0,0,5 });
		pArrow->AddColider(new ColliderBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Arrow].push_back(pArrow);
	}
}

void CSceneYJ::BuildPlayers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Players[0] = new CPlayer(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, m_pfbxManager, "resources/FbxExported/fbxsoldier.bin");
	m_Player = m_Players[0];

	m_Cameras[0]->SetOffset(XMFLOAT3(0.0f, 450.0f, -1320.0f));
	m_Cameras[0]->SetTarget(m_Players[0]);
	m_Players[0]->SetCamera(m_Cameras[0]);

	m_Players[0]->Scale(7, 7, 7);
	m_Players[0]->SetObjectName(OBJ_NAME::Player);
	m_Players[0]->Rotate({ 0,1,0 }, 180);
	//m_Players[0]->SetPosition({ 550.0f,   230.0f,  1850.0f });
	m_Players[0]->SetPosition({ 10500,  -2000, 13500 });
	

	m_Players[0]->SetDrawable(true);
	m_Players[0]->SetTextureIndex(0x400);

	m_Players[0]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 20, 72, 20, XMFLOAT3{ 0,0,0 });
	m_Players[0]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(10, 36, 10)));
	++m_CurrentPlayerNum;

	m_MinimapCamera->SetTarget(m_Players[0]);

	for (int i = 1; i < MAX_PLAYER; ++i) {
		//m_Players[i] = new CPlayer(pd3dDevice, pd3dCommandList);
		m_Players[i] = new CPlayer(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, m_pfbxManager, "resources/FbxExported/fbxsoldier.bin");
		//m_Players[i]->SetShader(CShaderHandler::GetInstance().GetData("FBX"));

		//m_PlayerCameras[i]->SetOffset(XMFLOAT3(0.0f, 450.0f, -1320.0f));
		//m_PlayerCameras[i]->SetTarget(m_Players[i]);
		//m_Players[i]->SetCamera(m_PlayerCameras[i]);

		m_Players[i]->Scale(7, 7, 7);
		m_Players[i]->SetObjectName(OBJ_NAME::Player);
		m_Players[i]->Rotate({ 0,1,0 }, 180);
		m_Players[i]->SetPosition({ 550.0f,   230.0f,  1850.0f });

		m_Players[i]->SetDrawable(false);
		m_Players[i]->SetTextureIndex(0x400);

		m_Players[i]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 20, 72, 20, XMFLOAT3{ 0,0,0 });
		m_Players[i]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(10, 36, 10)));
	}
}

void CSceneYJ::EnterNewSector(int sectorNum)
{
	if (m_BlockingPlateToPreviousSector.size() <= 0) return;

	CGameObject* obj = m_BlockingPlateToPreviousSector[sectorNum - 1];
	m_BlockingPlateToPreviousSector.erase(sectorNum - 1);

	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(obj);
	EnterNewSector(sectorNum - 1);
}

void CSceneYJ::ShotArrow()
{
	int i = 0;
	for (auto* pObj : m_ObjectLayers[(int)OBJECT_LAYER::Arrow]) {
		CArrow* pArrow = reinterpret_cast<CArrow*>(pObj);
		if (pArrow->IsCanUse()) {
			int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::ArrowParticle);
			if (-1 != idx) {
				cout << "파티클 인덱스 " << idx << " 화살 인덱스 : " << i << " \n";
				pArrow->SetUseable(false);
				XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Player->GetPosition() }, { 0,250,0 });
				pArrow->SetPosition(pos);
				pArrow->SetTargetVector(Vector3::Multifly(m_Player->GetLook(), -1));
				m_Particles->UseParticle(idx, pArrow->GetPosition(), XMFLOAT3(0.0f, 0.0f, -1.0f));
				m_Particles->SetDirection(idx, Vector3::Multifly(Vector3::Normalize(m_Player->GetLook()), 1));
				pArrow->ConnectParticle(m_Particles->GetParticleObj(idx));
			}
			break;
		}
		++i;
	}
}

void CSceneYJ::MakingFog()
{
	int i = 0;
	
	int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::RadialParitcle);
	if (-1 != idx)
	{
		XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Player->GetPosition() }, { 0,250,0 });
		m_Particles->UseParticle(idx, XMFLOAT3(16800.f, -5070.f, 17500.f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		//m_Particles->SetDirection(idx, Vector3::Multifly(Vector3::Normalize(m_Player->GetLook()), 1));
	}
}

void CSceneYJ::MakingRain()
{
	int i = 0;

	int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::RainParticle);
	if (-1 != idx)
	{
		XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Player->GetPosition() }, { 0,250,0 });
		m_Particles->UseParticle(idx, XMFLOAT3(5000.f, 0.f, 17500.f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		//m_Particles->SetDirection(idx, Vector3::Multifly(Vector3::Normalize(m_Player->GetLook()), 1));
	}
}

void CSceneYJ::MakingSandWind()
{
	int i = 0;

	int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::SandWindParticle);
	if (-1 != idx)
	{
		XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Player->GetPosition() }, { 0,250,0 });
		m_Particles->UseParticle(idx, XMFLOAT3(15000.f, -1000.f, 15000.f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		//m_Particles->SetDirection(idx, Vector3::Multifly(Vector3::Normalize(m_Player->GetLook()), 1));
	}
}

void CSceneYJ::SendMouseInputPacket()
{
	P_C2S_MOUSE_INPUT p_mouseInput;

	p_mouseInput.size = sizeof(p_mouseInput);
	p_mouseInput.type = PACKET_PROTOCOL::C2S_INGAME_MOUSE_INPUT;
	p_mouseInput.id = CFramework::GetInstance().GetPlayerId();
	p_mouseInput.inputNum = m_MousePositions.size();

	for (int i = 0; i < p_mouseInput.inputNum; ++i) {
		p_mouseInput.xInput[i] = FloatToInt(m_MousePositions[i].x);
		p_mouseInput.yInput[i] = FloatToInt(m_MousePositions[i].y);
	}
	p_mouseInput.InputType = m_prevMouseInputType;

	int retVal = 0;
	SendPacket(CFramework::GetInstance().GetSocket(),
		reinterpret_cast<char*>(&p_mouseInput), p_mouseInput.size, retVal);

	cout << "마우스 입력 전송 크기 : " << m_MousePositions.size() << "\n";
	m_MousePositions.clear();
}

void CSceneYJ::BuildBoundingRegions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 4개 벽 테두리
	CGameObject* pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 100, 10000, 20000, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f)));
	pObject->SetPosition({ 0,-2000,10000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 100, 10000, 20000, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f)));
	pObject->SetPosition({ 19950,-2000,10000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 20000, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(20000 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000,-2000,00 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 20000, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(20000 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000,-2000,19950 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// Forest to DryDesrt 아래 방향 벽  
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 9600, 800, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(9600 * 0.5f, 800 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 4800,-1000, 15900 });
	m_BlockingPlateToPreviousSector[0] = (std::move(pObject));
	//m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

// Forest to Desert 왼쪽 벽
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 800, 10000, 15200, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 15200 * 0.5f)));
	pObject->SetPosition({ 10000,-2000, 7600 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// Forest 지역 내 못가는 지형 
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 2000, 10000, 7000, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2000 * 0.5f, 10000 * 0.5f, 7000 * 0.5f)));
	pObject->SetPosition({ 4000 + 1000, -2000, 11100 });
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// Desrt to DryDesrt and Rock 왼쪽 벽
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 400, 10000, 12800, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(400 * 0.5f, 10000 * 0.5f, 12800 * 0.5f)));
	pObject->SetPosition({ 13800, -2000, 7200 + 6400 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// boss 지역 중간 벽
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 800, 10000, 5600, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f)));
	pObject->SetPosition({ 15200 + 400,-2000, 2800 + 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 800, 10000, 5600, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f)));
	pObject->SetPosition({ 17600 + 400,-2000, 2800 + 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	// 사막 지역 가로 벽
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 4000, 1000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(4000 * 0.5f, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 2000 + 9600,-2000, 15600 });
	m_BlockingPlateToPreviousSector[1] = (std::move(pObject));

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 4000, 1000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(4000 * 0.5f, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 2000 + 9600,-3000, 3600 });
	m_BlockingPlateToPreviousSector[2] = (std::move(pObject));

	// 보스 지역 입구 가로 벽
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 2400, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2400 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 1200 + 13600,-2000, 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 2400, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2400 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 1200 + 13600 + 1600 + 2400,-2000, 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
}

void CSceneYJ::RecvMouseProcessPacket()
{
	int retVal;
	char buffer[BUFSIZE + 1] = {};
	RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);

	PACKET_PROTOCOL type = (PACKET_PROTOCOL)buffer[1];

	// 플레이어 추가 혹은 삭제 패킷 수신
	if (type == PACKET_PROTOCOL::S2C_INGAME_MOUSE_INPUT) {
		P_S2C_PROCESS_MOUSE p_mouseProcess = *reinterpret_cast<P_S2C_PROCESS_MOUSE*>(&buffer);
		if (p_mouseProcess.cameraOffset != 0) {
			float offset = IntToFloat(p_mouseProcess.cameraOffset);
			cout << "offset : " << offset << "\n";
			m_CurrentCamera->MoveOffset(XMFLOAT3(0, 0, offset));
		}
		if (p_mouseProcess.cameraRotateX != 0) {
			m_CurrentCamera->RotateAroundTarget(XMFLOAT3(1, 0, 0), p_mouseProcess.cameraRotateX);
		}

		if (p_mouseProcess.cameraRotateY != 0) {
			float rotateY = IntToFloat(p_mouseProcess.cameraRotateY);
			//cout << "cameraRotateY : " << rotateY << "\n";
			m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 1, 0), rotateY);
		}

		if (p_mouseProcess.cameraRotateZ != 0) {
			m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 0, 1), p_mouseProcess.cameraRotateZ);
		}
		if (p_mouseProcess.playerRotateX != 0) {
			m_Player->Rotate(XMFLOAT3(1, 0, 0), p_mouseProcess.playerRotateX);
		}
		if (p_mouseProcess.playerRotateY != 0) {
			float rotateY = IntToFloat(p_mouseProcess.playerRotateY);
			//cout << "playerRotateY : " << rotateY << "\n";
			m_Player->Rotate(XMFLOAT3(0, 1, 0), rotateY);
			m_MinimapArrow->Rotate(-rotateY);
		}
		if (p_mouseProcess.playerRotateZ != 0) {
			m_Player->Rotate(XMFLOAT3(0, 0, 1), p_mouseProcess.playerRotateZ);
		}
	}
}
