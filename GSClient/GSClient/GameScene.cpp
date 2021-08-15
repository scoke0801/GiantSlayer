#include "stdafx.h"
#include "GameScene.h"

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
#include "Skill.h"
#include "MummyLaser.h"
#include "Enemy.h"
#include "Sound.h"
#include "FbxObject.h"
#include "FbxObject2.h"
#include "FbxLoader.h"
#include "Boss.h"
#include "Effect.h"
#include "Font.h"

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

XMFLOAT3 Mummy_scale = { 300.0f,300.0f,300.0f };
XMFLOAT3 scale = { 300.0f,300.0f,300.0f };
CGameScene::CGameScene()
{
	m_SoundManager = new CSoundManager();
	//m_SoundManager->AddStream("resources/sounds/TestTitle.mp3", Sound_Name::BGM_MAIN_GAME);

	m_SoundManager->AddSound("resources/sounds/ShotArrow.wav", Sound_Name::EFFECT_ARROW_SHOT);
	m_SoundManager->AddSound("resources/sounds/Laser.mp3", Sound_Name::EFFECT_Laser);
	m_SoundManager->AddSound("resources/sounds/ChessSuccess.mp3", Sound_Name::EFFECT_Chess_Success);
	m_SoundManager->AddSound("resources/sounds/FireBall.mp3", Sound_Name::EFFECT_Fire_Ball);
	m_SoundManager->AddSound("resources/sounds/Sword2.mp3", Sound_Name::EFFECT_Sword);
	//m_SoundManager->PlayBgm(Sound_Name::BGM_MAIN_GAME);

	cout << "Enter CGameScene \n";
	m_pd3dGraphicsRootSignature = NULL;
	m_isPlayerSelected = false;

	for (int i = 0; i < (int)FBX_MESH_TYPE::COUNT; ++i) {
		m_LoadedFbxMesh[i] = nullptr;
	}

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		m_PlayerExistingSector[i] = false;
	}
}

CGameScene::~CGameScene()
{

}

void CGameScene::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height)
{
	BuildMirrorResource(pd3dDevice);
	BuildMinimapResource(pd3dDevice);
	BuildShadowResource(pd3dDevice);

	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CShaderHandler::GetInstance().SetUserID(ShaderHandlerUser::GAMESCENE); // �����ʿ�
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

void CGameScene::BuildCamera(ID3D12Device* pd3dDevice,
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

	//m_MirrorCamera = m_Cameras[3];
	m_MinimapCamera = m_Cameras[1];

	m_MirrorCamera = new CCamera;
	m_MirrorCamera->SetLens(0.45f * PI, width, height, 1.0f, 60000.0f);
	m_MirrorCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
	m_MirrorCamera->SetScissorRect(0, 0, width, height);
	m_MirrorCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, width, height, 1.0f, 60000.0f);
		pCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
		pCamera->SetScissorRect(0, 0, width, height);
		pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		pCamera->SetPosition({ 500,  250 + 150, 1200 });
		pCamera->Pitch(XMConvertToRadians(15));
		pCamera->SetOffset(XMFLOAT3(0.0f, 1.5f, -4.0f));
		m_PlayerCameras.emplace_back(std::move(pCamera));
	}

	if (CFramework::GetInstance().IsOnConntected())
	{
		int id = CFramework::GetInstance().GetPlayerId();
		m_CurrentCamera = m_PlayerCameras[id];
		m_isPlayerSelected = true;
	}
	else {
		m_isPlayerSelected = true;
		m_CurrentCamera = m_PlayerCameras[0];
		//m_CurrentCamera = m_Cameras[2];
	}
}

void CGameScene::BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256�� ���
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void**)&m_pcbMappedMaterials);

	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CGameScene::BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}

void CGameScene::BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbMaterialBytes = ((sizeof(CB_GAMESCENE_FRAME_DATA) + 255) & ~255); //256�� ���
	m_pd3dcbSceneInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		NULL, ncbMaterialBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbSceneInfo->Map(0, NULL, (void**)&m_pcbMappedSceneFrameData);
}

void CGameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
		m_pd3dGraphicsRootSignature, 257 * 35 * MAP_SCALE_SIZE, 257 * 32 * MAP_SCALE_SIZE);
	pTerrainWater->SetPosition(XMFLOAT3(5450.0f * MAP_SCALE_SIZE, -1300.0f, 16500.0f * MAP_SCALE_SIZE));
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainWater].push_back(pTerrainWater);

	//FbxLoader(m_pfbxManager, "Elf_Mesh", false, 1);

	LoadFbxMeshes(pd3dDevice, pd3dCommandList);

	BuildMapSector1(pd3dDevice, pd3dCommandList);
	BuildMapSector2(pd3dDevice, pd3dCommandList);
	BuildMapSector3(pd3dDevice, pd3dCommandList);
	//BuildMapSector4(pd3dDevice, pd3dCommandList);
	//BuildMapSector5(pd3dDevice, pd3dCommandList);

	BuildBridges(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("Bridge"));

	BuildDoorWall(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("DoorWall"));
	BuildPuzzles(pd3dDevice, pd3dCommandList);

	BuildSigns(pd3dDevice, pd3dCommandList);
	BuildMirror(pd3dDevice, pd3dCommandList);
	BuildPlayers(pd3dDevice, pd3dCommandList);
	BuildNpc(pd3dDevice, pd3dCommandList);
	
	BuildParticles(pd3dDevice, pd3dCommandList);
	BuildArrows(pd3dDevice, pd3dCommandList);
	BuildMummyLaser(pd3dDevice, pd3dCommandList);
	BuildProjectiles(pd3dDevice, pd3dCommandList);

	BuildEnemys(pd3dDevice, pd3dCommandList);
	BuildBoundingRegions(pd3dDevice, pd3dCommandList);

	m_EffectsHandler = new CEffectHandler();
	m_EffectsHandler->Init(pd3dDevice, pd3dCommandList, m_Player);

	/*pfbxTestObject = new CFbxObject2(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, m_pfbxManager, "resources/Fbx/human.fbx");
	pfbxTestObject->SetAnimationStack(2);
	pfbxTestObject->m_pAnimationController->SetPosition(0, 0.0f);
	pfbxTestObject->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	pfbxTestObject->SetPosition({ 1000,  150, 1000 });
	pfbxTestObject->SetTextureIndex(0x01);*/

	auto end_t = chrono::high_resolution_clock::now();

	auto elasepsed_t = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_t);
	cout << "InitElapsed Time : " << elasepsed_t.count() * 0.000001f << "\n";
}

void CGameScene::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	const char* keyNames[] =
	{
		"Forest","Dry_Forest","Desert",
		"Dry_Desert","Rocky_Terrain","BossWall",
		"Sky_Front","Sky_Back", "Sky_Left", "Sky_Right","Sky_Top","Sky_Bottom",
		"Box","Wood", "WoodSignBoard",
		"GrassWall", "SandWall","RockyWall",
		"Door",
		"HP_SP", "Minimap", "WeaponUI",
		"HP_SP_PER",
		"Player_Clothes", "Player_Face", "Player_Hair", "Player_Sword", "Player_Bow", "Player_Staff", 
		"Tree", "NoLeafTree", "Leaves", "Moss_Rock",
		"PuzzleBoard",
		"HelpText",
		"Dry_Tree", "Stump", "Dead_Tree",
		"Desert_Rock",
		"TerrainWater",
		"Rain", "FogParticle",
		"Boss_D", "Boss_C","Boss_E","Boss_N",
		"MeleeSkeleton_01_D",
		"MeleeSkeleton_02","MeleeSkeleton_02_Equip", "MeleeSkeleton_02_EquipAll",
		"Mat01_mummy_A","Mat01_mummy_M",
		"GreenTree",
		"Effect_1", "Effect_2", "Effect_3", "Effect_4",
		"FireBall",
		"KingDiffuse","KnightDiffuse","PawnDiffuse","RookDiffuse",
		"ChessTile",
		"Laser",
		"Npc_A","Npc_M",
		"HelpBoard"
	};

	const wchar_t* address[] =
	{
		L"resources/OBJ/Forest.dds",L"resources/OBJ/Dry_Forest.dds",L"resources/OBJ/Desert.dds",
		L"resources/OBJ/Dry_Desert.dds",L"resources/OBJ/Rocky_Terrain.dds",L"resources/OBJ/bossWall.dds",
		L"resources/skybox/front.dds",L"resources/skybox/back.dds", L"resources/skybox/left.dds",L"resources/skybox/right.dds",L"resources/skybox/top.dds", L"resources/skybox/bottom.dds",
		L"resources/OBJ/Box.dds",L"resources/OBJ/Wood.dds", L"resources/OBJ/WoodSignBoard.dds",
		L"resources/OBJ/GrassWallTexture.dds", L"resources/OBJ/StoneWallTexture.dds",L"resources/OBJ/RockyWall.dds",
		L"resources/OBJ/Door.dds",
		L"resources/UI/HP_SP.dds", L"resources/UI/Minimap.dds", L"resources/UI/Weapon.dds",L"resources/UI/SmallICons.dds",
		L"resources/Textures/clothingSet_01_tex.dds",L"resources/Textures/girl_texture_01.dds",L"resources/Textures/hair1.dds",L"resources/Textures/sword1.dds", L"resources/Textures/bow_texture.dds", L"resources/Textures/twoHandedStaff_texture.dds",
		L"resources/Billboard/Tree02.dds",L"resources/Billboard/NoLeafTree2.dds",L"resources/OBJ/Leaves.dds",L"resources/OBJ/ROck_Texture_Surface2.dds",
		L"resources/OBJ/Board.dds",
		L"resources/UI/HelpText.dds",
		L"resources/OBJ/Dry_Tree.dds",L"resources/OBJ/Stump.dds",L"resources/OBJ/Dead_Tree.dds",
		L"resources/OBJ/Desert_Rock.dds",
		L"resources/OBJ/Water.dds",
		L"resources/OBJ/Rain.dds",		L"resources/OBJ/FogParticle.dds",
		L"resources/Textures/Body_D.dds",L"resources/Textures/Body_C.dds",L"resources/Textures/Body_E.dds",L"resources/Textures/Body_N.dds",
		L"resources/Textures/Skeleton_D.dds",
		L"resources/Textures/DemoSkeleton.dds", L"resources/Textures/DemoEquipment.dds",L"resources/Textures/DS_equipment_standard.dds",
		L"resources/Textures/Mat01_mummy_A.dds",L"resources/Textures/Mat01_mummy_M.dds",
		L"resources/OBJ/GreenTree.dds",
		L"resources/Effects/effect_1.dds", L"resources/Effects/Thunder.dds",L"resources/Effects/warnninggCircle.dds", L"resources/Effects/FireBall_Explosion.dds",
		L"resources/Textures/FireBall.dds",
		L"resources/Textures/KingDiffuse.dds",L"resources/Textures/KnightDiffuse.dds",L"resources/Textures/PawnDiffuse.dds",L"resources/Textures/RookDiffuse.dds",
		L"resources/OBJ/ChessTile.dds",
		L"resources/Textures/LightningSpriteSheet2.dds",
		L"resources/Textures/Elf_Albedo.dds",L"resources/Textures/Elf_Metallic.dds",
		L"resources/UI/HelpBoard.dds",

	};

	for (int i = 0; i < _countof(keyNames); ++i)
	{
		unique_ptr<CTexture> tempTex = make_unique<CTexture>();
		MakeTexture(pd3dDevice, pd3dCommandList, tempTex.get(), keyNames[i], address[i]);
		m_Textures[tempTex->m_Name] = std::move(tempTex);
	}
}

void CGameScene::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// Create the SRV heap. 
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size() + 4;
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
		"Forest","Dry_Forest","Desert",
		"Dry_Desert","Rocky_Terrain","BossWall",
		"Sky_Front","Sky_Back", "Sky_Left", "Sky_Right","Sky_Top","Sky_Bottom",
		"Box","Wood", "WoodSignBoard",
		"GrassWall", "SandWall","RockyWall",
		"Door",
		"HP_SP", "Minimap", "WeaponUI",
		"HP_SP_PER",
		"Player_Clothes", "Player_Face", "Player_Hair", "Player_Sword", "Player_Bow", "Player_Staff",
		"Tree", "NoLeafTree", "Leaves", "Moss_Rock",
		"PuzzleBoard",
		"HelpText",
		"Dry_Tree", "Stump", "Dead_Tree",
		"Desert_Rock",
		"TerrainWater",
		"Rain", "FogParticle",
		"Boss_D", "Boss_C","Boss_E","Boss_N",
		"MeleeSkeleton_01_D",
		"MeleeSkeleton_02","MeleeSkeleton_02_Equip", "MeleeSkeleton_02_EquipAll",
		"Mat01_mummy_A","Mat01_mummy_M",
		"GreenTree",
		"Effect_1", "Effect_2", "Effect_3", "Effect_4",
		"FireBall",
		"KingDiffuse","KnightDiffuse","PawnDiffuse","RookDiffuse",
		"ChessTile",
		"Laser",
		"Npc_A","Npc_M",
		"HelpBoard",
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

	// �׸��� ���̰�
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	pd3dDevice->CreateDepthStencilView(m_pd3dShadowMap, &dsvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, gnDsvDescriptorIncrementSize));

	TextHandler::GetInstance().InitVertexBuffer(pd3dDevice, pd3dCommandList, m_pd3dSrvDescriptorHeap, m_Textures.size() + 3);

	m_d3dDsvShadowMapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, gnDsvDescriptorIncrementSize);
	m_d3dSrvShadowMapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_Textures.size() + 4, gnCbvSrvDescriptorIncrementSize);
}

void CGameScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	//m_Objects.clear();
}

