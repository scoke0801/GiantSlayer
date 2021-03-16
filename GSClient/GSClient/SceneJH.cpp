#include "stdafx.h"
#include "SceneJH.h"
#include "stdafx.h" 
#include "GameFramework.h"
#include "Shader.h"
#include "UI.h"
#include "Camera.h"
#include "Player.h"
#include "Bridge.h"
#include "Wall.h"
#include "Communicates.h"
#include "Enemy.h"
#include "Puzzle.h"
#include "Terrain.h"
#include "Sign.h"

#define ROOT_PARAMETER_OBJECT			0
#define ROOT_PARAMETER_SCENE_FRAME_DATA 1
#define ROOT_PARAMETER_CAMERA			2
#define ROOT_PARAMETER_MATERIAL			3
#define ROOT_PARAMETER_LIGHT			4
#define ROOT_PARAMETER_TEXTURE			5

CSceneJH::CSceneJH()
{
	cout << "Enter CSceneJH \n";
	m_pd3dGraphicsRootSignature = NULL;
}

CSceneJH::~CSceneJH()
{
}

void CSceneJH::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height)
{
	BuildMirrorResource(pd3dDevice);
	BuildMinimapResource(pd3dDevice);
	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	BuildMaterials(pd3dDevice, pd3dCommandList);
	BuildCamera(pd3dDevice, pd3dCommandList, width, height);
	BuildLights(pd3dDevice, pd3dCommandList); 
	BuildSceneFrameData(pd3dDevice, pd3dCommandList);
	BuildObjects(pd3dDevice, pd3dCommandList);
	BuildBilboardObjects(pd3dDevice, pd3dCommandList);
	BuildUIs(pd3dDevice, pd3dCommandList);
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
		pCamera->SetLens(0.25f * PI, width, height, 1.0f, 60000.0f);
		pCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
		pCamera->SetScissorRect(0, 0, width, height);
		pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_Cameras[i] = pCamera;
	} 
	m_Cameras[0]->SetPosition({ 500,  250 + 150, 1200 }); 
	m_Cameras[0]->Pitch(XMConvertToRadians(15));
	m_Cameras[0]->SetOffset(XMFLOAT3(0.0f, 70.0f, -300.0f));

	m_Cameras[1]->SetPosition({ 500,  1500, 1500 }); 
	m_Cameras[1]->Pitch(XMConvertToRadians(90));  
	m_Cameras[2]->SetPosition({ 2500,  0, 2500 });
	m_Cameras[3]->SetPosition({ 2000, 1000, 8000 });
	m_Cameras[4]->SetPosition({ 10000,  22000, 10000 });
	m_Cameras[4]->Pitch(XMConvertToRadians(90));

	m_CurrentCamera = m_Cameras[0];
	m_MinimapCamera = m_Cameras[1];
	m_MirrorCamera = m_Cameras[3];
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

	//m_pLights->m_pLights[0].m_bEnable = true;
	//m_pLights->m_pLights[0].m_nType = SPOT_LIGHT;
	//m_pLights->m_pLights[0].m_fRange = 1500.0f;
	//m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	//m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	//m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	//if (m_CurrentCamera)
	//{
	//	m_pLights->m_pLights[0].m_xmf3Position = m_CurrentCamera->GetPosition3f();
	//	m_pLights->m_pLights[0].m_xmf3Direction = m_CurrentCamera->GetLook3f();
	//	m_CurrentCamera->SetLight(&m_pLights->m_pLights[0]);
	//} 
	//m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	//m_pLights->m_pLights[0].m_fFalloff = 14.0f;
	//m_pLights->m_pLights[0].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	//m_pLights->m_pLights[0].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 1000.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(0.0f, 300.0f, -150.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	 
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	m_pLights->m_pLights[4].m_bEnable = true;
	m_pLights->m_pLights[4].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[4].m_fRange = 600.0f;
	m_pLights->m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights->m_pLights[4].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[4].m_xmf3Direction = XMFLOAT3(0.577903390, -0.577350020, 0.576796055);
	m_pLights->m_pLights[4].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[4].m_fFalloff = 8.0f;
	m_pLights->m_pLights[4].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[4].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	  
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}

