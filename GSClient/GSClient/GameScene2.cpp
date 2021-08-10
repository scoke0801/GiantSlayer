#include "stdafx.h"
#include "GameScene2.h"

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
CTitleScene2::CTitleScene2()
{
	m_SoundManager = new CSoundManager();
	//m_SoundManager->AddStream("resources/sounds/TestTitle.mp3", Sound_Name::BGM_MAIN_GAME);

	m_SoundManager->AddSound("resources/sounds/ShotArrow.wav", Sound_Name::EFFECT_ARROW_SHOT);
	//m_SoundManager->PlayBgm(Sound_Name::BGM_MAIN_GAME);

	cout << "Enter CTitleScene2 \n";
	m_pd3dGraphicsRootSignature = NULL; 
}

CTitleScene2::~CTitleScene2()
{

}

void CTitleScene2::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height)
{  
	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CShaderHandler::GetInstance().SetUserID(ShaderHandlerUser::GAMESCENE); // 수정필요
	CShaderHandler::GetInstance().CreateAllShaders(pd3dDevice, m_pd3dGraphicsRootSignature);

	BuildMaterials(pd3dDevice, pd3dCommandList); 

	BuildCamera(pd3dDevice, pd3dCommandList, width, height);
	BuildLights(pd3dDevice, pd3dCommandList);
	BuildSceneFrameData(pd3dDevice, pd3dCommandList);
	BuildObjects(pd3dDevice, pd3dCommandList);
	BuildUIs(pd3dDevice, pd3dCommandList);

	m_CreatedTime = chrono::high_resolution_clock::now();
}

void CTitleScene2::BuildCamera(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	int width, int height)
{
	int nCameras = 1;
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
	 
}

void CTitleScene2::BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

void CTitleScene2::BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

void CTitleScene2::BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbMaterialBytes = ((sizeof(CB_GAMESCENE_FRAME_DATA) + 255) & ~255); //256의 배수
	m_pd3dcbSceneInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		NULL, ncbMaterialBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbSceneInfo->Map(0, NULL, (void**)&m_pcbMappedSceneFrameData);
}

void CTitleScene2::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto start_t = chrono::high_resolution_clock::now();
	m_pfbxManager = FbxManager::Create();
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	m_pfbxManager->SetIOSettings(m_pfbxIOs);

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		m_ObjectLayers[i].reserve(50);
	}    

	auto end_t = chrono::high_resolution_clock::now();

	auto elasepsed_t = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_t);
	cout << "InitElapsed Time : " << elasepsed_t.count() * 0.000001f << "\n";
}

void CTitleScene2::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
	};

	for (int i = 0; i < _countof(keyNames); ++i)
	{
		unique_ptr<CTexture> tempTex = make_unique<CTexture>();
		MakeTexture(pd3dDevice, pd3dCommandList, tempTex.get(), keyNames[i], address[i]);
		m_Textures[tempTex->m_Name] = std::move(tempTex);
	}
}

void CTitleScene2::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
	   
	TextHandler::GetInstance().InitVertexBuffer(pd3dDevice, pd3dCommandList, m_pd3dSrvDescriptorHeap, m_Textures.size() + 1); 
}

void CTitleScene2::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	//m_Objects.clear();
}

void CTitleScene2::Update(float elapsedTime)
{
	m_SoundManager->OnUpdate();
	ProcessInput();

	
	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) { 
			pObject->Update(elapsedTime);
			pObject->UpdateColliders();
		}
	}
	   
	if (m_CurrentCamera) m_CurrentCamera->Update(elapsedTime); 
}

void CTitleScene2::UpdateForMultiplay(float elapsedTime)
{
	m_SoundManager->OnUpdate();
	ProcessInput();

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->UpdateOnServer(elapsedTime);
			pObject->UpdateColliders();
		}
	}
	if (m_CurrentCamera) m_CurrentCamera->Update(elapsedTime);

}
void CTitleScene2::AnimateObjects(float fTimeElapsed)
{
}