void CGameScene::Update(float elapsedTime)
{
	m_SoundManager->OnUpdate();
	ProcessInput();

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			if (false == pObject->IsInNearSector(m_PlayerExistingSector)) {
				continue;
			}
			pObject->Update(elapsedTime);
			pObject->UpdateColliders();
		}
	}

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy]) {
		pEnemy->FixPositionByTerrain(m_Terrain);
	}
	for (auto pMummy : m_ObjectLayers[(int)OBJECT_LAYER::Mummy]) {
		pMummy->FixPositionByTerrain(m_Terrain);
	}

	m_EffectsHandler->Update(elapsedTime);

	m_Particles->Update(elapsedTime);

	m_HelpTextUI->Update(elapsedTime);

	if (m_Interaction == true)
	{
		m_HelpBoard->SetDrawable(true);
	}
	else if (m_Interaction==false)
	{
		m_HelpBoard->SetDrawable(false);
	}

	ZeroMemory(m_PlayerExistingSector, sizeof(m_PlayerExistingSector));
	for (auto player : m_Players) {
		if (!player->IsDrawable()) continue;
		player->Update(elapsedTime);
		player->UpdateColliders();
		player->FixPositionByTerrain(m_Terrain);
		player->FixCameraByTerrain(m_Terrain);
	}
	m_PlayerExistingSector[m_Player->GetPlayerExistingSector()] = true;

	

	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary]) {
		if (pObstacle->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pObstacle);
		}
	}

	// NPC�� �浹
	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Npc]) {
		if (false == pObstacle->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pObstacle->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pObstacle);
		}
	}

	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Obstacle]) {
		if (false == pObstacle->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pObstacle->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pObstacle);
		}
	}

	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Bridge]) {
		if (false == pObstacle->IsInNearSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pObstacle->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pObstacle);
			m_Player->UpdateCamera();
		}
	}
	 
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser]) {
		// ������ �������� ���� true/false ����..
		if (true == pArrow->IsDrawable()) {
			continue;
		}
		for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::MirrorBox])
		{
			if (pArrow->CollisionCheck(pEnemy)) {
				pArrow->InverseDirection();
				break;
			}
		}
	}

	
	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser]) {
		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pEnemy->CollisionCheck(m_Player)) {
			// ���� ������ ���� ü���� ��°��� ������...
			//if (ObjectState::Attack == pEnemy->GetStateInfo()) {
			//	
			//}
			if (false == m_Player->IsCanAttack()) {
				if (false == m_Player->IsAleradyAttack()) {
					pEnemy->ChangeState(ObjectState::Attacked, m_Player);
					m_Player->SetAleradyAttack(true);
				}
			}
			else if (m_Player->Attacked(pEnemy))
			{
				m_CurrentCamera->SetShake(true, 0.5f, 15);
				m_Player->FixCollision();
			}
		}
		//if (pEnemy->CollisionCheck(m_Mirror[0])) {
		//	//pEnemy->InverseDirection();
		//	pEnemy->SetTargetVector(XMFLOAT3(0, 0, 150));
		//}
	}

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser]) {
		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pEnemy->CollisionCheck(m_Player)) {
			// ���� ������ ���� ü���� ��°��� ������...
			//if (ObjectState::Attack == pEnemy->GetStateInfo()) {
			//	
			//}
			if (false == m_Player->IsCanAttack()) {
				if (false == m_Player->IsAleradyAttack()) {
					pEnemy->ChangeState(ObjectState::Attacked, m_Player);
					m_Player->SetAleradyAttack(true);
				}
			}
			else if (m_Player->Attacked(pEnemy))
			{
				m_CurrentCamera->SetShake(true, 0.5f, 15);
				m_Player->FixCollision();
			}
		}
		//if (pEnemy->CollisionCheck(m_Mirror[0])) {
		//	//pEnemy->InverseDirection();
		//	pEnemy->SetTargetVector(XMFLOAT3(0, 0, 150));
		//}
	}
	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser2]) {
		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pEnemy->CollisionCheck(m_Player)) {
			// ���� ������ ���� ü���� ��°��� ������...
			//if (ObjectState::Attack == pEnemy->GetStateInfo()) {
			//	
			//}
			if (false == m_Player->IsCanAttack()) {
				if (false == m_Player->IsAleradyAttack()) {
					pEnemy->ChangeState(ObjectState::Attacked, m_Player);
					m_Player->SetAleradyAttack(true);
				}
			}
			else if (m_Player->Attacked(pEnemy))
			{
				m_CurrentCamera->SetShake(true, 0.5f, 15);
				m_Player->FixCollision();
			}
		}
	}
	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser3]) {
		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pEnemy->CollisionCheck(m_Player)) {
			// ���� ������ ���� ü���� ��°��� ������...
			//if (ObjectState::Attack == pEnemy->GetStateInfo()) {
			//	
			//}
			if (false == m_Player->IsCanAttack()) {
				if (false == m_Player->IsAleradyAttack()) {
					pEnemy->ChangeState(ObjectState::Attacked, m_Player);
					m_Player->SetAleradyAttack(true);
				}
			}
			else if (m_Player->Attacked(pEnemy))
			{
				m_CurrentCamera->SetShake(true, 0.5f, 15);
				m_Player->FixCollision();
			}
		}
	}

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Mummy]) {
		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		if (pEnemy->CollisionCheck(m_Player)) {

			if (false == m_Player->IsCanAttack()) {
				if (false == m_Player->IsAleradyAttack()) {
					CMummy* thisEnemy = reinterpret_cast<CMummy*>(pEnemy);

					thisEnemy->SendDieInfotoFriends();

					DeleteEnemy(thisEnemy);

					m_Player->SetAleradyAttack(true);
				}
			}
			else if (m_Player->Attacked(pEnemy))
			{
				m_CurrentCamera->SetShake(true, 0.5f, 15);
				m_Player->FixCollision();
			}
		}
	}

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy]) {
		for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Obstacle]) {
			if (false == pEnemy->IsInSameSector(pObstacle->GetExistingSector())) {
				continue;
			}
			if (pObstacle->CollisionCheck(pEnemy)) {
				CEnemy* thisEnemy = reinterpret_cast<CEnemy*>(pEnemy);
				thisEnemy->FixCollision(pObstacle);

			}
		}

		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}

		if (pEnemy->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pEnemy);
		}

		if (false == m_Player->IsCanAttack() && pEnemy->IsCanDamaged()) {
			if (pEnemy->CollisionCheck(m_Player->m_pWeapon)) {
				pEnemy->ChangeState(ObjectState::Attacked, m_Player);
				m_Player->SetAleradyAttack(true);
				break;
			}
		}
		else if (pEnemy->GetStateInfo() == ObjectState::Attack)
		{
			if (pEnemy->CollisionCheck(m_Player)) {
				m_Player->Attacked(pEnemy);
				m_CurrentCamera->SetShake(true, 0.5f, 15);
				m_Player->FixCollision();
			}
		}

		//if (pEnemy->CollisionCheck(m_Player)) {
		//	// ���� ������ ���� ü���� ��°��� ������...
		//	//if (ObjectState::Attack == pEnemy->GetStateInfo()) {
		//	//	
		//	//}
		//	if (false == m_Player->IsCanAttack() && pEnemy->IsCanDamaged()) {
		//		if (pEnemy->CollisionCheck(m_Player->m_pWeapon)) {
		//			pEnemy->ChangeState(ObjectState::Attacked, m_Player);
		//		}
		//		m_Player->SetAleradyAttack(true);
		//	}
		//	/*else if (m_Player->Attacked(pEnemy))
		//	{
		//		m_CurrentCamera->SetShake(true, 0.5f, 15);
		//		m_Player->FixCollision();
		//	}*/
		//}
	}
	
	for (auto pMummy : m_ObjectLayers[(int)OBJECT_LAYER::Mummy])
	{
		// ���ʺ�
		if (pMummy->GetPosition().x < 21269)
		{
			m_Mummy_Reverse_Direction = true;
		}
		else if (pMummy->GetPosition().x > 21369)
		{
			m_Mummy_Reverse_Direction = false;
		}
		if (m_Mummy_Reverse_Direction == true)
		{
			pMummy->SetTargetVector(Vector3::Multifly(XMFLOAT3(180, 0, 0), 1));
		}
		if (pMummy->GetPosition().x > 29485)
		{
			m_Mummy_Reverse_Direction = true;
		}
		else if (pMummy->GetPosition().x < 29385)
		{
			m_Mummy_Reverse_Direction = false;
		}
		if (m_Mummy_Reverse_Direction == true)
		{
			pMummy->SetTargetVector(Vector3::Multifly(XMFLOAT3(180, 0, 0), -1));
		}
	}
	
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow]) {
		// ������ �������� ���� true/false ����..
		if (true == pArrow->IsDrawable()) {
			continue;
		}
		if (pArrow->CollisionCheck(m_Player)) {
			if (m_Player->Attacked(pArrow)) {
				m_CurrentCamera->SetShake(true, 0.5f, 15);
				pArrow->SetDrawable(true);
			}
		}
	}

	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow]) {
		// ������ �������� ���� true/false ����..
		if (true == pArrow->IsDrawable()) {
			continue;
		}
		for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy])
		{
			if (pArrow->CollisionCheck(pEnemy)) {
				pEnemy->ChangeState(ObjectState::Attacked, pArrow);
				pArrow->SetDrawable(true);

				cout << "�浹 : �÷��̾� ȭ�� - ��\n";
				break;
			}
		}
	}
	for (auto pFireball : m_ObjectLayers[(int)OBJECT_LAYER::FireBall]) {
		// ������ �������� ���� true/false ����..
		if (true == pFireball->IsDrawable()) {
			continue;
		}
		for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy])
		{
			if (pFireball->CollisionCheck(pEnemy)) {
				pEnemy->ChangeState(ObjectState::Attacked, pFireball);
				pFireball->SetDrawable(true);

				cout << "�浹 : �÷��̾� ���̾ - ��\n";
				break;
			}
		}
	}
	for (auto pPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::Puzzle]) {
		if (pPuzzle->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pPuzzle);
			m_isPlayerBoxCollide = true;
			m_Player->UpdateCamera();
			break;
		}
	}


	for (auto pChessPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle]) {
		if (pChessPuzzle->CollisionCheck(m_Player)) {
			m_Player->FixCollision(pChessPuzzle);
			m_isPlayerBoxCollide = true;
			m_Player->UpdateCamera();
			break;
		}
	}

	// ���� ��ġ ����
	XMFLOAT3 lookVec = Vector3::Normalize(m_Player->GetLook());
	XMFLOAT3 Final_Vec = Vector3::Multifly(lookVec, 5.0f);
	XMFLOAT3 Speed = { 0.3f,0.3f,0.3f };
	
	// ü�� ���� �浹ó��
	for (auto pPlayerChessPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle]) {

		if (pPlayerChessPuzzle->CollisionCheck(m_Player))
		{
			m_Player->FixCollision(pPlayerChessPuzzle);
			m_isPlayerBoxCollide = true;
			m_Player->UpdateCamera();

			break;
		}
	}

	// npc ��ü�� �浹ó��
	if	(
		((m_Npc->GetPosition().x + 215.0f > m_Player->GetPosition().x)
			&& (m_Npc->GetPosition().x - 215.0f < m_Player->GetPosition().x))
		&&
		((m_Npc->GetPosition().z + 225.0f > m_Player->GetPosition().z)
			&& (m_Npc->GetPosition().z - 225.0f < m_Player->GetPosition().z))
		)
	{
		m_Npc_Event = true;
	}
	else
	{
		m_Npc_Event = false;
	}
		
	// ü�� �� ��ü�� �浹ó��
	for (int i = 0; i < ChessType::Count; i++)
	{
		if (
			(
				(m_Chess[i]->GetPosition().x + 215.0f > m_Player->GetPosition().x)
				&& (m_Chess[i]->GetPosition().x - 215.0f < m_Player->GetPosition().x)
				)
			&&
			(
				(m_Chess[i]->GetPosition().z + 225.0f > m_Player->GetPosition().z)
				&& (m_Chess[i]->GetPosition().z - 225.0f < m_Player->GetPosition().z)
				)

			)
		{
			m_ChessCollide_Check[i] = true;
		}
		else
		{
			m_ChessCollide_Check[i] = false;
		}

	}
	for (int i = 0; i < ChessType::Count; i++)
	{
		if (m_ChessCollide_Check[i] == true)
		{
			m_Player->Box_Pull(TRUE);
			m_Chess[i]->SetPosition({
									m_Chess[i]->GetPosition().x + Final_Vec.x,
									m_Chess[i]->GetPosition().y ,
									m_Chess[i]->GetPosition().z + Final_Vec.z
				});
			if (m_Chess[i]->GetPosition().x < 16500.0f)
			{
				m_Chess[i]->SetPosition({
									16500.0f ,
									m_Chess[i]->GetPosition().y ,
									m_Chess[i]->GetPosition().z
					});
			}
			if (m_Chess[i]->GetPosition().x > 19300.0f)
			{
				m_Chess[i]->SetPosition({
									19300.0f ,
									m_Chess[i]->GetPosition().y ,
									m_Chess[i]->GetPosition().z
					});
			}
			if (m_Chess[i]->GetPosition().z < 15600.0f)
			{
				m_Chess[i]->SetPosition({
									m_Chess[i]->GetPosition().x ,
									m_Chess[i]->GetPosition().y ,
									15600.0f
					});
			}
			if (m_Chess[i]->GetPosition().z > 17800.0f)
			{
				m_Chess[i]->SetPosition({
									m_Chess[i]->GetPosition().x ,
									m_Chess[i]->GetPosition().y ,
									17800.0f
					});
			}
		}
		else if (m_ChessCollide_Check[King] == false && m_ChessCollide_Check[Knight] == false && m_ChessCollide_Check[Pawn] == false && m_ChessCollide_Check[Rook] == false)
		{
			m_Player->Box_Pull(FALSE);
		}
	}
	
	// 1 ŷ 2 ����Ʈ 3 �� 4 �� // ���� üũ 

	if ((m_Chess[King]->GetPosition().x > 18320.0f && m_Chess[King]->GetPosition().x < 18720.0f)
		&& (m_Chess[King]->GetPosition().z > 17150.0f && m_Chess[King]->GetPosition().z < 17550.0f))
	{
		m_ChessPlate_Check[King] = true;
	}
	if ((m_Chess[Knight]->GetPosition().x > 18320.0f && m_Chess[Knight]->GetPosition().x < 18720.0f)
		&& (m_Chess[Knight]->GetPosition().z > 15950.0f && m_Chess[Knight]->GetPosition().z < 16350.0f))
	{
		m_ChessPlate_Check[Knight] = true;
	}
	if ((m_Chess[Pawn]->GetPosition().x > 17720.0f && m_Chess[Pawn]->GetPosition().x < 18120.0f)
		&& (m_Chess[Pawn]->GetPosition().z > 16550.0f && m_Chess[Pawn]->GetPosition().z < 16950.0f))
	{
		m_ChessPlate_Check[Pawn] = true;
	}
	if ((m_Chess[Rook]->GetPosition().x > 17140.0f && m_Chess[Rook]->GetPosition().x < 17520.0f)
		&& (m_Chess[Rook]->GetPosition().z > 15950.0f && m_Chess[Rook]->GetPosition().z < 16350.0f))
	{
		m_ChessPlate_Check[Rook] = true;
	}
	
	if (m_ChessPlate_Check[King] && m_ChessPlate_Check[Knight] && m_ChessPlate_Check[Pawn] && m_ChessPlate_Check[Rook])
	{
		m_SoundManager->PlayEffect(Sound_Name::EFFECT_Chess_Success);
		CDoorWall* p = reinterpret_cast<CDoorWall*>(m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][m_DoorIdx + 1]);
		p->OpenDoor();
	}

	//m_PlayerCameras[CFramework::GetInstance().GetPlayerId()]->Update(elapsedTime);

	if (m_CurrentCamera) m_CurrentCamera->Update(elapsedTime);

	if (m_MirrorCamera)
	{
		m_MirrorCamera->UpdateViewMatrix();
	}

	if (m_pLightCamera)
	{
		LightPos = m_Player->GetPosition();
		XMFLOAT3 TempPlayerPosition = m_Player->GetPosition();

		LightPos.y = 3000.0f;
		LightPos.z += 5000.0f; 
		m_pLightCamera->LookAt(LightPos,
			{ TempPlayerPosition },
			m_Player->GetUp()
		); 

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

void CGameScene::UpdateForMultiplay(float elapsedTime)
{
	m_SoundManager->OnUpdate();
	ProcessInput();

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->UpdateOnServer(elapsedTime);
			pObject->UpdateColliders();
		}
	}
	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy]) {
		pEnemy->FixPositionByTerrain(m_Terrain);
	}
	m_Particles->Update(elapsedTime);

	m_HelpTextUI->Update(elapsedTime);

	ZeroMemory(m_PlayerExistingSector, sizeof(m_PlayerExistingSector));
	for (auto player : m_Players) {
		if (!player->IsDrawable()) continue;
		player->UpdateOnServer(elapsedTime);
		player->UpdateColliders(); 
		m_PlayerExistingSector[player->GetPlayerExistingSector()] = true;
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
		LightPos.z += 5000.0f;

		m_pLightCamera->LookAt({ LightPos },
			{ m_Player->GetPosition().x,m_Player->GetPosition().y,m_Player->GetPosition().z },
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

void CGameScene::AnimateObjects(float fTimeElapsed)
{
}

void CGameScene::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}
	 
	m_pcbMappedSceneFrameData->m_PlayerHP = m_Player->GetHP();
	m_pcbMappedSceneFrameData->m_PlayerSP = m_Player->GetSP();
	m_pcbMappedSceneFrameData->m_PlayerWeapon = m_Player->GetSelectedWeapon();
	auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		chrono::high_resolution_clock::now() - m_CreatedTime);
	m_pcbMappedSceneFrameData->m_Time = timeElapsed.count() * 0.001f;
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData
	 
	m_Skybox->Draw(pd3dCommandList, m_CurrentCamera);
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);
	for (auto mirror : m_Mirror) {
		mirror->Draw(pd3dCommandList, m_CurrentCamera);
	}
	 
	m_Particles->Draw(pd3dCommandList, m_CurrentCamera);
	m_EffectsHandler->Draw(pd3dCommandList, m_CurrentCamera);

	auto playerPos = m_Player->GetPosition();
	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		if (i == (int)OBJECT_LAYER::TerrainWater) {
			continue;
		} 
		else {
			for (auto pObject : m_ObjectLayers[i]) {
				if (false == pObject->IsInNearSector(m_PlayerExistingSector)) {
					continue;
				}
				pObject->Draw(pd3dCommandList, m_CurrentCamera);
			}
		}
	}

	for (auto player : m_Players) {
		if (!player->IsDrawable()) continue;
		player->Draw(pd3dCommandList, m_CurrentCamera);
	}

	m_ObjectLayers[(int)OBJECT_LAYER::TerrainWater][0]->Draw(pd3dCommandList, m_CurrentCamera);

	// draw the text
	//TextHandler::GetInstance().Render(pd3dCommandList, std::wstring(L"TEST"), XMFLOAT2(0.02f, 0.01f), XMFLOAT2(200.0f, 200.0f));
}