void CSceneJH::BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbMaterialBytes = ((sizeof(CB_GAMESCENE_FRAME_DATA) + 255) & ~255); //256의 배수
	m_pd3dcbSceneInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		NULL, ncbMaterialBytes, 
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbSceneInfo->Map(0, NULL, (void**)&m_pcbMappedSceneFrameData);
}

void CSceneJH::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pfbxManager = FbxManager::Create();
	m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	m_pfbxManager->SetIOSettings(m_pfbxIOs);
	  
	m_Objects.reserve(30);
	   
	CShader* pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTextured");
	pSkyBoxShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTextured");
	pSkyBoxShader->CreateInputLayout(ShaderTypes::Textured);
	pSkyBoxShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//pSkyBoxShader->CreateFBXMeshShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_Skybox = new CSkyBox(pd3dDevice, pd3dCommandList, pSkyBoxShader);
	//m_Skybox = new CSkyBoxSphere(pd3dDevice, pd3dCommandList, pSkyBoxShader, 10, 15, 15);

#pragma region Create Terrain
	CShader* pTerrainShader = new CTerrainTessellationShader();
	pTerrainShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTerrainTessellation");
	pTerrainShader->CreatePixelShader( L"Shaders\\ShaderJH.hlsl", "PSTerrainTessellation");
	pTerrainShader->CreateDomainShader(L"Shaders\\ShaderJH.hlsl", "DSTerrainTessellation");
	pTerrainShader->CreateHullShader(  L"Shaders\\ShaderJH.hlsl", "HSTerrainTessellation");
	pTerrainShader->CreateInputLayout(ShaderTypes::Terrain);
	pTerrainShader->CreateTerrainShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	 
	//m_Terrain = new CTerrain(pd3dDevice, pd3dCommandList, 257, 257, 9, 9, pTerrainShader);  
	m_Terrain = new CTerrain(pd3dDevice, pd3dCommandList, pTerrainShader);
#pragma endregion   

	CShader* pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
	pShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSBridgeLight");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature); 
	BuildBridges(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShader);

	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
	pShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTexturedLighting");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	BuildDoorWall(pd3dDevice, pd3dCommandList, pShader);
	BuildEnemys(pd3dDevice, pd3dCommandList);
	BuildPuzzles(pd3dDevice, pd3dCommandList);

// Objects Test Setings
	/// FBX Model
	CShader* pFBXShader = new CShader();
	pFBXShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
	pFBXShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTexturedLighting");
	pFBXShader->CreateInputLayout(ShaderTypes::Textured);
	pFBXShader->CreateFBXMeshShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pFBXShader->CreateBoundaryShader(pd3dDevice, m_pd3dGraphicsRootSignature );

	CMeshFbx* fbxMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/babymos.fbx", true);
	CGameObject* pObject = new CGameObject();
	pObject->SetMesh(fbxMesh);
	pObject->SetPosition({ 2000,  650, 12550 });
	pObject->SetTextureIndex(0x01);
	pObject->SetShader(pShader);
	pObject->SetTextureIndex(0x80);
	pObject->Scale(15, 15, 15);
	m_Objects.push_back(std::move(pObject)); 

	CSphereMesh* pSphereMesh = new CSphereMesh(pd3dDevice, pd3dCommandList,
		30, 20, 20);
	pObject = new CGameObject();
	pObject->SetMesh(pSphereMesh);
	pObject->SetPosition({ 500,  250 + 82.5, 3300 });
	pObject->SetTextureIndex(0x80);
	pObject->SetShader(pShader);
	m_Objects.push_back(std::move(pObject));