void CTitleScene2::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}
	  
	auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		chrono::high_resolution_clock::now() - m_CreatedTime);
	m_pcbMappedSceneFrameData->m_Time = timeElapsed.count() * 0.001f;
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData
	  
	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			pObject->Draw(pd3dCommandList, m_CurrentCamera);
		}
	} 
}

void CTitleScene2::DrawUI(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (UI* pUI : m_UIs)
	{
		pUI->Draw(pd3dCommandList, m_CurrentCamera);
	} 
} 
void CTitleScene2::DrawFont(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// draw the text
	TextHandler::GetInstance().Render(pd3dCommandList, std::wstring(L"나옴1234abcd!@#"),
		XMFLOAT2(0.02f, 0.51f), XMFLOAT2(2.0f, 2.0f));
}
   
void CTitleScene2::Communicate(SOCKET& sock)
{
	P_C2S_UPDATE_SYNC_REQUEST p_syncUpdateRequest;
	p_syncUpdateRequest.size = sizeof(P_C2S_UPDATE_SYNC_REQUEST);
	p_syncUpdateRequest.type = PACKET_PROTOCOL::C2S_INGAME_UPDATE_SYNC;
	//p_syncUpdateRequest.playerNum = m_CurrentPlayerNum;

	int retVal;
	bool haveToRecv = false; 

	if (m_MousePositions.size() > 0) {
		SendMouseInputPacket(); 
	}
}

void CTitleScene2::ProcessPacket(unsigned char* p_buf)
{
	 
}

void CTitleScene2::LoginToServer()
{
	P_C2S_LOGIN p_login;
	p_login.size = sizeof(p_login);
	p_login.type = PACKET_PROTOCOL::C2S_LOGIN;
	p_login.roomIndex = -1;
	strcpy_s(p_login.name, CFramework::GetInstance().GetPlayerName().c_str());

	SendPacket(&p_login);
}

void CTitleScene2::LogoutToServer()
{
}

void CTitleScene2::ProcessInput()
{
	if (false == m_IsFocusOn) {
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
	}
}

void CTitleScene2::ProcessWindowKeyboard(WPARAM wParam, bool isKeyUp)
{
	if (isKeyUp == false)
	{
		if (wParam == VK_9) {
			 
		} 
	}
	else
	{
		if (wParam == VK_J) {
		 
		}
	}
}

void CTitleScene2::OnMouseDown(WPARAM btnState, int x, int y)
{
	if (false == m_IsFocusOn) {
		return;
	} 
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
	SetCapture(CFramework::GetInstance().GetHWND());
}

void CTitleScene2::OnMouseUp(WPARAM btnState, int x, int y)
{
	if (false == m_IsFocusOn) {
		return;
	}
	 
	ReleaseCapture();
}

void CTitleScene2::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (false == m_IsFocusOn) {
		return;
	} 
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

void CTitleScene2::ReleaseUploadBuffers()
{
	for (int i = 0; i < (int)OBJECT_LAYER::Count; ++i) {
		for (auto pObject : m_ObjectLayers[i])
		{
			pObject->ReleaseUploadBuffers();
		}
	}
}

ID3D12RootSignature* CTitleScene2::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
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

void CTitleScene2::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	pUI = new UI(pd3dDevice, pd3dCommandList, 0.1f, 0.1f, 0.0f, false);
	pUI->SetPosition({ -0.53, 0.65,  0 });		// WeaponUI
	pUI->SetTextureIndex(0x10);
	pUI->SetShader(CShaderHandler::GetInstance().GetData("Ui"));
	m_UIs.push_back(pUI);
}



void CTitleScene2::SendMouseInputPacket()
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

	//cout << "마우스 입력 전송 크기 : " << m_MousePositions.size() << "\n";
}

void CTitleScene2::RecvMouseProcessPacket()
{
	return;
	int retVal;
	char buffer[BUFSIZE + 1] = {};
	RecvPacket(CFramework::GetInstance().GetSocket(), buffer, retVal);

	PACKET_PROTOCOL type = (PACKET_PROTOCOL)buffer[1];

	// 플레이어 추가 혹은 삭제 패킷 수신
	if (type == PACKET_PROTOCOL::S2C_INGAME_MOUSE_INPUT) {

	}
}