void CGameScene::DrawUI(ID3D12GraphicsCommandList* pd3dCommandList)
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

void CGameScene::DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_Player) m_Player->Draw(pd3dCommandList, m_CurrentCamera);
}

void CGameScene::FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameScene::DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV)
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

	for (auto pObject : m_ObjectLayers[(int)OBJECT_LAYER::Obstacle]) {
		pObject->Draw(pd3dCommandList, m_CurrentCamera);
	}
	for (auto pObject : m_ObjectLayers[(int)OBJECT_LAYER::TerrainWater]) {
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

void CGameScene::DrawFont(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// draw the text
	TextHandler::GetInstance().Render(pd3dCommandList, std::wstring(L"����1234abcd!@#"), 0,
		XMFLOAT2(0.02f, 0.51f), XMFLOAT2(2.0f, 2.0f));
}

void CGameScene::DrawMirror(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV)
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
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);
	m_Player->Draw(pd3dCommandList, m_CurrentCamera);
	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		if (i == (int)OBJECT_LAYER::MirrorBox) {
			continue;
		}
		if (i == (int)OBJECT_LAYER::Enemy) {
			continue;
		}
		for (auto pObject : m_ObjectLayers[i]) {
			if (false == pObject->IsInSameSector(m_PlayerExistingSector)) {
				continue;
			}
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

void CGameScene::DrawShadow(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_isPlayerSelected == true)
	{
		pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

		if (m_pLightCamera)
		{
			m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
			m_pLightCamera->GenerateViewMatrix();
			m_pLightCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_LIGHT_CAMERA);
			//m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		}

		pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dShadowMap,
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		pd3dCommandList->ClearDepthStencilView(m_d3dDsvShadowMapCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		pd3dCommandList->OMSetRenderTargets(0, NULL, FALSE, &m_d3dDsvShadowMapCPUHandle);
		  
		auto playerPos = m_Player->GetPosition();
		for (int i = 0; i < m_ObjectLayers.size(); ++i) {
			for (auto pObject : m_ObjectLayers[i]) {
				if (false == pObject->IsInNearSector(m_PlayerExistingSector)) {
					continue;
				}
				pObject->Draw_Shadow(pd3dCommandList, m_pLightCamera);
			}
		}


		for (auto player : m_Players) {
			if (!player->IsDrawable()) continue;
			player->Draw_Shadow(pd3dCommandList, m_pLightCamera);
		}

		pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dShadowMap,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
	} 
}

void CGameScene::Communicate(SOCKET& sock)
{
	P_C2S_UPDATE_SYNC_REQUEST p_syncUpdateRequest;
	p_syncUpdateRequest.size = sizeof(P_C2S_UPDATE_SYNC_REQUEST);
	p_syncUpdateRequest.type = PACKET_PROTOCOL::C2S_INGAME_UPDATE_SYNC;
	p_syncUpdateRequest.playerNum = m_CurrentPlayerNum;

	int retVal;
	bool haveToRecv = false; 

	if (m_MousePositions.size() > 0) {
		SendMouseInputPacket(); 
	}
}

void CGameScene::ProcessPacket(unsigned char* p_buf)
{
	char buf[10000];
	PACKET_PROTOCOL type = (PACKET_PROTOCOL)p_buf[1];
	switch (type)
	{
	case PACKET_PROTOCOL::S2C_LOGIN_HANDLE:
		P_S2C_PROCESS_LOGIN p_processLogin; //= *reinterpret_cast<P_S2C_PROCESS_LOGIN*>(&p_buf);
		memcpy(&p_processLogin, p_buf, p_buf[0]);
		if (p_processLogin.isSuccess)
		{
			XMFLOAT3 pos = XMFLOAT3{ IntToFloat(p_processLogin.x),
				IntToFloat(p_processLogin.y), IntToFloat(p_processLogin.z) };

			CFramework::GetInstance().SetPlayerId(p_processLogin.id);

			cout << "Login id = " << p_processLogin.id << "\n";

			m_Players[p_processLogin.id]->SetDrawable(true);

			m_Player = m_Players[p_processLogin.id];
			m_CurrentCamera = m_PlayerCameras[p_processLogin.id]; 

			m_MinimapCamera->SetTarget(m_Players[p_processLogin.id]);

			for (int i = 0; i < 5; ++i) {
				CDoorWall* p = reinterpret_cast<CDoorWall*>(m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][m_DoorIdx + i]);
				p->OpenDoor();
			} 
		}
		break;
	case PACKET_PROTOCOL::S2C_NEW_PLAYER:
		cout << "Packet::NewPlayer[ServerToClient]\n";
		P_S2C_ADD_PLAYER p_addPlayer;
		memcpy(&p_addPlayer, p_buf, p_buf[0]);
		XMFLOAT3 pos = { IntToFloat(p_addPlayer.x), IntToFloat(p_addPlayer.y), IntToFloat(p_addPlayer.z) };

		m_Players[p_addPlayer.id]->SetPosition(pos);
		m_Players[p_addPlayer.id]->SetDrawable(true);
		++m_CurrentPlayerNum;
		break;
	case PACKET_PROTOCOL::S2C_DELETE_PLAYER:
		cout << "Packet::DeletePlayer[ServerToClient]\n";
		P_S2C_DELETE_PLAYER p_deletePlayer;
		memcpy(&p_deletePlayer, p_buf, p_buf[0]);
		m_Players[p_deletePlayer.id]->SetDrawable(false);
		break;
	case PACKET_PROTOCOL::S2C_INGAME_KEYBOARD_INPUT:
	{
		P_S2C_PROCESS_KEYBOARD p_keyboardProcess;
		memcpy(&p_keyboardProcess, p_buf, p_buf[0]);
		XMFLOAT3 pos = XMFLOAT3{ IntToFloat(p_keyboardProcess.posX),
			IntToFloat(p_keyboardProcess.posY),
			IntToFloat(p_keyboardProcess.posZ) };
		XMFLOAT3 look = XMFLOAT3{ IntToFloat(p_keyboardProcess.lookX),
			IntToFloat(p_keyboardProcess.lookY),
			IntToFloat(p_keyboardProcess.lookZ) };

		m_Player->SetPosition(pos);

		m_Player->SetVelocity(Vector3::Add(XMFLOAT3(0, 0, 0),
			look, -PLAYER_RUN_SPEED));
	}
	break;
	case PACKET_PROTOCOL::S2C_INGAME_MOUSE_INPUT:
		P_S2C_PROCESS_MOUSE p_mouseProcess;
		memcpy(&p_mouseProcess, p_buf, p_buf[0]);
		if (p_mouseProcess.cameraOffset != 0) {
			float offset = IntToFloat(p_mouseProcess.cameraOffset);
			//cout << "offset : " << offset << "\n";
			m_CurrentCamera->MoveOffset(XMFLOAT3(0, 0, offset));
		}
		/*if (p_mouseProcess.cameraRotateX != 0) {
			m_CurrentCamera->RotateAroundTarget(XMFLOAT3(1, 0, 0), p_mouseProcess.cameraRotateX);
		}*/

		if (p_mouseProcess.cameraRotateY != 0) {
			float rotateY = IntToFloat(p_mouseProcess.cameraRotateY);
			//cout << "cameraRotateY : " << rotateY << "\n";
			m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 1, 0), rotateY);
		}

		//if (p_mouseProcess.cameraRotateZ != 0) {
		//	m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 0, 1), p_mouseProcess.cameraRotateZ);
		//}
		/*if (p_mouseProcess.playerRotateX != 0) {
			m_Player->Rotate(XMFLOAT3(1, 0, 0), p_mouseProcess.playerRotateX);
		}*/
		if (p_mouseProcess.playerRotateY != 0) {
			float rotateY = IntToFloat(p_mouseProcess.playerRotateY);
			//cout << "playerRotateY : " << rotateY << "\n";
			m_Player->Rotate(XMFLOAT3(0, 1, 0), rotateY);
			//m_MinimapArrow->Rotate(-rotateY * 0.1f);
		}
		/*if (p_mouseProcess.playerRotateZ != 0) {
			m_Player->Rotate(XMFLOAT3(0, 0, 1), p_mouseProcess.playerRotateZ);
		}*/
		break;
	case PACKET_PROTOCOL::S2C_INGAME_MONSTER_ACT:
	{
		//P_S2C_MONSTERS_UPDATE_SYNC p_monsterUpdate;
		//memcpy(&p_monsterUpdate, p_buf, p_buf[0]);
		P_S2C_MONSTERS_UPDATE_SYNC* p_monsterUpdate = reinterpret_cast<P_S2C_MONSTERS_UPDATE_SYNC*>(p_buf);

		XMFLOAT3 pos = { IntToFloat(p_monsterUpdate->posX),
			IntToFloat(p_monsterUpdate->posY),
			IntToFloat(p_monsterUpdate->posZ) };
		XMFLOAT3 look = { IntToFloat(p_monsterUpdate->lookX),
			IntToFloat(p_monsterUpdate->lookY),
			IntToFloat(p_monsterUpdate->lookZ) };
		int id = p_monsterUpdate->id;

		reinterpret_cast<CEnemy*>(m_ObjectLayers[(int)OBJECT_LAYER::Enemy][id])->SetAnimationSet(p_monsterUpdate->state);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy][id]->SetPosition(pos);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy][id]->LookAt(pos, Vector3::Multifly(look, 15000.0f), { 0,1,0 });

		m_ObjectLayers[(int)OBJECT_LAYER::Enemy][id]->LookAtDirection(Vector3::Add(XMFLOAT3(0, 0, 0), look, 15000.0f), nullptr);
	}
	break;
	case PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE:
		P_S2C_UPDATE_SYNC p_syncUpdate;
		memcpy(&p_syncUpdate, p_buf, p_buf[0]);
		m_CurrentPlayerNum = p_syncUpdate.playerNum;
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			m_Players[i]->SetDrawable(p_syncUpdate.existance[i]);
			if (m_Players[i]->IsDrawable() == false) continue;

			XMFLOAT3 pos = { IntToFloat(p_syncUpdate.posX[i]), IntToFloat(p_syncUpdate.posY[i]), IntToFloat(p_syncUpdate.posZ[i]) };
			XMFLOAT3 look = { IntToFloat(p_syncUpdate.lookX[i]), IntToFloat(p_syncUpdate.lookY[i]), IntToFloat(p_syncUpdate.lookZ[i]) };

			m_Players[i]->SetHP(p_syncUpdate.hp[i]);
			m_Players[i]->SetPosition(pos);
			m_Players[i]->UpdateCamera();
			m_Players[i]->LookAt(pos, Vector3::Multifly(look, 15000.0f), { 0,1,0 });
			m_Players[i]->SetVelocity(Vector3::Add(XMFLOAT3(0, 0, 0),
				look, -PLAYER_RUN_SPEED));
			/*if (m_Player == m_Players[i]) {
				if (p_syncUpdate.states[i] == AnimationType::DAMAGED) {
					if (m_Players[i]->GetAnimationSet() != AnimationType::DAMAGED) {
						m_CurrentCamera->SetShake(true, 0.5f, 15);
					}
				}
				else if (p_syncUpdate.states[i] == AnimationType::ATTACK) {
					if (m_Players[i]->GetAnimationSet() != AnimationType::ATTACK) {
						m_SoundManager->PlayEffect(Sound_Name::EFFECT_ARROW_SHOT);
					}
				}
			}*/
			switch (p_syncUpdate.states[i]) {
			case IDLE:
			case SWORD_IDLE:
			case BOW_IDLE:
				m_Players[i]->SetAnimationSet((int)m_Players[i]->IDLE);
				break;
			case SWORD_RUN:
			case BOW_RUN:
				m_Players[i]->SetAnimationSet((int)m_Players[i]->RUN);
				break;
			case SWORD_ATK:
			case BOW_ATK:
				m_Players[i]->SetAnimationSet((int)m_Players[i]->ATK);
				break;
			case SWORD_DEATH:
			case BOW_DEATH:
				m_Players[i]->SetAnimationSet((int)m_Players[i]->DEATH);
				break;

			case WALK: break;
			case DAMAGED: break;
			case SWORD_GET:break;
			case BOW_GET:break;
			default:
				break;
			}
		}

		CFramework::GetInstance().SetFrameDirtyFlag(true);
		break;
	case PACKET_PROTOCOL::S2C_INGAME_DOOR_EVENT:
		cout << "Packet::DoorEvent[ServerToClient]\n";
		break;
	case PACKET_PROTOCOL::S2C_INGAME_PUZZLE_EVENT:
		break;
	case PACKET_PROTOCOL::S2C_INGAME_END:
		cout << "Packet::GameEnd[ServerToClient]\n";
		break;
	default:
		cout << "Unknown Packet Type from server" << " Packet Type [" << +p_buf[1] << "]" << endl;
		while (true) {
			// ����
		}
		break;
	}
}