///////////////////////////////////////////////////////////////////////////////	 
	fbxMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, m_pfbxManager, "resources/Fbx/Golem.fbx");
	m_Player = new CPlayer(pd3dDevice, pd3dCommandList);

	m_CurrentCamera->SetOffset(XMFLOAT3(0.0f, 450.0f, -750.0f));
	m_CurrentCamera->SetTarget(m_Player);

	m_Player->SetShader(pFBXShader); 
	m_Player->Scale(20, 20, 20);  
	m_Player->SetObjectName(OBJ_NAME::Player); 
	m_Player->SetPosition({ 750,  230, 1850 });
	m_Player->SetCamera(m_CurrentCamera);
	m_Player->SetTextureIndex(0x80);
	m_Player->SetMesh(fbxMesh);  
	m_Player->BuildBoundigMeshes(pd3dDevice, pd3dCommandList, 10, 10, 10);

	m_MinimapCamera->SetTarget(m_Player);  

	m_Mirror = new CGameObject();

	CPlaneMeshTextured* pMirrorMesh = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList, 4000.0f, 1000.0f, 1.0f);

	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
	pShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTexturedLighting");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, TRUE);

	m_Mirror->SetMesh(pMirrorMesh);
	m_Mirror->SetShader(pShader);
	m_Mirror->SetPosition({ 2000,500,10000 });
	m_Mirror->SetTextureIndex(0x800);

	// 첫번째 지형 표지판
	CSign* pSign = new CSign(pd3dDevice, pd3dCommandList, true, pShader);
	pSign->SetPosition({ 2700, 200,7000 });
	m_Objects.push_back(pSign);
	

	// 퍼즐 벽 표지판
	pSign = new CSign(pd3dDevice, pd3dCommandList, false, pShader);
	pSign->SetPosition({ 11200.0f, 0.0f - 1800.0f, 3200.0f + 5000.0f });
	m_Objects.push_back(pSign);

	//퍼즐 앞 표지판 
	//pSign = new CSign(pd3dDevice, pd3dCommandList, false, pShader);
	//pSign->SetPosition({ 11200.0f, 0.0f - 1800.0f, 1500.0f + 10500.0f });
	//m_Objects.push_back(pSign);
	//{ 10600.0f, 0.0f - 2000.0f, 1500.0f + 8000.0f }
}

void CSceneJH::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	const char* keyNames[] =
	{
		"Forest","Dry_Forest","Desert","Dry_Desert","Rocky_Terrain",
		"Sky_Front","Sky_Back", "Sky_Left", "Sky_Right","Sky_Top","Sky_Bottom",
		"Box","Wood", "Wall", "Door",
		"HP_SP","Minimap","WeaponUI",
		"HP_SP_PER",
		"Flower_Red","Flower_White","Grass_1","Grass_2","Tree","Cactus",
		"PuzzleBoard",
		"HelpText"
	};

	const wchar_t* address[] =
	{
		//Wood->WoodBoard2
		//WallTest2->StoneWallTexture3
		L"resources/OBJ/Forest.dds",L"resources/OBJ/Dry_Forest.dds",L"resources/OBJ/Desert.dds",L"resources/OBJ/Dry_Desert.dds",L"resources/OBJ/Rocky_Terrain.dds",
		L"resources/skybox/front.dds",L"resources/skybox/back.dds", L"resources/skybox/left.dds",L"resources/skybox/right.dds",L"resources/skybox/top.dds", L"resources/skybox/bottom.dds",
		L"resources/OBJ/Box.dds",L"resources/OBJ/WoodBoard2.dds",  L"resources/OBJ/StoneWallTexture3.dds", L"resources/OBJ/Door3.dds",
		L"resources/UI/HP_SP.dds", L"resources/UI/Minimap.dds", L"resources/UI/Weapon.dds",L"resources/UI/SmallICons.dds",
		L"resources/Billboard/Flower01.dds",L"resources/Billboard/Flower02.dds",L"resources/Billboard/Grass01.dds",L"resources/Billboard/Grass02.dds",
		L"resources/Billboard/Tree02.dds", L"resources/Billboard/Cactus.dds",
		L"resources/OBJ/Board_Test.dds",
		L"resources/UI/HelpText.dds" 
	};
	 
	for (int i = 0; i < _countof(keyNames); ++i)
	{
		unique_ptr<CTexture> tempTex = make_unique<CTexture>();
		MakeTexture(pd3dDevice, pd3dCommandList, tempTex.get(), keyNames[i], address[i]); 
		m_Textures[tempTex->m_Name] = std::move(tempTex);
	}       
}

void CSceneJH::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{ 
	// Create the SRV heap. 
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size() + 2;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dBasicSrvDescriptorHeap));
	 
	// Fill out the heap with actual descriptors. 
	D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = m_pd3dBasicSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	
	const char* keyNames[] =
	{
		"Forest","Dry_Forest","Desert","Dry_Desert","Rocky_Terrain",
		"Sky_Front", "Sky_Back", "Sky_Left", "Sky_Right", "Sky_Top","Sky_Bottom",
		"Box", "Wood", "Wall", "Door",
		"HP_SP","HP_SP_PER",
		"Minimap",
		"WeaponUI",
		"Flower_Red","Flower_White","Grass_1","Grass_2","Tree","Cactus", "PuzzleBoard",
		"HelpText"
	};
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; 
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1; 
	for (int i = 0; i < _countof(keyNames); ++i)
	{
		if(i != 0)	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
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
}

void CSceneJH::ReleaseObjects()
{  
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
 
	m_Objects.clear();
}

void CSceneJH::Update(double elapsedTime)
{
	ProcessInput();
	
	m_Skybox->Rotate(XMFLOAT3(0, 1, 0), 0.3 * elapsedTime);

	for (auto pObject : m_Objects)
	{
		pObject->Update(elapsedTime);
	} 
	m_HelpTextUI->Update(elapsedTime);
	
	m_Player->Update(elapsedTime);
	m_Player->FixPositionByTerrain(m_Terrain);

	if (m_CurrentCamera) m_CurrentCamera->Update(elapsedTime);
	if (m_MirrorCamera)
	{
		m_MirrorCamera->UpdateViewMatrix();
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

void CSceneJH::AnimateObjects(float fTimeElapsed)
{
}

void CSceneJH::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dBasicSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	  
	m_pcbMappedSceneFrameData->m_PlayerHP = m_Player->GetHP();
	m_pcbMappedSceneFrameData->m_PlayerSP = m_Player->GetSP();
	m_pcbMappedSceneFrameData->m_PlayerWeapon = m_Player->GetSelectedWeapon();
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbSceneFrameDataGpuVirtualAddress = m_pd3dcbSceneInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_SCENE_FRAME_DATA, d3dcbSceneFrameDataGpuVirtualAddress); //GameSceneFrameData

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials

	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dBasicSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE, tex);
	 
	m_Skybox->Draw(pd3dCommandList, m_CurrentCamera);
	m_Terrain->Draw(pd3dCommandList, m_CurrentCamera);
	m_Mirror->Draw(pd3dCommandList, m_CurrentCamera);

	for (auto pBillboardObject : m_BillboardObjects)
	{
		pBillboardObject->Draw(pd3dCommandList, m_CurrentCamera);
	}

	for (auto pObject : m_Objects)
	{
		pObject->Draw(pd3dCommandList, m_CurrentCamera);
	} 
}

void CSceneJH::DrawUI(ID3D12GraphicsCommandList* pd3dCommandList)
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

void CSceneJH::DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_Player) m_Player->Draw(pd3dCommandList, m_CurrentCamera);
}

void CSceneJH::FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList)
{ 
}