void CGameScene::LoginToServer()
{
	P_C2S_LOGIN p_login;
	p_login.size = sizeof(p_login);
	p_login.type = PACKET_PROTOCOL::C2S_LOGIN;
	p_login.roomIndex = -1;
	strcpy_s(p_login.name, CFramework::GetInstance().GetPlayerName().c_str());

	SendPacket(&p_login);
}

void CGameScene::LogoutToServer()
{
}

void CGameScene::ProcessInput()
{
	if (false == m_IsFocusOn) {
		return;
	}
	if (m_CurrentCamera->IsOnShake()) {
		// �ǰ� ������ �� ��� ����� ����
		return;
	}
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
		if (keyInput.KEY_J) {
			p_keyboard.keyInput = VK_J;
			processKey = true;
		}
		if (keyInput.KEY_U) {
			p_keyboard.keyInput = VK_U;
			processKey = true;
			//for (int i = 0; i < 5; ++i) {
			//	CDoorWall* p = reinterpret_cast<CDoorWall*>(m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][m_DoorIdx + i]);
			//	p->OpenDoor();
			//}
		}
		if (keyInput.KEY_I) {
			p_keyboard.keyInput = VK_I;
			processKey = true;
			//for (int i = 0; i < 5; ++i) {
			//	CDoorWall* p = reinterpret_cast<CDoorWall*>(m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][m_DoorIdx + i]);
			//	p->CloserDoor();
			//}
		}

		if (keyInput.KEY_3)
		{
			m_isPlayerSelected = true;
			m_CurrentCamera = m_Cameras[0];
			m_CurrentCamera = m_PlayerCameras[0];
		}
		if (keyInput.KEY_4)
		{
			m_isPlayerSelected = false;
			m_CurrentCamera = m_Cameras[2];
		}
		if (keyInput.KEY_O)
		{
			gbBoundaryOn = true;
		}
		if (keyInput.KEY_P)
		{
			gbBoundaryOn = false;
		}
		if (keyInput.KEY_K)
		{
			gbWireframeOn = true;
		}
		if (keyInput.KEY_L)
		{
			gbWireframeOn = false;
		}
		if (processKey == false) return;
		int retVal = 0;
		SendPacket(&p_keyboard);
		return;
	}

	if (m_CurrentCamera == nullptr) return;

	float cameraSpeed = m_CurrentCamera->GetSpeed();
	XMFLOAT3 velocity = m_Player->GetVelocity();

	XMFLOAT3 shift = XMFLOAT3(0, 0, 0);
	float distance = PLAYER_RUN_SPEED;

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

	if (keyInput.KEY_1)
	{
		m_Player->SetWeapon(PlayerWeaponType::Sword); 
	}
	if (keyInput.KEY_2)
	{
		m_Player->SetWeapon(PlayerWeaponType::Bow); 
	}
	if (keyInput.KEY_3)
	{
		m_Player->SetWeapon(PlayerWeaponType::Staff);
		m_isPlayerSelected = true; 
		m_CurrentCamera = m_PlayerCameras[0];
	}
	if (keyInput.KEY_4)
	{
		m_isPlayerSelected = false;
		m_CurrentCamera = m_Cameras[2];
	}
	if (keyInput.KEY_5)
	{
		m_isPlayerSelected = false; 
	}
	 
	if (keyInput.KEY_SPACE)
	{
		DisplayVector3(m_Player->GetPosition());
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
		m_Player->SetPosition({ 1622 * MAP_SCALE_SIZE, 0, 10772 * MAP_SCALE_SIZE });
		m_Player->FixPositionByTerrain(m_Terrain);
	}
	if (keyInput.KEY_F2)
	{
		m_Player->SetPosition({ 5500 * MAP_SCALE_SIZE,  -1000, 18000 * MAP_SCALE_SIZE });
		m_Player->FixPositionByTerrain(m_Terrain);
	}
	if (keyInput.KEY_F3)
	{
		m_Player->SetPosition({ 11838.8 * MAP_SCALE_SIZE,  -1000, 10428.2 * MAP_SCALE_SIZE });
		m_Player->FixPositionByTerrain(m_Terrain);
	}
	if (keyInput.KEY_F4)
	{
		m_Player->SetPosition({ 17000 * MAP_SCALE_SIZE,  -6000, 5500 * MAP_SCALE_SIZE });
		m_Player->FixPositionByTerrain(m_Terrain);
	}
	if (keyInput.KEY_F5)
	{
		m_Player->SetPosition({ 16749.9 * MAP_SCALE_SIZE,  -6000, 8500.78 * MAP_SCALE_SIZE });
		m_Player->FixPositionByTerrain(m_Terrain);
	}
	if (keyInput.KEY_F6)
	{
		m_Player->SetPosition({ 16958.4 * MAP_SCALE_SIZE,  -6000, 14861.1 * MAP_SCALE_SIZE });
		m_Player->FixPositionByTerrain(m_Terrain);
	}
	if (keyInput.KEY_F7)
	{
		DeleteEnemy(m_Mummy[0]);
	}
	if (keyInput.KEY_F8)
	{
		DeleteEnemy(m_Mummy[1]);
	}
	if (keyInput.KEY_F9)
	{
		DeleteEnemy(m_Mummy[2]);
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
	if (keyInput.KEY_R)
	{
		if (m_Npc_Event == true)
		{
			m_Interaction = true;
		}
		else if (m_Npc_Event == false)
		{
			m_Interaction = false;
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
	if (keyInput.KEY_K)
	{
		gbWireframeOn = true;
	}
	if (keyInput.KEY_L)
	{
		gbWireframeOn = false;
	}

	m_CurrentCamera->UpdateViewMatrix();
}

void CGameScene::ProcessWindowKeyboard(WPARAM wParam, bool isKeyUp)
{
	if (isKeyUp == false)
	{
		if (wParam == VK_9) {
			if (m_Player->GetWeapon() == PlayerWeaponType::Sword) {
				m_Player->SetWeapon(PlayerWeaponType::Bow);
			}
			else if (m_Player->GetWeapon() == PlayerWeaponType::Bow) {
				m_Player->SetWeapon(PlayerWeaponType::Staff);
			}
			else if (m_Player->GetWeapon() == PlayerWeaponType::Staff) {
				m_Player->SetWeapon(PlayerWeaponType::Sword);
			}
			else {
				cout << "...?" << endl;
			}
		}
		if (wParam == VK_J) {
			if (m_Player->IsCanAttack()) {
				switch (m_Player->GetWeapon())
				{
				case PlayerWeaponType::Sword:
					m_Player->Attack(0);
					break;
				case PlayerWeaponType::Bow:
					m_Player->Attack(0);
					m_Player->pullString = true;
					break;
				case PlayerWeaponType::Staff:
					m_Player->Attack(0);
					ActiveSkill(OBJECT_LAYER::FireBall, m_Player);
					break;
				}
			}
			else {
				switch (m_Player->GetWeapon())
				{
				case PlayerWeaponType::Sword:
					if (m_Player->GetAttackWaitTime() < 0.5f) {
						cout << m_Player->GetAttackWaitTime() << endl;
						m_Player->SetSwordAttackKeyDown(true);
					}
					break;
				}
			}
		}
		if (wParam == VK_M) {
			if (m_Player->IsCanAttack()) {
				m_Player->Attack(1);
				cout << "SKILLLLL~~" << endl;
			}
		}
	}
	else
	{
		if (wParam == VK_J) {
			switch (m_Player->GetWeapon())
			{
			case PlayerWeaponType::Sword:
				m_SoundManager->PlayEffect(Sound_Name::EFFECT_Sword);
				break;
			case PlayerWeaponType::Bow:
				if (m_Player->ShotAble()) {
					ShotPlayerArrow();
					m_SoundManager->PlayEffect(Sound_Name::EFFECT_ARROW_SHOT);
				}
				else {
					m_Player->IncreaseAttackWaitingTime(0);
					m_Player->SetAnimationSet(IDLE);
				}
				m_Player->ResetBow();
				break;
			case PlayerWeaponType::Staff:
				m_SoundManager->PlayEffect(Sound_Name::EFFECT_Fire_Ball);
				break;
			}
		}
	}
}

void CGameScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	if (false == m_IsFocusOn) {
		return;
	}
	if (m_CurrentCamera->IsOnShake()) {
		// �ǰ� ������ �� ��� ����� ����
		return;
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
	SetCapture(CFramework::GetInstance().GetHWND());
}

void CGameScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	if (false == m_IsFocusOn) {
		return;
	}

	if (m_CurrentCamera->IsOnShake()) {
		// �ǰ� ������ �� ��� ����� ����
		return;
	}
	ReleaseCapture();
}

void CGameScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (false == m_IsFocusOn) {
		return;
	}
	if (m_CurrentCamera->IsOnShake()) {
		// �ǰ� ������ �� ��� ����� ����
		return;
	}
	//
	// �̱��÷���, ��Ƽ �÷��� �ڵ� �и� �۾� ���� �ʿ�
	//
	if (CFramework::GetInstance().IsOnConntected())
	{
		if ((btnState & MK_LBUTTON) != 0)
		{
			float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
			float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

			m_prevMouseInputType = MOUSE_INPUT_TYPE::M_LMOVE;
			m_MouseInputTypes.emplace_back(MOUSE_INPUT_TYPE::M_LMOVE);
			m_MousePositions.emplace_back(POINTF{ dx, dy });
		}
		else if ((btnState & MK_RBUTTON) != 0)
		{
			float dx = static_cast<float>(x - m_LastMousePos.x);
			float dy = static_cast<float>(y - m_LastMousePos.y);
			m_prevMouseInputType = MOUSE_INPUT_TYPE::M_RMOVE;

			m_MouseInputTypes.emplace_back(MOUSE_INPUT_TYPE::M_RMOVE);
			m_MousePositions.emplace_back(POINTF{ dx, dy });
		}
	}
	else {
		if ((btnState & MK_LBUTTON) != 0)
		{
			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
			float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

			if (m_isPlayerSelected)
			{
				//m_CurrentCamera->RotateAroundTarget(XMFLOAT3(1, 0, 0), dy * 30);
				if (m_Player->pullString) {
					m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 150);
				}
				else {
					m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 80);
				}

				if (m_Player->IsMoving() || m_Player->pullString)
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
			float dx = static_cast<float>(x - m_LastMousePos.x);
			float dy = static_cast<float>(y - m_LastMousePos.y);

			m_CurrentCamera->MoveOffset(XMFLOAT3(0, 0, dy * 0.025f));
		}
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void CGameScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < (int)OBJECT_LAYER::Count; ++i) {
		for (auto pObject : m_ObjectLayers[i])
		{
			pObject->ReleaseUploadBuffers();
		}
	}
}

ID3D12RootSignature* CGameScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// Ground
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = m_Textures.size() + 4;
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

void CGameScene::BuildBridges(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	CBridge* pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ 8000.0f * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ (10000.0f - 680) * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ (9000.0f - 340) * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ (11000.0f - 680 - 340) * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ 8000.0f * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ (10000.0f - 680) * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ (9000.0f - 340) * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);
	pBridge->SetShader(pShader);
	pBridge->SetObjectName(OBJ_NAME::Bridge);
	pBridge->RotateAll({ 0,1,0 }, 90.0f);
	pBridge->SetPositionPlus({ (11000.0f - 680 - 340) * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);
}