void CSceneJH::DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_MinimapCamera)
	{
		m_MinimapCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_MinimapCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}
	 
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dBasicSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dBasicSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
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

	for (auto pBillboardObject : m_BillboardObjects)
	{
		pBillboardObject->Draw(pd3dCommandList, m_CurrentCamera);
	}
	for (auto pObject : m_Objects)
	{
		pObject->Draw(pd3dCommandList, m_CurrentCamera);
	}

	//m_Player->Draw(pd3dCommandList, m_MinimapCamera);

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

void CSceneJH::DrawMirror(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_MirrorCamera)
	{
		m_MirrorCamera->UpdateShaderVariables(pd3dCommandList, ROOT_PARAMETER_CAMERA);
		m_MirrorCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dBasicSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dBasicSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
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

void CSceneJH::Communicate(SOCKET& sock)
{
	int retVal = 0;

	string toSendData ="\n";
	XMFLOAT3 xmf3PlayerPos = m_Player->GetPosition();
	XMFLOAT3 xmf3PlayerLook = m_Player->GetLook();
	toSendData += "<PlayerPosition>:\n";
	toSendData += to_string(xmf3PlayerPos.x);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerPos.y);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerPos.z);
	toSendData += "\n";
	toSendData += "<PlayerLook>:\n";
	toSendData += to_string(xmf3PlayerLook.x);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerLook.y);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerLook.z);
	toSendData += "\n"; 

	auto keyInput = GAME_INPUT;
	toSendData += "<Command>\n"; 
	toSendData += to_string(CInputHandler::GetInstance().GetCommandType());
	toSendData += "\n";
	SendFrameData(sock, toSendData, retVal);

	char buffer[BUFSIZE + 1];
	 
	RecvFrameData(sock, buffer, retVal);
	char* token = strtok(buffer, "\n");
	while (token != NULL)
	{
		if (strstr(token, "<PlayerPosition>:"))
		{
			XMFLOAT3 res = GetVectorFromText(token);
			cout << "<PlayerPosition>: ";
			DisplayVector3(res);
		}
		else if (strstr(token, "<PlayerLook>:"))
		{
			XMFLOAT3 res = GetVectorFromText(token);
			cout << "<PlayerLook>: ";
			DisplayVector3(res);
		}
		token = strtok(NULL, "\n");
	}
}

void CSceneJH::ProcessInput()
{
	if (m_CurrentCamera == nullptr) return;

	float cameraSpeed = m_CurrentCamera->GetSpeed();
	XMFLOAT3 velocity = m_Player->GetVelocity();

	auto keyInput = GAME_INPUT;
	if (keyInput.KEY_W)
	{
		if(m_isPlayerSelected)
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
		m_CurrentCamera = m_Cameras[3];
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
		m_Player->SetPosition({ 2500,  0, 2500 }); 
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
		m_Player->SetPosition({ 12500,  -3000, 2500 });
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
	}
	if (keyInput.KEY_L)
	{
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

		if (m_isPlayerSelected)
		{
			//m_CurrentCamera->RotateAroundTarget(XMFLOAT3(1, 0, 0), dy * 30);
			m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 15);

			if (m_Player->IsMoving())
			{
				m_Player->Rotate(XMFLOAT3(0, 1, 0), dx * 15);
				m_MinimapArrow->Rotate(-dx * 15);
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

void CSceneJH::ReleaseUploadBuffers()
{
	for (auto pObject : m_Objects)
	{
		pObject->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* CSceneJH::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// Ground
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = m_Textures.size()+ 2;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; 

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[6];
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
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Material
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Light
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

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

void CSceneJH::BuildBridges(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature,
	CShader* pShader)
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

void CSceneJH::BuildDoorWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	CShader* pShader)
{
	CDoorWall* pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 4000, 1000, 500, pShader);
	pDoorWall->SetPosition({ 0,0, 7500 });
	m_Objects.push_back(pDoorWall); 

	pDoorWall = new CDoorWall(pd3dDevice, pd3dCommandList, 3300, 1000, 500, pShader);
	pDoorWall->SetPosition({ 10300, -2000, 7500 });
	m_Objects.push_back(pDoorWall);
}

void CSceneJH::BuildEnemys(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject* pEnemy = new CEnemy();
}

void CSceneJH::BuildPuzzles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CShader* pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
	pShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSPuzzle");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false, true);

	CPlate* pPuzzlePlate = new CPlate(pd3dDevice, pd3dCommandList, pShader);
	pPuzzlePlate->SetPosition({ 10600.0f,  0.0f - 2000.0f, 1500.0f + 8000.0f });
	pPuzzlePlate->SetShader(pShader);
	//pPuzzlePlate->RotateAll ({ 0,1,0 }, 180);
	m_Objects.push_back(std::move(pPuzzlePlate));

	CGameObject* pObject = new CPuzzle(pd3dDevice, pd3dCommandList, PuzzleType::Holding, pShader);
	pObject->SetPosition({ 10500.0f,  0.0f - 2000.0f, 1500.0f + 8000.0f });
	pObject->SetShader(pShader);
	m_Objects.push_back(std::move(pObject)); 

	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
	pShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTexturedLighting");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			pObject = new CBox(pd3dDevice, pd3dCommandList, 150, 100, 150);
			pObject->SetPosition({ 10900.0f + i * 1800.0f,  300 - 2000.0f, 1800.0f + j * 300.0f + 8000.0f });
			pObject->SetTextureIndex(0x80);
			pObject->SetShader(pShader);
			m_Objects.push_back(std::move(pObject));
		}
	} 
}