void CGameScene::BuildDoorWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	CDoorWall* pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 4000 * MAP_SCALE_SIZE, 1000, 500, pShader);
	pDoorWall->SetPosition({ 0,0, 7500 * MAP_SCALE_SIZE });
	pDoorWall->SetExistingSector(SECTOR_POSITION::SECTOR_1);
	m_DoorIdx = m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].size();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 3300 * MAP_SCALE_SIZE, 1000, 500, pShader);
	pDoorWall->SetPosition({ 10300 * MAP_SCALE_SIZE, -2000, 7500 * MAP_SCALE_SIZE });
	pDoorWall->SetTextureIndexes(0x02);
	pDoorWall->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 4000 * MAP_SCALE_SIZE, 2500, 500, true, pShader);
	pDoorWall->SetTextureIndexes(0x04);
	pDoorWall->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	//pDoorWall->RotateAll({ 0,1,0 }, 90);
	pDoorWall->SetPosition({ 13500 * MAP_SCALE_SIZE, -3500, 0 });
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 5500 * MAP_SCALE_SIZE, 2000, 500, pShader);
	pDoorWall->SetPosition({ 14000 * MAP_SCALE_SIZE,-4500, 8000 * MAP_SCALE_SIZE });
	pDoorWall->SetTextureIndexes(0x08);
	pDoorWall->SetExistingSector(SECTOR_POSITION::SECTOR_5);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 5800 * MAP_SCALE_SIZE, 4500, 800 * MAP_SCALE_SIZE, pShader);
	pDoorWall->SetPosition({ 14000 * MAP_SCALE_SIZE, -7050, 13650 * MAP_SCALE_SIZE });
	pDoorWall->SetTextureIndexes(0x08);
	pDoorWall->SetExistingSector(SECTOR_POSITION::SECTOR_5);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pDoorWall);

	CWall* pWall = new CWall(pd3dDevice, pd3dCommandList, 1500 * MAP_SCALE_SIZE, 2500, 500);
	pWall->Rotate({ 0,1,0 }, 90);
	pWall->SetPosition({ 13750 * MAP_SCALE_SIZE, -3500 + 1250, 4750 * MAP_SCALE_SIZE });
	pWall->SetTextureIndex(0x04);
	pWall->SetShader(pShader);
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1500, 2500, 500, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	pWall->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pWall);

	pWall = new CWall(pd3dDevice, pd3dCommandList, 1500 * MAP_SCALE_SIZE, 2500, 500);
	pWall->Rotate({ 0,1,0 }, 90);
	pWall->SetPosition({ 13750 * MAP_SCALE_SIZE, -3500 + 1250, 6250 * MAP_SCALE_SIZE });
	pWall->SetTextureIndex(0x04);
	pWall->SetShader(pShader);
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1500, 2500, 500, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	pWall->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pWall);
}

void CGameScene::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	pUI = new UI(pd3dDevice, pd3dCommandList, 0.1f, 0.1f, 0.0f, false);
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

	m_HelpBoard = new UI(pd3dDevice, pd3dCommandList, 1.6f, 1.6f, 0.0f, false);
	m_HelpBoard->SetDrawable(false);
	m_HelpBoard->SetPosition({ 0.0f, 0.0, 0.90 });        // RoomBoard
	m_HelpBoard->SetTextureIndex(0x08);
	m_HelpBoard->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
	m_UIs.push_back(m_HelpBoard);

}

void CGameScene::BuildPuzzles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < ChessPuzzleSize; i++)
	{
		for (int j = 0; j < ChessPuzzleSize; j++)
		{
			m_ChessPlate[i][j].x = 16855 + 355 * i;
			m_ChessPlate[i][j].z = 14417 + 450 * j;
			m_ChessPlate[i][j].y = -1750.0f;
		}
	}
	CPlate* pPuzzlePlate = new CPlate(pd3dDevice, pd3dCommandList, CShaderHandler::GetInstance().GetData("Puzzle"));

	pPuzzlePlate->SetPosition({ 11130.0f * MAP_SCALE_SIZE, -2000.0f,(2000.0f + 8000.0f) * MAP_SCALE_SIZE });

	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pPuzzlePlate);

	CGameObject* pObject = new CPuzzle(pd3dDevice, pd3dCommandList, PuzzleType::Holding, CShaderHandler::GetInstance().GetData("Puzzle"));
	CGameObject* pObject2 = new CPuzzle(pd3dDevice, pd3dCommandList, PuzzleType::Holding, CShaderHandler::GetInstance().GetData("Puzzle"));

	// ������ ü����
	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::King]);
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[1][0]);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->SetTextureIndex(0x4000);
	pObject->Scale(300, 300, 300);
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Knight]);
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[0][2]);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->SetTextureIndex(0x8000);
	pObject->Scale(300, 300, 300);
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Pawn]);
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[3][1]);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->SetTextureIndex(0x10000);
	pObject->Scale(300, 300, 300);
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Rook]);
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[5][2]);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->SetTextureIndex(0x20000);
	pObject->Scale(300, 300, 300);
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);


	// �÷��̾ �����ϼ� �ִ� ����
	m_Chess[King] = new CGameObject();
	m_Chess[King]->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::King]);
	m_Chess[King]->Rotate({ 1,0,0 }, -90);
	m_Chess[King]->SetPosition(m_ChessPlate[0][6]);
	m_Chess[King]->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	m_Chess[King]->SetChess(Chess_Type::King);
	m_Chess[King]->SetTextureIndex(0x400);
	m_Chess[King]->Scale(300, 300, 300);
	m_Chess[King]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	m_Chess[King]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[King]->SetExistingSector(SECTOR_POSITION::SECTOR_3);

	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[King]);

	m_Chess[Knight] = new CGameObject();
	m_Chess[Knight]->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Knight]);
	m_Chess[Knight]->Rotate({ 1,0,0 }, -90);
	m_Chess[Knight]->SetPosition(m_ChessPlate[6][3]);
	m_Chess[Knight]->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	m_Chess[Knight]->SetChess(Chess_Type::Knight);
	m_Chess[Knight]->SetTextureIndex(0x800);
	m_Chess[Knight]->Scale(300, 300, 300);
	m_Chess[Knight]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	m_Chess[Knight]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[Knight]->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[Knight]);

	m_Chess[Pawn] = new CGameObject();
	m_Chess[Pawn]->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Pawn]);
	m_Chess[Pawn]->Rotate({ 1,0,0 }, -90);
	m_Chess[Pawn]->SetPosition(m_ChessPlate[3][5]);
	m_Chess[Pawn]->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	m_Chess[Pawn]->SetChess(Chess_Type::Pawn);
	m_Chess[Pawn]->SetTextureIndex(0x1000);
	m_Chess[Pawn]->Scale(300, 300, 300);
	m_Chess[Pawn]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	m_Chess[Pawn]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[Pawn]->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[Pawn]);

	m_Chess[Rook] = new CGameObject();
	m_Chess[Rook]->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Rook]);
	m_Chess[Rook]->Rotate({ 1,0,0 }, -90);
	m_Chess[Rook]->SetPosition(m_ChessPlate[6][6]);
	m_Chess[Rook]->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	m_Chess[Rook]->SetChess(Chess_Type::Rook);
	m_Chess[Rook]->SetTextureIndex(0x2000);
	m_Chess[Rook]->Scale(300, 300, 300);
	m_Chess[Rook]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.5, 0.7, 1.5, XMFLOAT3{ 0,0,0.7 });
	m_Chess[Rook]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[Rook]->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[Rook]); 
}

void CGameScene::BuildSigns(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ù��° ���� ǥ����
	CSign* pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::Scroll,
		false, true, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 2700 * MAP_SCALE_SIZE, 200,7000 * MAP_SCALE_SIZE });
	pSign->SetExistingSector(SECTOR_POSITION::SECTOR_1);
	//m_Objects.push_back(pSign); 
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pSign);

	// ���� �� ǥ����
	pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::NumPuzzle,
		false, false, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 11200.0f * MAP_SCALE_SIZE, -1800.0f, 8200.0f * MAP_SCALE_SIZE });
	pSign->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	//m_Objects.push_back(pSign);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pSign);

	// �޵λ� �� ǥ����
	pSign = new CSign(pd3dDevice, pd3dCommandList, SignBoardInfos::Medusa,
		true, true, CShaderHandler::GetInstance().GetData("Sign"));
	pSign->SetPosition({ 13000.0f * MAP_SCALE_SIZE, -3250.0f, 1300.0f * MAP_SCALE_SIZE });
	pSign->RotateAll({ 0,1,0 }, 90.0f);
	pSign->SetExistingSector(SECTOR_POSITION::SECTOR_4);

	//m_Objects.push_back(pSign);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pSign);
}

void CGameScene::BuildEnemys(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	{
		XMFLOAT3 scale = { 120.0f, 120.0f, 120.0f };
		m_Boss = new CBoss(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

		//x : 25592.2 y : -6962.67 z : 25934.3
		//z : 21800.0 14533
		//z : 29700.0 19800
		//x : 29700.0 19800
		//x : 21800.0 14533
		m_Boss->SetPosition({ 17166 * MAP_SCALE_SIZE, -6070, 17166 * MAP_SCALE_SIZE });
		//m_Boss->SetPosition({ 1500 * MAP_SCALE_SIZE, -6070, 1500 * MAP_SCALE_SIZE });
		m_Boss->FixPositionByTerrain(m_Terrain);
		m_Boss->Scale(120, 120, 120);
		m_Boss->Rotate({ 0,1,0 }, 180);
		m_Boss->ConnectPlayer(m_Players, m_CurrentPlayerNum);

		XMFLOAT3 centerPos = m_Boss->GetPosition();
		XMFLOAT3 scopeSize = { 4100 * 2, 0, 4100 * 2 };
		m_Boss->SetActivityScope({ scopeSize.x, 0, scopeSize.z }, { centerPos });
		m_Boss->SetSightBoundingBox({ scopeSize.x / scale.x, 15, scopeSize.z / scale.z });
		m_Boss->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, scopeSize.x / scale.x, 15, scopeSize.z / scale.z, XMFLOAT3{ 0, 0.0f,0 });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(m_Boss);

	}

	CGameObjectVer2* pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, "resources/FbxExported/BasicSkeleton.bin", NULL, true);

	CGameObjectVer2* pMummyModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, "resources/FbxExported/Mummy.bin", NULL, true);

	CEnemy* pEnemy;

	
		// Monster Area4 �ӽ� ����� ��ġ // ���Ŀ� ���� ����� �� ������ ����
	{
		scale = { 600.0f,600.0f,600.0f };
		CGameObjectVer2* pMummyModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Mummy.bin", NULL, true);

		m_Mummy[0] = new CMummy();
		m_Mummy[0]->Scale(scale.x, scale.y, scale.z, true);
		m_Mummy[0]->SetChild(pMummyModel, true);
		m_Mummy[0]->SetShadertoAll();
		//m_Mummy[0]->SetTextureInedxToAll(0x100);
		//m_Mummy[0]->SetTextureIndex(0x100);
		//m_Mummy[0]->SetTextureIndexFindByName();
		
		m_Mummy[0]->SetPosition({ 18900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.0f, 6250.0f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[0]->SetActivityScope({ 1200.0f, 0, 250.0f }, { 18900.f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[0]->SetEnemyAttackType(EnemyAttackType::Mummy1);
		m_Mummy[0]->SetMummyDie(0);
		m_Mummy[0]->ConnectPlayer(m_Players, m_CurrentPlayerNum);

		m_Mummy[0]->SetSightBoundingBox({ 5020.0f * 0.75f / scale.x, 3, 2250 * 0.75f / scale.z });
		m_Mummy[0]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1020.0f * 0.25f / scale.x, 0.5, 225.0f  / scale.z, XMFLOAT3{ 0, 0.25f,0 });
		m_Mummy[0]->AddColider(new ColliderBox(XMFLOAT3{ 0, 0.25f,0 }, XMFLOAT3(1020.0f * 0.25f / scale.x, 0.5f, 225.0f / scale.z)));

		m_ObjectLayers[(int)OBJECT_LAYER::Mummy].push_back(reinterpret_cast<CGameObject*>(std::move(m_Mummy[0])));
		m_Mummy[0]->AddFriends(m_Mummy[0]);

		scale = { 600.0f,600.0f,600.0f };
		pMummyModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Mummy.bin", NULL, true);

		m_Mummy[1] = new CMummy();
		m_Mummy[1]->Scale(scale.x, scale.y, scale.z);
		m_Mummy[1]->SetChild(pMummyModel, true);
		m_Mummy[1]->SetShadertoAll();
		m_Mummy[1]->SetTextureInedxToAll(0x100);

		m_Mummy[1]->SetPosition({ 16900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(16900.0f, 6250), 6250 * MAP_SCALE_SIZE });
		m_Mummy[1]->SetActivityScope({ 1200.0f, 0, 250.0f }, { 16900.f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(16900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[1]->SetEnemyAttackType(EnemyAttackType::Mummy2);
		m_Mummy[1]->SetMummyDie(0);
		m_Mummy[1]->ConnectPlayer(m_Players, m_CurrentPlayerNum);

		m_Mummy[1]->SetSightBoundingBox({ 5020.0f * 0.75f / scale.x, 3, 2250 * 0.75f / scale.z });
		m_Mummy[1]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1020.0f * 0.25f / scale.x, 0.5, 225.0f / scale.z, XMFLOAT3{ 0, 0.25f,0 });
		m_Mummy[1]->AddColider(new ColliderBox(XMFLOAT3{ 0, 0.25f,0 }, XMFLOAT3(1020.0f * 0.25f / scale.x, 0.5f, 225.0f / scale.z)));

		m_ObjectLayers[(int)OBJECT_LAYER::Mummy].push_back(reinterpret_cast<CGameObject*>(std::move(m_Mummy[1])));
		m_Mummy[1]->AddFriends(m_Mummy[1]);

		scale = { 600.0f,600.0f,600.0f };

		pMummyModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Mummy.bin", NULL, true);

		m_Mummy[2] = new CMummy();
		m_Mummy[2]->Scale(scale.x, scale.y, scale.z);
		m_Mummy[2]->SetChild(pMummyModel, true);
		m_Mummy[2]->SetShadertoAll();
		m_Mummy[2]->SetTextureInedxToAll(0x100);
		
		m_Mummy[2]->SetPosition({ 14900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(14900.0f, 6250), 6250 * MAP_SCALE_SIZE });
		m_Mummy[2]->SetActivityScope({ 1200.0f, 0, 250.0f }, { 14900.f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(14900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[2]->SetEnemyAttackType(EnemyAttackType::Mummy3);
		m_Mummy[2]->SetMummyDie(0);
		m_Mummy[2]->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		m_Mummy[2]->SetSightBoundingBox({ 5020.0f * 0.75f / scale.x, 3, 2250 * 0.75f / scale.z });

		m_Mummy[2]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1020.0f * 0.25f / scale.x, 0.5, 225.0f / scale.z, XMFLOAT3{ 0, 0.25f,0 });
		m_Mummy[2]->AddColider(new ColliderBox(XMFLOAT3{ 0, 0.25f,0 }, XMFLOAT3(1020.0f * 0.25f / scale.x, 0.5f, 225.0f / scale.z)));

		m_ObjectLayers[(int)OBJECT_LAYER::Mummy].push_back(reinterpret_cast<CGameObject*>(std::move(m_Mummy[2])));
		m_Mummy[2]->AddFriends(m_Mummy[2]);
	}

	scale = { 300.0f,300.0f,300.0f };
	{	// Monster Area1 
		XMFLOAT3 boundbox = { 0.3f, 0.75f, 0.1f };

		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 2005.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, boundbox));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z, XMFLOAT3{ 0, 0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
  
		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/BasicSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 2005.0f * MAP_SCALE_SIZE + 1000.0f, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, boundbox));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/BasicSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x40);
		pEnemy->SetPosition({ 2005.0f * MAP_SCALE_SIZE + 2000.0f, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, boundbox));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/BasicSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x40);
		pEnemy->SetPosition({ 2005.0f * MAP_SCALE_SIZE + 3000.0f, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, boundbox));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}
	//return;
	{	// Monster Area1-2
		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/BasicSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1600 * 0.75f / scale.x , 10, 2950 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1600 * 0.75f / scale.x, 3, 2950 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/MaceSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1600 * 0.75f / scale.x , 10, 2950 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1600 * 0.75f / scale.x, 3, 2950 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/StrongSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1600 * 0.75f / scale.x , 10, 2950 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1600 * 0.75f / scale.x, 3, 2950 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/AxeSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(7800.0f,  11450.0f),  11450.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1600 * 0.75f / scale.x , 10, 2950 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1600 * 0.75f / scale.x, 3, 2950 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}
	{// Monster Area2-1
		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/AxeSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 12100.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1300 * 0.75f / scale.x, 3, 1450 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1300 * 0.75f / scale.x, 3, 1450 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/MaceSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 12100.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1300 * 0.75f / scale.x, 3, 1450 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1300 * 0.75f / scale.x, 3, 1450 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/MaceSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 12100.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(12100.0f, 17950.0f), 17950.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1300 * 0.75f / scale.x, 3, 1450 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1300 * 0.75f / scale.x, 3, 1450 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}
	{// Monster Area2-2
		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/StrongSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1400 * 0.75f / scale.x , 10, 1200 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1400 * 0.75f / scale.x, 3, 1200 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/StrongSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1400 * 0.75f / scale.x , 10, 1200 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1400 * 0.75f / scale.x, 3, 1200 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/StrongSkeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 13300.0f), 13300.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1400 * 0.75f / scale.x , 10, 1200 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1400 * 0.75f / scale.x, 3, 1200 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}

	{// Monster Area3
		scale = { 600.0f,600.0f,600.0f };
		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Skeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1200 * 0.75f / scale.x, 3, 2750 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_4);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Skeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1200 * 0.75f / scale.x, 3, 2750 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_4);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Skeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1200 * 0.75f / scale.x, 3, 2750 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_4);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		pSkeletonModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Skeleton.bin", NULL, true);
		pEnemy = new CMeleeEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pSkeletonModel, true);
		pEnemy->SetShadertoAll();
		pEnemy->SetTextureInedxToAll(0x20);
		pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(11900.0f, 3250.0f), 3250.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.5f, 0.8f, XMFLOAT3{ 0, 0.0f, 0 });
		pEnemy->AddColider(new ColliderBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
		pEnemy->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1200 * 0.75f / scale.x, 3, 2750 * 0.75f / scale.z, XMFLOAT3{ 0,0.0f,0 });
		pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_4);
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
	}
}

void CGameScene::BuildMirror(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < 1; i++)
	{
		m_Mirror[i] = new CGameObject();

		CPlaneMeshTextured* pMirrorMesh = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList, 6000.0f * MAP_SCALE_SIZE, 2600.0f, 1.0f);

		m_MirrorCamera->SetPosition({ 17000 * MAP_SCALE_SIZE, -3000, 210 * MAP_SCALE_SIZE });

		m_Mirror[i]->SetMesh(pMirrorMesh);
		m_Mirror[i]->SetShader(CShaderHandler::GetInstance().GetData("Mirror")); 
		m_Mirror[i]->SetExistingSector(SECTOR_POSITION::SECTOR_4);
		if (i == 1 || i == 2)
		{
			m_Mirror[i]->Rotate({ 0,1,0 }, 90);
		}
		if (i == 0)
		{
			m_Mirror[i]->SetPosition({ float(17000 - (2900 * i)) * MAP_SCALE_SIZE , -2300, 200 * MAP_SCALE_SIZE });
		}
		if (i == 1)
		{
			m_Mirror[i]->SetPosition({ float(17000 - (2900 * i)) * MAP_SCALE_SIZE, -2300, 3200 * MAP_SCALE_SIZE });
		}
		if (i == 2)
		{
			m_Mirror[i]->SetPosition({ float(17000 + 2900) * MAP_SCALE_SIZE, -2300, 3200 * MAP_SCALE_SIZE });
		}
		m_Mirror[i]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 6000 * MAP_SCALE_SIZE, 2600, 10.0, XMFLOAT3{ 0,0,0 });
		m_Mirror[i]->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 6000.0f * 0.5f* MAP_SCALE_SIZE, 2600.0f * 0.5f, 10.0f * 0.5f }));

		m_Mirror[i]->SetTextureIndex(0x01);

		m_ObjectLayers[(int)OBJECT_LAYER::MirrorBox].push_back(m_Mirror[i]);
	}
}

void CGameScene::BuildNpc(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObjectVer2* pNpcModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, "resources/FbxExported/Elf_Mesh.bin", NULL, true);

	pNpcModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, "resources/FbxExported/Elf_Mesh.bin", NULL, true);

	m_Npc = new CNpc();
	m_Npc->Scale(scale.x/1.5f,scale.y/1.5f,scale.z/1.5f);
	m_Npc->Rotate(XMFLOAT3(0, 1, 0), 180);
	m_Npc->SetChild(pNpcModel, true);
	m_Npc->SetShadertoAll();
	m_Npc->SetTextureInedxToAll(0x800);
	m_Npc->SetPosition({ 2305.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2305.0f, 4650.0f), 4650.0f * MAP_SCALE_SIZE });
	//pNpc->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
	m_Npc->ConnectPlayer(m_Players, m_CurrentPlayerNum);
	m_Npc->SetExistingSector(SECTOR_POSITION::SECTOR_1);
	m_ObjectLayers[(int)OBJECT_LAYER::Npc].push_back(reinterpret_cast<CGameObject*>(std::move(m_Npc)));
}

void CGameScene::BuildMinimapResource(ID3D12Device* pd3dDevice)
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

void CGameScene::BuildMirrorResource(ID3D12Device* pd3dDevice)
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

void CGameScene::BuildMapSector1(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
#pragma region Create Tree
	// �������� ���ϴ� ù��° �������� ���� ������
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
		pBillboardObject->SetPosition({ x_Tree * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 340.0f , z_Tree * MAP_SCALE_SIZE });

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
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pBillboardObject->SetPosition({ x_Tree * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 340.0f, z_Tree * MAP_SCALE_SIZE });

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
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 340.0f, z_Tree });

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
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 340.0f, z_Tree });

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
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 340.0f, z_Tree });

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
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 340.0f, z_Tree });

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
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 380.0f, z_Tree });

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
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 380.0f, z_Tree });

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(CShaderHandler::GetInstance().GetData("Billboard"));
		m_ObjectLayers[(int)OBJECT_LAYER::Billboard].push_back(pBillboardObject);
	}
#pragma endregion 

	CGameObject* pObject;

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::GreenTree]);
		pObject->SetPosition({ 1000.0f + i * 1000.0f, 100, 850 });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x200);
		pObject->Scale(100, 100, 100);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		//m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->Rotate({ 1,0,0 }, 90);
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->SetPosition({ (1000.0f + i * 1000.0f) * MAP_SCALE_SIZE, 100, 850 * MAP_SCALE_SIZE });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->Rotate({ 1,0,0 }, 90);
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->SetPosition({ (1200.0f + i * 1000.0f) * MAP_SCALE_SIZE, 0, 3150 * MAP_SCALE_SIZE });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ (500.0f + i * 2700.0f) * MAP_SCALE_SIZE, 0, 2150 * MAP_SCALE_SIZE });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 2600.0f * MAP_SCALE_SIZE, 0,(4650 + 500.0f * i) * MAP_SCALE_SIZE });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 5; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
		pObject->Rotate({ 0,0,1 }, 90);
		pObject->SetPosition({ 2600.0f * MAP_SCALE_SIZE, -200.0f, (4650 + 500.0f * i) * MAP_SCALE_SIZE });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);
		pObject->Scale(50, 50, 50);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
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

		pObject->SetPosition({ (x_bush + 300 * i) * MAP_SCALE_SIZE, 0, (z_bush + 700.0f * i) * MAP_SCALE_SIZE });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
		pObject->SetTextureIndex(0x01);

		pObject->Scale(50, 50, 50);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1]);
	pObject->SetPosition({ 2500.0f * MAP_SCALE_SIZE, 100, 250 * MAP_SCALE_SIZE });
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->SetTextureIndex(0x01);
	pObject->Scale(50, 50, 50);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
}
void CGameScene::BuildMapSector2(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject* pObject;

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryForestRock]);
		float x = 500.0f + i * 1000.0f;
		float z = 19500 - 500.0f * i;
		float y = m_Terrain->GetDetailHeight(x, z);
		pObject->SetPosition({ x * MAP_SCALE_SIZE, y, z * MAP_SCALE_SIZE });
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->SetTextureIndex(0x02);
		pObject->Scale(50, 50, 50);
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5 * 0.5f, 7 * 0.5f, 3 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 5, 7, 3, { 0,0,0 });
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		pObject->FixPositionByTerrain(m_Terrain);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	float x_Tree, z_Tree;

	for (int i = 0; i < 1; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01]);

		x_Tree = 900 + 6200 * i;
		z_Tree = 18800;
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pObject->Scale(1.0f, 1.0f, 1.0f);
		pObject->Rotate({ 0,1,0 }, 60 + 30 * i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) - 100.0f , z_Tree });
		pObject->SetTextureIndex(0x04);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 200, 1500, 150, { 0, 0, 100 });
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01]);

	x_Tree = 900 + 6400;
	z_Tree = 19300;
	x_Tree *= MAP_SCALE_SIZE;
	z_Tree *= MAP_SCALE_SIZE;
	pObject->Scale(1.0f, 1.0f, 1.0f);
	pObject->Rotate({ 0,1,0 }, 60 + 30);
	pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) - 100.0f , z_Tree });
	pObject->SetTextureIndex(0x04);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 200, 1500, 150, { 0, 0, 100 });
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01]);

		x_Tree = 200 + 3000 * i;
		z_Tree = 17000;
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pObject->Scale(1.0f, 1.0f, 1.0f);
		pObject->Rotate({ 0,1,0 }, 0 + 15 * i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) - 100.0f, z_Tree });
		pObject->SetTextureIndex(0x04);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("Tree"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 200, 1500, 150, { 0, 0, 100 });
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Stump]);

	x_Tree = 200;
	z_Tree = 18000;

	x_Tree *= MAP_SCALE_SIZE;
	z_Tree *= MAP_SCALE_SIZE;
	pObject->Scale(20.0f, 20.0f, 20.0f);
	pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree), z_Tree });
	pObject->SetTextureIndex(0x08);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureLeft"));
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(15 * 0.5f, 10 * 0.5f, 15 * 0.5f)));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 15, 10, 15, { 0,0,0 });
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	///////////////////////////////////////////////////////////////////////////////////

	pObject = new CGameObject();
	pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DeadTree_01]);

	x_Tree = 3700;
	z_Tree = 19000;
	x_Tree *= MAP_SCALE_SIZE;
	z_Tree *= MAP_SCALE_SIZE;
	pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 1000.0f,z_Tree });
	pObject->SetTextureIndex(0x10);
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
	pObject->AddColider(new ColliderBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1, 5, 1, { 1, -5,-2.5 });
	pObject->Scale(150.0f, 150.0f, 150.0f);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	for (int i = 0; i < 2; i++)
	{
		pObject = new CGameObject();
		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DeadTree_01]);

		x_Tree = 1500 + 5000 * i;
		z_Tree = 17500;
		x_Tree *= MAP_SCALE_SIZE;
		z_Tree *= MAP_SCALE_SIZE;
		pObject->Scale(150.0f + 50 * i, 150.0f + 50 * i, 150.0f + 50 * i);
		pObject->Rotate({ 0,1,0 }, 30 + 30 * i);
		pObject->SetPosition({ x_Tree , m_Terrain->GetDetailHeight(x_Tree,z_Tree) + 1000.0f + 400.0f * i, z_Tree });
		pObject->SetTextureIndex(0x10);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 1, 5, 1, { 1, -5,-2.5 });
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}
}