void CSceneJH::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CShader* pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders/ShaderJH.hlsl", "VS_UI_Textured");
	pShader->CreatePixelShader(L"Shaders/ShaderJH.hlsl", "PS_UI_Textured");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateUIShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	UI* pUI = new UI(pd3dDevice, pd3dCommandList, 0.4f, 0.09f, 0.0f, true);
	pUI->SetPosition({ -0.50, 0.88,  0.92 });		// HP, SP
	pUI->SetTextureIndex(0x01);
	pUI->SetShader(pShader);
	m_UIs.push_back(pUI);

	pUI = new UI(pd3dDevice, pd3dCommandList, 0.3f, 0.09f, 0.0f, true);
	pUI->SetPosition({ -0.50, 0.79,  0.92 });		// HP, SP
	pUI->SetTextureIndex(0x02);
	pUI->SetShader(pShader);
	m_UIs.push_back(pUI);

	for (int i = 0; i < 20; ++i)
	{  
		pUI = new HpSpPercentUI(pd3dDevice, pd3dCommandList, 0.015, 0.053f, 0.0f, true);
		pUI->SetPosition({float( -0.615 + 0.015 * i), 0.885,  0.91 });
		pUI->SetTextureIndex(0x04);
		pUI->SetShader(pShader); 
		m_HPGauges.push_back(pUI);
	}
	for (int i = 0; i < 20; ++i)
	{ 
		pUI = new HpSpPercentUI(pd3dDevice, pd3dCommandList, 0.011, 0.053f, 0.0f, false);
		pUI->SetPosition({ float (-0.575 + 0.011 * i), 0.795,  0.91 });
		pUI->SetTextureIndex(0x04);
		pUI->SetShader(pShader);
		m_SPGauges.push_back(pUI);
	} 

	m_MinimapArrow = new MinimapArrow(pd3dDevice, pd3dCommandList, 0.05, 0.05, 0.0f);
	m_MinimapArrow->SetPosition({ -0.78f, 0.78f,  0.8 });	// MinimapArrow
	m_MinimapArrow->SetTextureIndex(0x04);
	m_MinimapArrow->SetShader(pShader);
	m_UIs.push_back(m_MinimapArrow);

	pUI = new UI(pd3dDevice, pd3dCommandList, 0.1f, 0.1f, 0.0f, true);
	pUI->SetPosition({ -0.53, 0.65,  0 });		// WeaponUI
	pUI->SetTextureIndex(0x10);
	pUI->SetShader(pShader);
	m_UIs.push_back(pUI); 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders/ShaderJH.hlsl", "VS_UI_Textured");
	pShader->CreatePixelShader(L"Shaders/ShaderJH.hlsl", "PS_UI_HelpText");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateUIShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	m_HelpTextUI = new HelpTextUI(pd3dDevice, pd3dCommandList, 1.5, 0.25f, 0.8f, HELP_TEXT_INFO::QuestAccept);
	m_HelpTextUI->SetPosition({ 0.0f, -0.8,  0 });		 
	m_HelpTextUI->SetTextureIndex(0x01);
	m_HelpTextUI->SetShader(pShader);
	m_UIs.push_back(m_HelpTextUI);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders/ShaderJH.hlsl", "VSMinimap");  
	pShader->CreatePixelShader(L"Shaders/ShaderJH.hlsl", "PSMinimap");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature); 

	pUI = new Minimap(pd3dDevice, pd3dCommandList, 0.20f);
	pUI->SetPosition({ -0.78f, 0.78f, 0.9 });	// Minmap Background
	pUI->SetTextureIndex(0x01);
	pUI->SetShader(pShader); 
	m_UIs.push_back(pUI);

	pUI = new Minimap(pd3dDevice, pd3dCommandList, 0.165f);
	pUI->SetPosition({ -0.78f, 0.78f,  0.8});	// Minimap
	pUI->SetTextureIndex(0x02);
	pUI->SetShader(pShader); 
	pUI->Rotate(180);
	m_UIs.push_back(pUI);  
}