void CGameScene::BuildMapSector3(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
		x_Pos *= MAP_SCALE_SIZE;
		z_Pos *= MAP_SCALE_SIZE;
		pObject->Scale(0.5f, 0.5f, 0.5f);

		pObject->SetPosition({ x_Pos , m_Terrain->GetDetailHeight(x_Pos,z_Pos), z_Pos });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 600, 250, 600, { 0, 220, 0 });
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 1; i < 6; i++)
	{
		pObject = new CGameObject();

		pObject->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DesertRock]);

		x_Pos = 9700.0f;
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
			x_Pos = 9700.0f + 500 * i;
			z_Pos = 13900;
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 5)
		{
			x_Pos = 13000;
			z_Pos = 11700;
			pObject->Rotate({ 0,1,0 }, 135);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 4)
		{
			x_Pos = 13200;
			z_Pos = 15300;
		}
		else
		{
			x_Pos = 9700.0f + 500 * i;
			z_Pos = 13900;
		}

		x_Pos *= MAP_SCALE_SIZE;
		z_Pos *= MAP_SCALE_SIZE;
		pObject->Scale(0.5f, 0.5f, 0.5f);
		pObject->SetPosition({ x_Pos , m_Terrain->GetDetailHeight(x_Pos,z_Pos) , z_Pos });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 600, 250, 600, { 0, 220, 0 });
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	return;
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

		x_Pos *= MAP_SCALE_SIZE;
		z_Pos *= MAP_SCALE_SIZE;
		pObject->Scale(0.5f, 0.5f, 0.5f);
		pObject->SetPosition({ x_Pos , m_Terrain->GetDetailHeight(x_Pos,z_Pos) - 100.0f, z_Pos });
		pObject->SetTextureIndex(0x020);
		pObject->SetShader(CShaderHandler::GetInstance().GetData("FBXFeatureRight"));
		pObject->AddColider(new ColliderBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 600, 250, 600, { 0, 220, 0 });
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}
}
void CGameScene::BuildMapSector4(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameScene::BuildMapSector5(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameScene::LoadFbxMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*m_pfbxManager = FbxManager::Create();
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	m_pfbxManager->SetIOSettings(m_pfbxIOs);*/

	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Bush_1] = new CFixedMesh(pd3dDevice, pd3dCommandList, "bush-01");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryForestRock] = new CFixedMesh(pd3dDevice, pd3dCommandList, "rock");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Player] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Golem");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DryTree_01] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Dry_Tree");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Stump] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Stump_01");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DeadTree_01] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Dead_Tree");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::DesertRock] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Desert_Rock");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::GreenTree] = new CFixedMesh(pd3dDevice, pd3dCommandList, "GreenTree");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_01] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Enemy_t1");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Enemy_02] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Enemy_t2");

	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Boss] = new CFixedMesh(pd3dDevice, pd3dCommandList, "babymos");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Arrow] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Arrow");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::King] = new CFixedMesh(pd3dDevice, pd3dCommandList, "king");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Rook] = new CFixedMesh(pd3dDevice, pd3dCommandList, "rook");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Pawn] = new CFixedMesh(pd3dDevice, pd3dCommandList, "pawn");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Knight] = new CFixedMesh(pd3dDevice, pd3dCommandList, "knight");

	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Laser] = new CFixedMesh(pd3dDevice, pd3dCommandList, "Laser");
	m_LoadedFbxMesh[(int)FBX_MESH_TYPE::FireBall] = new CFixedMesh(pd3dDevice, pd3dCommandList, "FireBall");
}

void CGameScene::BuildShadowResource(ID3D12Device* pd3dDevice)
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

void CGameScene::CreateLightCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nHeight)
{
	XMFLOAT3 xmf3Look = XMFLOAT3(1.0f, -1.0f, 0.0f);

	XMFLOAT3 xmf3Right = XMFLOAT3(0.0f, 0.0f, -1.0f);

	XMFLOAT3 xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_pLightCamera = new CLightCamera();

	m_pLightCamera->SetOffset(XMFLOAT3(0.0f, 500.0f, -400.0f));
	m_pLightCamera->SetLens(0.25 * PI, nWidth, nHeight, 1.0f, lensize);
	m_pLightCamera->SetRight(xmf3Right);
	m_pLightCamera->SetUp(xmf3Up);
	m_pLightCamera->SetLook(xmf3Look);
	//m_pLightCamera->SetPosition(XMFLOAT3(10000.0f, 1300.0f, 0.0f));
	m_pLightCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pLightCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pLightCamera->SetScissorRect(0, 0, nWidth, nHeight);

	m_pLightCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CGameScene::BuildParticles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Particles = new CParticle();
	for (int i = 0; i < 10; ++i) {
		m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::ArrowParticle);
		//m_Particles->UseParticle(i, XMFLOAT3(500.0f * i, -500.0f, 3000.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::FireBallParticle);
	}
	// �Ȱ�
	m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::RadialParitcle);

	// ��
	m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 100000, PARTICLE_TYPE::RainParticle);


	m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::MummyLaserParticle, m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Laser]);

	m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::MummyLaserParticle2, m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Laser]);

	m_Particles->AddParticle(pd3dDevice, pd3dCommandList, 10000, PARTICLE_TYPE::MummyLaserParticle3, m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Laser]);


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
}
void CGameScene::BuildProjectiles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// FireBall
	for (int i = 0; i < 5; ++i) {
		CFireBall* pFireb = new CFireBall();
		pFireb->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::FireBall]);
		pFireb->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pFireb->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pFireb->SetTextureIndex(0x400);
		pFireb->SetShader(CShaderHandler::GetInstance().GetData("Object"));
		pFireb->Scale(40.0f, 40.0f, 40.0f);
		pFireb->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 0.0f, 0.0f, 0.0f, XMFLOAT3{ 0,0,0 });
		pFireb->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2.0f, 2.0f, 2.0f)));
		m_ObjectLayers[(int)OBJECT_LAYER::FireBall].push_back(pFireb);
	}
}

void CGameScene::BuildArrows(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
		m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow].push_back(pArrow);
	}

	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Arrow]);
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->SetTextureIndex(0x20);
		pArrow->SetShader(CShaderHandler::GetInstance().GetData("Object"));
		pArrow->Scale(100.0f, 100.0f, 50.0f);
		pArrow->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 0.5f, 0.5f, 15, XMFLOAT3{ 0,0,5 });
		pArrow->AddColider(new ColliderBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow].push_back(pArrow);
	}
}
void CGameScene::BuildMummyLaser(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Mummy[0] = new CMummy();
	for (int i = 0; i < 3; i++)
	{
		m_MummyLaser[i] = new CMummyLaser();
		m_MummyLaser[i]->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Laser]);
		m_MummyLaser[i]->SetTextureIndex(0x01);
		m_MummyLaser[i]->SetLaserType(Laser_TYPE::Laser1);
		m_MummyLaser[i]->SetShader(CShaderHandler::GetInstance().GetData("Laser"));
		m_MummyLaser[i]->Scale(100.0f, 100.0f, 1000.0f);
		m_MummyLaser[i]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.0f, 2.f, XMFLOAT3{ 0,0,1 });
		m_MummyLaser[i]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 1), XMFLOAT3(0.5f, 0.5f, 1.0f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser].push_back(m_MummyLaser[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		m_MummyLaser2[i] = new CMummyLaser();
		m_MummyLaser2[i]->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Laser]);
		m_MummyLaser2[i]->SetTextureIndex(0x02);
		m_MummyLaser2[i]->SetLaserType(Laser_TYPE::Laser2);
		m_MummyLaser2[i]->SetShader(CShaderHandler::GetInstance().GetData("Laser"));
		m_MummyLaser2[i]->Scale(100.0f, 100.0f, 1000.0f);
		m_MummyLaser2[i]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.0f, 2.f, XMFLOAT3{ 0,0,1 });
		m_MummyLaser2[i]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 1), XMFLOAT3(0.5f, 0.5f, 1.0f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser].push_back(m_MummyLaser2[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		m_MummyLaser3[i] = new CMummyLaser();
		m_MummyLaser3[i]->SetMesh(m_LoadedFbxMesh[(int)FBX_MESH_TYPE::Laser]);
		m_MummyLaser3[i]->SetTextureIndex(0x04);
		m_MummyLaser3[i]->SetLaserType(Laser_TYPE::Laser3);
		m_MummyLaser3[i]->SetShader(CShaderHandler::GetInstance().GetData("Laser"));
		m_MummyLaser3[i]->Scale(100.0f, 100.0f, 1000.0f);
		m_MummyLaser3[i]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 1.0f, 1.0f, 2.f, XMFLOAT3{ 0,0,1 });
		m_MummyLaser3[i]->AddColider(new ColliderBox(XMFLOAT3(0, 0, 1), XMFLOAT3(0.5f, 0.5f, 1.0f)));
		m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser].push_back(m_MummyLaser3[i]);
	}
}

void CGameScene::ShotMummyLaser(CMummy* pMummy, const XMFLOAT3& lookVector)
{
	if (pMummy->GetEnemyAttackType() == EnemyAttackType::Mummy1 && m_MummyExist[0] == true)
	{
		m_SoundManager->PlayEffect(Sound_Name::EFFECT_Laser);
		// �̶� 2���� ��������
		if (m_One_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 1.5f, scale.y * 1.5f, scale.z * 1.5f));
				m_Mummy[i]->SetActivityScope({ 2000.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
			}
			m_One_Mira_Die_Laser = false;
		}
		if (m_Two_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 3.f, scale.y * 3.f, scale.z * 3.f));
				m_Mummy[i]->SetActivityScope({ 2800.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });

			}
			m_Two_Mira_Die_Laser = false;
		}

		
		for (int i = 0; i < 3; i++)
		{
			m_MummyLaser[i]->SetUseable(false);

			XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Mummy[0]->GetPosition() }, { 0,200,0 });
			m_MummyLaser[i]->SetPosition(pos);
			m_MummyLaser[i]->SetLaser(i);
			m_MummyLaser[i]->SetLaserType(Laser_TYPE::Laser1);
			m_Mummy[0]->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));
			m_MummyLaser[i]->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));
			m_MummyLaser[i]->LookAtDirections(Vector3::Multifly(XMFLOAT3(m_Mummy[0]->GetLook().x, m_Mummy[0]->GetLook().y, m_Mummy[0]->GetLook().z), -1));
			//m_MummyLaser[i]->ConnectParticle(m_Particles->GetParticleObj(idx));
			m_Mummy[0]->AddFriends_Laser(m_MummyLaser[i]);
			m_MummyLaser[i]->AddFriends_p(m_Mummy[0]);
		}
	}

	if (pMummy->GetEnemyAttackType() == EnemyAttackType::Mummy2 && m_MummyExist[1] == true)
	{
		m_SoundManager->PlayEffect(Sound_Name::EFFECT_Laser);
		// �̶� 2���� ��������
		if (m_One_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2000.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 1.5f, scale.y * 1.5f, scale.z * 1.5f));
			}
			m_One_Mira_Die_Laser = false;
		}
		if (m_Two_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2800.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 3.f, scale.y * 3.f, scale.z * 3.f));
			}
			m_Two_Mira_Die_Laser = false;
		}
		for (int i = 0; i < 3; i++)
		{
			m_MummyLaser2[i]->SetUseable(false);
			XMFLOAT3 pos2 = Vector3::Add(XMFLOAT3{ m_Mummy[1]->GetPosition() }, { 0,200,0 });
			m_MummyLaser2[i]->SetPosition(pos2);
			m_MummyLaser2[i]->SetLaser(i);
			m_MummyLaser2[i]->SetLaserType(Laser_TYPE::Laser2);
			m_Mummy[1]->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));
			m_MummyLaser2[i]->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));
			m_MummyLaser2[i]->LookAtDirections(Vector3::Multifly(XMFLOAT3(m_Mummy[1]->GetLook().x, m_Mummy[1]->GetLook().y, m_Mummy[1]->GetLook().z), -1));
			//m_MummyLaser[i]->ConnectParticle(m_Particles->GetParticleObj(idx));
			m_Mummy[1]->AddFriends_Laser(m_MummyLaser2[i]);
			m_MummyLaser2[i]->AddFriends_p(m_Mummy[1]);
		}
	}
	
	if (pMummy->GetEnemyAttackType() == EnemyAttackType::Mummy3 && m_MummyExist[2] == true)
	{
		m_SoundManager->PlayEffect(Sound_Name::EFFECT_Laser);
		// �̶� 2���� ��������
		if (m_One_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2000.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 1.5f, scale.y * 1.5f, scale.z * 1.5f));
			}
			m_One_Mira_Die_Laser = false;
		}
		if (m_Two_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2800.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 3.f, scale.y * 3.f, scale.z * 3.f));
			}
			m_Two_Mira_Die_Laser = false;
		}
		for (int i = 0; i < 3; i++)
		{
			m_MummyLaser3[i]->SetUseable(false);
			XMFLOAT3 pos3 = Vector3::Add(XMFLOAT3{ m_Mummy[2]->GetPosition() }, { 0,200,0 });
			m_MummyLaser3[i]->SetPosition(pos3);
			m_MummyLaser3[i]->SetLaser(i);
			m_MummyLaser3[i]->SetLaserType(Laser_TYPE::Laser3);
			m_Mummy[2]->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));
			m_MummyLaser3[i]->SetTargetVector(Vector3::Multifly(XMFLOAT3(0, 0, -150), 1));
			m_MummyLaser3[i]->LookAtDirections(Vector3::Multifly(XMFLOAT3(m_Mummy[2]->GetLook().x, m_Mummy[2]->GetLook().y, m_Mummy[2]->GetLook().z), -1));
			//m_MummyLaser[i]->ConnectParticle(m_Particles->GetParticleObj(idx));
			m_Mummy[2]->AddFriends_Laser(m_MummyLaser3[i]);
			m_MummyLaser3[i]->AddFriends_p(m_Mummy[2]);
		}
	}
		
}



void CGameScene::BuildPlayers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObjectVer2* pPlayerModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, "resources/FbxExported/Player.bin", NULL, true);

	m_Players[0] = new CPlayer(pd3dDevice, pd3dCommandList);
	m_Player = m_Players[0];

	m_Players[0]->SetChild(pPlayerModel, true);
	m_Players[0]->SetPosition({ 550.0f,   230.0f,  1850.0f });
	m_Players[0]->Scale(200, 200, 200);
	m_Players[0]->SetShadertoAll();

	m_PlayerCameras[0]->SetOffset(XMFLOAT3(0.0f, 1.5f, -4.0f));
	m_PlayerCameras[0]->SetTarget(m_Players[0]);
	m_Players[0]->SetCamera(m_PlayerCameras[0]);

	m_Players[0]->SetDrawable(true);
	m_Players[0]->SetWeapon(PlayerWeaponType::Sword);
	m_Players[0]->SetWeaponPointer();
	//m_Players[0]->SetTextureIndex(0x400); 

	//m_Players[0]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.4, 1.2, 0.4, XMFLOAT3{ 0,0.6,0 });
	//m_Players[0]->AddColider(new ColliderBox(XMFLOAT3(0, 0.6, 0), XMFLOAT3(0.2, 0.6, 0.2)));

	m_Players[0]->SetWeapon(PlayerWeaponType::Sword);
	++m_CurrentPlayerNum;

	m_MinimapCamera->SetTarget(m_Players[0]);
	//auto pBox = new CBox(pd3dDevice, pd3dCommandList, 80, 165.0, 80.0f);
	//pBox->SetShader(CShaderHandler::GetInstance().GetData("Object"));
	//pBox->SetTextureIndex(0x100); 
	//pBox->SetPosition({ 550.0f,  m_Terrain->GetDetailHeight(550.0f, 1850.0f) + 82.5f,  1850.0f }); 
	//m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pBox);

	for (int i = 1; i < MAX_ROOM_PLAYER; ++i) {
		pPlayerModel = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "resources/FbxExported/Player.bin", NULL, true);

		m_Players[i] = new CPlayer(pd3dDevice, pd3dCommandList);

		m_Players[i]->SetWeapon(PlayerWeaponType::Sword);

		m_Players[i]->SetCamera(m_PlayerCameras[i]);

		m_Players[i]->SetChild(pPlayerModel, true);
		m_Players[i]->SetPosition({ 550.0f,   230.0f,  1850.0f });
		m_Players[i]->Scale(200, 200, 200);
		m_Players[i]->SetShadertoAll();

		m_PlayerCameras[i]->SetOffset(XMFLOAT3(0.0f, 1.5f, -4.0f));
		m_PlayerCameras[i]->SetTarget(m_Players[i]);

		m_Players[i]->SetDrawable(false);
		m_Players[i]->SetWeaponPointer();

		m_Players[i]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.4, 1.2, 0.4, XMFLOAT3{ 0,0.6,0 });
		m_Players[i]->AddColider(new ColliderBox(XMFLOAT3(0, 0.6, 0), XMFLOAT3(0.2, 0.6, 0.2)));
	}
}