void CSceneJH::BuildBilboardObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{ 
	CShader* pBillboardShader = new CBillboardShader();
	pBillboardShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSBillboard");
	pBillboardShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSBillboard");
	pBillboardShader->CreateGeometryShader(L"Shaders\\ShaderYJ.hlsl", "GSBillboard");
	pBillboardShader->CreateInputLayout(ShaderTypes::Billboard);
	pBillboardShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
#pragma region Create Tree
	// 지나가지 못하는 첫번째 지형쪽의 나무 빌보드
	CBillboardMesh* pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 160.0f, 160.0f);

	CGameObject* pBillboardObject = new CGameObject();

	for (int i = 0; i < 8; i++)
	{
		pBillboardObject = new CGameObject();

		pBillboardObject->SetMesh(pBillboardMesh);
		pBillboardObject->Scale(5.0f, 5.0f, 1.0f);

		float x_Tree = 4000 + 100.0f * i;
		float z_Tree = 4500 + 700.0f * i;
		pBillboardObject->SetPosition({ x_Tree , m_Terrain->GetHeight(x_Tree,z_Tree) + 80.0f * 5.0f, z_Tree });
		if (i == 5)
		{
			float x_Tree = 200 + 500.0f;
			float z_Tree = 3500 + 500.0f;
			pBillboardObject->SetPosition({ 200 + 500.0f , m_Terrain->GetHeight(x_Tree,z_Tree) + 80.0f * 5.0f, 3500 + 500.0f });
		}
		if (i == 6)
		{
			pBillboardObject->SetPosition({ 3000 ,m_Terrain->GetHeight(3000,4500) + 80.0f * 5.0f, 4500 });
		}
		if (i == 7)
		{
			pBillboardObject->SetPosition({ 3500 ,m_Terrain->GetHeight(3500,4500) + 80.0f * 5.0f , 4500 });
		}

		pBillboardObject->SetTextureIndex(0x010);
		pBillboardObject->SetShader(pBillboardShader);
		m_BillboardObjects.push_back(std::move(pBillboardObject));
	}
#pragma endregion 
}

void CSceneJH::BuildMinimapResource(ID3D12Device* pd3dDevice)
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

void CSceneJH::BuildMirrorResource(ID3D12Device* pd3dDevice)
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