void CGameScene::EnterNewSector(int sectorNum)
{
	if (m_BlockingPlateToPreviousSector.size() <= 0) return;

	CGameObject* obj = m_BlockingPlateToPreviousSector[sectorNum - 1];
	m_BlockingPlateToPreviousSector.erase(sectorNum - 1);

	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(obj);
	EnterNewSector(sectorNum - 1);
}

void CGameScene::ActiveSkill(OBJECT_LAYER type, CGameObject* user)
{
	if (type == OBJECT_LAYER::FireBall) {
		for (auto* pObj : m_ObjectLayers[(int)OBJECT_LAYER::FireBall]) {
			CFireBall* pFireb = reinterpret_cast<CFireBall*>(pObj);
			if (pFireb->IsCanUse()) {
				int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::FireBallParticle);
				if (-1 != idx) {
					CPlayer* player = reinterpret_cast<CPlayer*>(user);
					pFireb->SetExistingSector((SECTOR_POSITION)player->GetPlayerExistingSector());
					pFireb->SetUseable(false);
					pFireb->SetSkill(user);
					cout << "shot fb" << endl;
				}

				break;
			}
		}
	}
}

void CGameScene::ShotPlayerArrow()
{
	int i = 0;
	for (auto* pObj : m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow]) {
		CArrow* pArrow = reinterpret_cast<CArrow*>(pObj);
		if (pArrow->IsCanUse()) {
			int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::ArrowParticle);
			if (-1 != idx) {
				cout << "��ƼŬ �ε��� " << idx << " ȭ�� �ε��� : " << i << " \n";
				pArrow->SetUseable(false);
				XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Player->GetPosition() }, { 0,180,0 });
				pArrow->SetPosition(pos);
				pArrow->m_startPos = pos;
				pArrow->SetStringPower(m_Player->GetStringPullTime());
				cout << "string power: " << m_Player->GetStringPullTime() << endl;
				pArrow->SetTargetVector(Vector3::Multifly(m_Player->GetLook(), 1));
				m_Particles->UseParticle(idx, pArrow->GetPosition(), XMFLOAT3(0.0f, 0.0f, -1.0f));
				m_Particles->SetDirection(idx, Vector3::Multifly(Vector3::Normalize(m_Player->GetLook()), -1));
				pArrow->ConnectParticle(m_Particles->GetParticleObj(idx));
				pArrow->SetExistingSector((SECTOR_POSITION)m_Player->GetPlayerExistingSector());
				m_SoundManager->PlayEffect(Sound_Name::EFFECT_ARROW_SHOT);
			}
			break;
		}
		++i;
	}
}


void CGameScene::ShotMonsterArrow(CEnemy* pEmeny, const XMFLOAT3& lookVector)
{
	for (auto* pObj : m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow]) {
		CArrow* pArrow = reinterpret_cast<CArrow*>(pObj);
		if (pArrow->IsCanUse()) {
			pArrow->SetUseable(false);
			XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ pEmeny->GetPosition() }, { 0,150,0 });
			pArrow->SetPosition(pos);
			pArrow->SetTargetVector(lookVector);
			pArrow->SetExistingSector(pEmeny->GetExistingSector());
			//m_SoundManager->PlayEffect(Sound_Name::EFFECT_ARROW_SHOT); 
			break;
		}
	}
}

void CGameScene::DeleteEnemy(CEnemy* pEmeny)
{
	int i = 0;
	auto res2 = std::find(m_ObjectLayers[(int)OBJECT_LAYER::Mummy].begin(), m_ObjectLayers[(int)OBJECT_LAYER::Mummy].end(), pEmeny);
	CMummy* pMummy = reinterpret_cast<CMummy*>(pEmeny);
	if (res2 != m_ObjectLayers[(int)OBJECT_LAYER::Mummy].end()) {

		if (pEmeny->GetEnemyAttackType() == EnemyAttackType::Mummy1)
		{
			m_MummyExist[0] = false;
			m_Mummy[1]->SetMummyDie(1);
			m_Mummy[2]->SetMummyDie(1);
			if (m_One_Mira_Die == true)
			{
				m_Two_Mira_Die = true;
				m_Two_Mira_Die_Laser = true;
				m_Mummy[1]->Scale(1.5f, 1.5f, 1.5f, true);
				m_Mummy[2]->Scale(1.5f, 1.5f, 1.5f, true);
			}
			m_Mummy[1]->Scale(1.5f, 1.5f, 1.5f, true);
			m_Mummy[2]->Scale(1.5f, 1.5f, 1.5f, true);
			
			m_ObjectLayers[(int)OBJECT_LAYER::Mummy].erase(res2);
			m_One_Mira_Die = true;
			m_One_Mira_Die_Laser = true;
		}

		else if (pEmeny->GetEnemyAttackType() == EnemyAttackType::Mummy2)
		{
			m_MummyExist[1] = false;
			m_Mummy[0]->SetMummyDie2(1);
			m_Mummy[2]->SetMummyDie2(1);
			if (m_One_Mira_Die == true)
			{
				m_Two_Mira_Die = true;
				m_Two_Mira_Die_Laser = true;
				m_Mummy[0]->Scale(1.5f, 1.5f, 1.5f, true);
				m_Mummy[2]->Scale(1.5f, 1.5f, 1.5f, true);
			}
			m_Mummy[0]->Scale(1.5f, 1.5f, 1.5f, true);
			m_Mummy[2]->Scale(1.5f, 1.5f, 1.5f, true);

			m_ObjectLayers[(int)OBJECT_LAYER::Mummy].erase(res2);
			m_One_Mira_Die = true;
			m_One_Mira_Die_Laser = true;
		}
		else if (pEmeny->GetEnemyAttackType() == EnemyAttackType::Mummy3)
		{
			m_MummyExist[2] = false;
			m_Mummy[0]->SetMummyDie3(1);
			m_Mummy[1]->SetMummyDie3(1);
			if (m_One_Mira_Die == true)
			{
				m_Two_Mira_Die = true;
				m_Two_Mira_Die_Laser = true;
				m_Mummy[0]->Scale(1.5f, 1.5f, 1.5f, true);
				m_Mummy[1]->Scale(1.5f, 1.5f, 1.5f, true);
			}
			m_Mummy[0]->Scale(1.5f, 1.5f, 1.5f, true);
			m_Mummy[1]->Scale(1.5f, 1.5f, 1.5f, true);

			m_ObjectLayers[(int)OBJECT_LAYER::Mummy].erase(res2);
			m_One_Mira_Die = true;
			m_One_Mira_Die_Laser = true;
		}

	}

	auto res = std::find(m_ObjectLayers[(int)OBJECT_LAYER::Enemy].begin(), m_ObjectLayers[(int)OBJECT_LAYER::Enemy].end(), pEmeny);
	if (res != m_ObjectLayers[(int)OBJECT_LAYER::Enemy].end()) {
		cout << " ���� ����\n";
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].erase(res);
	}
}


void CGameScene::MakingFog()
{
	int i = 0;

	int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::RadialParitcle);
	if (-1 != idx)
	{
		XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Player->GetPosition() }, { 0,250,0 });
		m_Particles->UseParticle(idx, XMFLOAT3(16800.f * MAP_SCALE_SIZE, -5070.f, 17500.f * MAP_SCALE_SIZE), XMFLOAT3(0.0f, 0.0f, -1.0f));
		//m_Particles->SetDirection(idx, Vector3::Multifly(Vector3::Normalize(m_Player->GetLook()), 1));
	}
}

void CGameScene::MakingRain()
{
	int i = 0;

	int idx = m_Particles->GetCanUseableParticle(PARTICLE_TYPE::RainParticle);
	if (-1 != idx)
	{
		XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Player->GetPosition() }, { 0,250,0 });
		m_Particles->UseParticle(idx, XMFLOAT3(5000.f * MAP_SCALE_SIZE, 0.f, 17500.f * MAP_SCALE_SIZE), XMFLOAT3(0.0f, 0.0f, -1.0f));
		//m_Particles->SetDirection(idx, Vector3::Multifly(Vector3::Normalize(m_Player->GetLook()), 1));
	}
}

void CGameScene::UseEffects(int effectType, const XMFLOAT3& xmf3Position)
{
	auto effect = m_EffectsHandler->RecycleEffect((EffectTypes)effectType);
	if (effect != nullptr) {
		effect->SetPosition(xmf3Position);
		if (effectType < 4)
			effect->FixPositionByTerrain(m_Terrain);
		effect->SetDrawable(true);
	}
}

void CGameScene::UseEffects(int effectType, const XMFLOAT3& xmf3Position, float wakeupTime)
{
	auto effect = m_EffectsHandler->RecycleEffect((EffectTypes)effectType);
	if (effect != nullptr) {
		effect->SetPosition(xmf3Position);
		if (effectType < 4)
			effect->FixPositionByTerrain(m_Terrain);
		effect->WakeUpAfterTime(wakeupTime);
	}
}

void CGameScene::SendMouseInputPacket()
{
	P_C2S_MOUSE_INPUT p_mouseInput;

	p_mouseInput.size = sizeof(p_mouseInput);
	p_mouseInput.type = PACKET_PROTOCOL::C2S_INGAME_MOUSE_INPUT;
	p_mouseInput.id = CFramework::GetInstance().GetPlayerId();
	p_mouseInput.inputNum = m_MousePositions.size();

	for (int i = 0; i < p_mouseInput.inputNum; ++i) {
		p_mouseInput.InputType[i] = m_MouseInputTypes[i];
		p_mouseInput.xInput[i] = FloatToInt(m_MousePositions[i].x);
		p_mouseInput.yInput[i] = FloatToInt(m_MousePositions[i].y);
	}
	m_MousePositions.clear();
	m_MouseInputTypes.clear();
	//p_mouseInput.InputType = m_prevMouseInputType; 

	int retVal = 0;
	SendPacket(&p_mouseInput);

	//cout << "���콺 �Է� ���� ũ�� : " << m_MousePositions.size() << "\n";
}

void CGameScene::BuildBoundingRegions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 4�� �� �׵θ�
	CGameObject* pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 100, 10000, 20000 * MAP_SCALE_SIZE, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 0,-2000,10000 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 100, 10000, 20000 * MAP_SCALE_SIZE, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 19950 * MAP_SCALE_SIZE,-2000,10000 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 20000 * MAP_SCALE_SIZE, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(20000 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000 * MAP_SCALE_SIZE,-2000,00 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 20000 * MAP_SCALE_SIZE, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(20000 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000 * MAP_SCALE_SIZE,-2000,19950 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Forest to DryDesrt �Ʒ� ���� ��  
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 9600 * MAP_SCALE_SIZE, 800, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(9600 * 0.5f * MAP_SCALE_SIZE, 800 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 4800 * MAP_SCALE_SIZE,-1000, 15900 * MAP_SCALE_SIZE });
	m_BlockingPlateToPreviousSector[0] = (std::move(pObject));
	//m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

// Forest to Desert ���� ��
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 800, 10000, 15200 * MAP_SCALE_SIZE, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 15200 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 10000 * MAP_SCALE_SIZE,-2000, 7600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Forest ���� �� ������ ���� 
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 2000 * MAP_SCALE_SIZE, 10000, 7000 * MAP_SCALE_SIZE, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2000 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 7000 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 5000 * MAP_SCALE_SIZE, -2000, 11100 * MAP_SCALE_SIZE });
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Desrt to DryDesrt and Rock ���� ��
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 400, 10000, 12800 * MAP_SCALE_SIZE, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(400 * 0.5f, 10000 * 0.5f, 12800 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 13800 * MAP_SCALE_SIZE, -2000, 13600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// boss ���� �߰� ��
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 800, 10000, 5600 * MAP_SCALE_SIZE, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 15600 * MAP_SCALE_SIZE,-2000, 10800 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 800, 10000, 5600 * MAP_SCALE_SIZE, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 18000 * MAP_SCALE_SIZE,-2000, 10800 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// �縷 ���� ���� ��
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 4000 * MAP_SCALE_SIZE, 1000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(4000 * 0.5f * MAP_SCALE_SIZE, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 11600 * MAP_SCALE_SIZE,-2000, 15600 * MAP_SCALE_SIZE });
	m_BlockingPlateToPreviousSector[1] = (std::move(pObject));

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 4000 * MAP_SCALE_SIZE, 1000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(4000 * 0.5f * MAP_SCALE_SIZE, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 11600 * MAP_SCALE_SIZE ,-3000, 3600 * MAP_SCALE_SIZE });
	m_BlockingPlateToPreviousSector[2] = (std::move(pObject));

	// ���� ���� �Ա� ���� ��
	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 2400 * MAP_SCALE_SIZE, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2400 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 14800 * MAP_SCALE_SIZE,-2000, 8000 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 2400 * MAP_SCALE_SIZE, 10000, 100, XMFLOAT3{ 0,0,0 });
	pObject->AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2400 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 18800 * MAP_SCALE_SIZE, -2000, 8000 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);
}

void CGameScene::RecvMouseProcessPacket()
{
	return;
	int retVal;
	char buffer[BUFSIZE + 1] = {};
	RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);

	PACKET_PROTOCOL type = (PACKET_PROTOCOL)buffer[1];

	// �÷��̾� �߰� Ȥ�� ���� ��Ŷ ����
	if (type == PACKET_PROTOCOL::S2C_INGAME_MOUSE_INPUT) {

	}
}
