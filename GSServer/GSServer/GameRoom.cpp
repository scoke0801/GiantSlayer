#include "stdafx.h"
#include "GameRoom.h"
#include "protocol.h" 
#include "MapObjects.h"
#include "Enemy.h"
#include "Boss.h"
#include "Arrow.h"
#include "PacketProcessor.h"
#include "AnimationObject.h"

int g_Heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1]; 

CGameRoom::CGameRoom()
{ 
}

void CGameRoom::Update(float elapsedTime)
{
	if (m_CurrentPlayerNum == 0) {
		return;
	}
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
		pEnemy->FixPositionByTerrain(g_Heights);
	}
	for (auto pMummy : m_ObjectLayers[(int)OBJECT_LAYER::Mummy]) {
		pMummy->FixPositionByTerrain(g_Heights);
	}

	ZeroMemory(m_PlayerExistingSector, sizeof(m_PlayerExistingSector));
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Players[i]->IsExist()) {
			m_Players[i]->Update(elapsedTime);
			m_Players[i]->UpdateColliders();
			m_Players[i]->FixPositionByTerrain(g_Heights);
			m_Players[i]->FixCameraByTerrain(g_Heights);
			m_PlayerExistingSector[m_Players[i]->GetPlayerExistingSector()] = true;
		}
	}


	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary]) {
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pObstacle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pObstacle);
				//cout << "충돌발생 - [오브젝트, 플레이어 " << i << "]\n";
			}
		} 
	}
	// NPC랑 충돌
	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Npc]) {
		if (false == pObstacle->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pObstacle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pObstacle);
			}
		}
	}

	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Obstacle]) {
		if (false == pObstacle->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pObstacle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pObstacle);
				//cout << "충돌발생 - [오브젝트, 플레이어 " << i << "]\n";
			}
		}
	}


	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Bridge]) {
		if (false == pObstacle->IsInNearSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;


			if (pObstacle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pObstacle);
				m_Players[i]->UpdateCamera();
			}
		}
	}

	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser]) {
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pArrow->IsUsable()) {
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

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy]) {
		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (false == pEnemy->CollisionCheck(m_Players[i])) {
				continue;
			}
			if (false == m_Players[i]->IsCanAttack()) {
				if (false == m_Players[i]->IsAleradyAttack()) {
					pEnemy->ChangeState(ObjectState::Attacked, m_Players[i]);
					cout << "플레이어 공격 - 몬스터\n";
					m_Players[i]->SetAleradyAttack(true);
				}
			}
			else if (m_Players[i]->Attacked(pEnemy))
			{
				//m_CurrentCamera->SetShake(true, 0.5f, 15);
				m_Players[i]->FixCollision();
				cout << "충돌 : 플레이어 - 적\n";
			}
		}
	}
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow]) {
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pArrow->IsUsable()) {
			continue;
		}

		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (pArrow->CollisionCheck(m_Players[i])) {
				if (m_Players[i]->Attacked(pArrow)) {
					pArrow->SetIsUsable(true);
				}
			}
		}
	}
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow]) {
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pArrow->IsUsable()) {
			continue;
		}
		for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy])
		{
			if (false == pEnemy->IsInSameSector(pArrow->GetExistingSector())) {
				continue;
			}
			if (pArrow->CollisionCheck(pEnemy)) {
				pEnemy->ChangeState(ObjectState::Attacked, pArrow);
				pArrow->SetIsUsable(true);

				//cout << "충돌 : 플레이어 화살 - 적\n";
				break;
			}
		}
	}
	for (auto pPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::Puzzle]) { 
		if (false == pPuzzle->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (pPuzzle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pPuzzle);
				//m_isPlayerBoxCollide = true; 
			}
		}
	}
}
void CGameRoom::InitAll()
{  
	InitPlayers();
	InitCameras();
	InitMonsters();
	InitObstacle();
	InitArrows();
	BuildBlockingRegionOnMap();

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		m_PlayerExistingSector[i] = false;
	}
	for (int i = m_DoorStartIndex; i < m_DoorStartIndex + 5; ++i) {
		CDoorWall* pDoorWall = reinterpret_cast<CDoorWall*>(
			m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][i]);
		pDoorWall->OpenDoor();
	} 

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) {
			if (i == 9) {
				int stop =3;
			}
			pObject->Update(0.016f);
			pObject->UpdateColliders();
		}
	}	
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		m_Players[i]->UpdateColliders();
	}
}
 
void CGameRoom::InitPlayers()
{
	CAnimationObject* pPlayerModel;
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		pPlayerModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
			"resources/FbxExported/Player.bin", true);
		m_Players[i] = new CPlayer();
		m_Players[i]->SetChild(pPlayerModel, true);
		m_Players[i]->Scale(200, 200, 200); 
		m_Players[i]->SetPosition(PLAYER_START_POSITIONS[i]);
		m_Players[i]->SetExistence(false);
		//m_Players[i]->SetWeaponPointer();
	}
}

void CGameRoom::InitCameras()
{
	int nCameras = MAX_ROOM_PLAYER;
	const float PI = 3.141592;
	FRAME_BUFFER_WIDTH;
	for (int i = 0; i < nCameras; ++i)
	{
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.0f, 60000.0f);
		m_Cameras[i] = pCamera;

		m_Cameras[i]->SetPosition(m_Players[i]->GetPosition());
		m_Cameras[i]->Pitch(XMConvertToRadians(15));
		m_Cameras[i]->SetOffset(XMFLOAT3(0.0f, 1.5f, -4.0f));
		m_Cameras[i]->SetTarget(m_Players[i]);
		m_Players[i]->SetCamera(m_Cameras[i]);
	}
}

void CGameRoom::InitMonsters()
{
	CEnemy* pEnemy;
	CAnimationObject* pMonsterModel;
	XMFLOAT3 scale = { 300.0f,300.0f,300.0f };
	{	// Monster Area1
		for (int i = 0; i < 4; ++i) {
			pMonsterModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
				"resources/FbxExported/BasicSkeleton.bin", true);
			pEnemy = new CMeleeEnemy();
			pEnemy->SetChild(pMonsterModel, true);
			pEnemy->Scale(scale.x, scale.y, scale.z);
			pEnemy->SetPosition({ 2005.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 2005.0f * MAP_SCALE_SIZE, 11650.0f * MAP_SCALE_SIZE), 11650.0f * MAP_SCALE_SIZE });
			pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 2005.0f * MAP_SCALE_SIZE, 11650.0f * MAP_SCALE_SIZE), 11650.0f * MAP_SCALE_SIZE });
			pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
			pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 10, 3050 * 0.75f / scale.z });
			pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
			m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
		}
	}

	{	
		// Monster Area1-2
		string fileNames[4] = {
			"resources/FbxExported/BasicSkeleton.bin",
			"resources/FbxExported/MaceSkeleton.bin",
			"resources/FbxExported/StrongSkeleton.bin",
			"resources/FbxExported/ExeSkeleton.bin"
		};

		for (int i = 0; i < 4; ++i) {
			pMonsterModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
				fileNames[i].c_str(), true);
			pEnemy = new CMeleeEnemy();
			pEnemy->SetChild(pMonsterModel, true);
			pEnemy->Scale(scale.x, scale.y, scale.z);
			pEnemy->SetPosition({ 7800.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights,7800.0f * MAP_SCALE_SIZE,  11450.0f * MAP_SCALE_SIZE),  11450.0f * MAP_SCALE_SIZE });
			pEnemy->SetActivityScope({ 1600, 0, 2950 }, { 7800.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 7800.0f * MAP_SCALE_SIZE,  11450.0f * MAP_SCALE_SIZE),  11450.0f * MAP_SCALE_SIZE });
			pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
			pEnemy->SetSightBoundingBox({ 1600 * 0.75f / scale.x , 10, 2950 * 0.75f / scale.z });
			pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_1);
			m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

		} 
	}
	{// Monster Area2-1
		string fileNames[3] = {
			"resources/FbxExported/ExeSkeleton.bin",
			"resources/FbxExported/MaceSkeleton.bin",
			"resources/FbxExported/MaceSkeleton.bin", 
		};
		for (int i = 0; i < 3; ++i) {
			pMonsterModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
				fileNames[i].c_str(), true);
			pEnemy = new CMeleeEnemy();
			pEnemy->SetChild(pMonsterModel, true);
			//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
			pEnemy->Scale(scale.x, scale.y, scale.z);
			pEnemy->SetPosition({ 12100.0f * MAP_SCALE_SIZE,GetDetailHeight(g_Heights,12100.0f * MAP_SCALE_SIZE, 17950.0f * MAP_SCALE_SIZE), 17950.0f * MAP_SCALE_SIZE });
			pEnemy->SetActivityScope({ 1300, 0, 1450 }, { 12100.0f * MAP_SCALE_SIZE,GetDetailHeight(g_Heights,12100.0f * MAP_SCALE_SIZE, 17950.0f * MAP_SCALE_SIZE), 17950.0f * MAP_SCALE_SIZE });
			pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
			pEnemy->SetSightBoundingBox({ 1300 * 0.75f / scale.x, 10, 1450 * 0.75f / scale.z });
			pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
			m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
		} 
	}

	{// Monster Area2-2
		string fileNames[3] = {
			"resources/FbxExported/StrongSkeleton.bin",
			"resources/FbxExported/StrongSkeleton.bin",
			"resources/FbxExported/StrongSkeleton.bin",
		};
		for (int i = 0; i < 3; ++i) {
			pMonsterModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
				fileNames[i].c_str(), true);
			pEnemy = new CMeleeEnemy();
			pEnemy->SetChild(pMonsterModel, true);
			pEnemy->Scale(scale.x, scale.y, scale.z);
			pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE,GetDetailHeight(g_Heights,11900.0f * MAP_SCALE_SIZE, 13300.0f * MAP_SCALE_SIZE), 13300.0f * MAP_SCALE_SIZE });
			pEnemy->SetActivityScope({ 1400, 0, 1200 }, { 11900.0f * MAP_SCALE_SIZE,GetDetailHeight(g_Heights,11900.0f * MAP_SCALE_SIZE, 13300.0f * MAP_SCALE_SIZE), 13300.0f * MAP_SCALE_SIZE });
			pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
			pEnemy->SetSightBoundingBox({ 1400 * 0.75f / scale.x , 10, 1200 * 0.75f / scale.z });
			pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_3);
			m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
		} 
	}

	{// Monster Area3 
		scale = { 600.0f,600.0f,600.0f }; 
		for (int i = 0; i < 4; ++i) {
			pMonsterModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
				"resources/FbxExported/Skeleton.bin", true);
			pEnemy = new CMeleeEnemy();
			pEnemy->SetChild(pMonsterModel, true);
			//pEnemy->Rotate(XMFLOAT3(1, 0, 0), -90.0f);
			pEnemy->Scale(scale.x, scale.y, scale.z);
			pEnemy->SetPosition({ 11900.0f * MAP_SCALE_SIZE,GetDetailHeight(g_Heights,11900.0f * MAP_SCALE_SIZE, 3250.0f * MAP_SCALE_SIZE), 3250.0f * MAP_SCALE_SIZE });
			pEnemy->SetActivityScope({ 1200, 0, 2750 }, { 11900.0f * MAP_SCALE_SIZE,GetDetailHeight(g_Heights,11900.0f * MAP_SCALE_SIZE, 3250.0f * MAP_SCALE_SIZE), 3250.0f * MAP_SCALE_SIZE });
			pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum);
			pEnemy->SetSightBoundingBox({ 1200 * 0.75f / scale.x , 10, 2750 * 0.75f / scale.z });
			pEnemy->SetExistingSector(SECTOR_POSITION::SECTOR_4);
			m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));
		} 
	}

	for (int i = 0; i < m_ObjectLayers[(int)OBJECT_LAYER::Enemy].size(); ++i) {
		auto pEnemy = reinterpret_cast<CEnemy*>(m_ObjectLayers[(int)OBJECT_LAYER::Enemy][i]);
		pEnemy->ConnectPlayer(m_Players, MAX_ROOM_PLAYER);
	}
	 
	CBoss* pBoss = new CBoss(); 
	pBoss->SetPosition({ 17166 * MAP_SCALE_SIZE, -6983.47559, 17166 * MAP_SCALE_SIZE });
	pBoss->Scale(120, 120, 120);
	pBoss->Rotate({ 0,1,0 }, 180);
	pBoss->ConnectPlayer(m_Players, MAX_ROOM_PLAYER);

	XMFLOAT3 centerPos = pBoss->GetPosition();
	XMFLOAT3 scopeSize = { 4100 * 2, 0, 4100 * 2 };
	pBoss->SetActivityScope({ scopeSize.x, 0, scopeSize.z }, { centerPos });
	pBoss->SetSightBoundingBox({ scopeSize.x / scale.x, 15, scopeSize.z / scale.z });
	m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(pBoss);
}

void CGameRoom::InitArrows()
{
	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->Scale(25.0f, 25.0f, 25.0f);
		pArrow->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow].push_back(pArrow);
	}

	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->Scale(100.0f, 100.0f, 50.0f);
		pArrow->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow].push_back(pArrow);
	}
}
  
void CGameRoom::InitObstacle()
{
	// Bridge --------------------------------------------------------------------
	CGameObject* pObject = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_1);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_1]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_2);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_2]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_3);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::BRIDEGE_SEC2_SEC3_3]);
	pObject->Rotate({ 0, 1, 0 }, 90);
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	///////////////////////////////////////////////////////////////////////////////// 

	// PUZZLE----------------------------------------------------------------------
	pObject = new CPuzzle(OBJECT_ID::PUZZLE_1);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::PUZZLE_1]);
	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pObject);

	XMFLOAT3 tempPos = g_ObjectPositions[OBJECT_ID::PUZZLE_1];
	tempPos.x += 100;
	pObject = new CPlate(OBJECT_ID::PUZZLE_1_PLATE);
	pObject->SetPosition(tempPos);
	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pObject);

	for (int i = 0; i < 10; ++i) {
		pObject = new CPuzzleBox((OBJECT_ID)((int)OBJECT_ID::PUZZLE_BOX_1 + i));
		pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::PUZZLE_BOX_1 + i)]);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::PuzzleBox].push_back(pObject);
	}
	//pObject = new CPuzzle(OBJECT_ID::PUZZLE_2);
	//pObject->SetPosition(g_ObjectPositions[OBJECT_ID::PUZZLE_2]);
	//m_Objects.push_back(std::move(pObject));
/////////////////////////////////////////////////////////////////////////////////

// DoorWall----------------------------------------------------------------------
	m_DoorStartIndex = m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].size();
	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC1);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC1]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC2);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC2]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC3);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC3]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC4);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC4]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_5);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC5);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::DOOR_WALL_SEC5]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_5);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->Rotate({ 0,1,0 }, 90);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::WALL_1]);
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->Rotate({ 0,1,0 }, 90);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::WALL_2]);
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	////////////////////////////////////////////////////////////////////////////////

	// Sign-------------------------------------------------------------------------
	pObject = new CSign(OBJECT_ID::SIGN_SCROLL);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::SIGN_SCROLL]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1); 
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CSign(OBJECT_ID::SIGN_PUZZLE);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::SIGN_PUZZLE]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CSign(OBJECT_ID::SIGN_MEDUSA);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::SIGN_MEDUSA]);
	pObject->Rotate({ 0,1,0 }, 90.0f);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	//pObject = new CSign(OBJECT_ID::BOSS);
	//pObject->SetPosition(g_ObjectPositions[OBJECT_ID::BOSS]);
	//m_Objects.push_back(std::move(pObject));
////////////////////////////////////////////////////////////////////////////////
// FBX Models
//
	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject();
		pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_ROCK_1 + i)]);
		pObject->Scale(50, 50, 50);
		pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5 * 0.5f, 7 * 0.5f, 3 * 0.5f)));
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject();
		pObject->Scale(0.5f + 0.5 * i, 0.5f, 0.5f + 0.5 * i);
		pObject->Rotate({ 0,1,0 }, 60 + 30 * i);
		pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DRY_TREE_1 + i)]);
		pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}
	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject();
		pObject->Scale(0.5f + 0.5 * i, 0.5f, 0.5f + 0.5 * i);
		pObject->Rotate({ 0,1,0 }, 0 + 15 * i);
		pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DRY_TREE_3 + i)]);
		pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 100), XMFLOAT3(200 * 0.5f, 1500 * 0.5f, 150 * 0.5f)));
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	pObject = new CGameObject();
	pObject->Scale(20.0f, 20.0f, 20.0f);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::DRY_FOREST_STUMP_1]);
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(15 * 0.5f, 10 * 0.5f, 15 * 0.5f)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->Scale(150.0f, 150.0f, 150.0f);
	pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DEAD_TREE_1)]);
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	for (int i = 0; i < 2; ++i) {
		pObject = new CGameObject();
		pObject->Scale(150.0f + 50 * i, 150.0f + 50 * i, 150.0f + 50 * i);
		pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DRY_FOREST_DEAD_TREE_2 + i)]);
		pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(1, -5, -2.5), XMFLOAT3(1 * 0.5f, 5 * 0.5f, 1 * 0.5f)));
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_2);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 5; ++i) {
		if (i == 0) {
			pObject->Scale(4.0f, 4.0f, 4.0f);
		}
		else if (i == 1) {
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(2.0f, 2.0f, 2.0f);
		}
		else if (i == 4) {
			pObject->Scale(1.0f, 1.0f, 1.0f);
		}
		pObject->Scale(0.5f, 0.5f, 0.5f);

		pObject = new CGameObject();
		pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DESERT_ROCK_1 + i)]);
		pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	for (int i = 0; i < 5; ++i) {
		if (i == 0) {
			pObject->Rotate({ 0,1,0 }, 90);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		else if (i == 2) {
			pObject->Rotate({ 0,1,0 }, 270);
			pObject->Scale(3.0f, 3.0f, 3.0f);
		}
		else if (i == 4) {
			pObject->Rotate({ 0,1,0 }, 135);
			pObject->Scale(1.5f, 1.5f, 1.5f);
		}
		pObject->Scale(0.5f, 0.5f, 0.5f);

		pObject = new CGameObject();
		pObject->SetPosition(g_ObjectPositions[(OBJECT_ID)((int)OBJECT_ID::DESERT_ROCK_6 + i)]);
		pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 220, 0), XMFLOAT3(600 * 0.5f, 250 * 0.5f, 600 * 0.5f)));
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
		m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	}

	int stop = 3;
}
 
void CGameRoom::BuildBlockingRegionOnMap()
{
	CGameObject* pObject = new CGameObject();
	pObject->SetPosition({ 0,-2000,10000 });
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f)));
	pObject->SetPosition({ 19950,-2000,10000 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(20000 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000,-2000,00 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(20000 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000,-2000,19950 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Forest to DryDesrt 아래 방향 벽  
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(9600 * 0.5f, 800 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 4800,-1000, 15900 });
	m_BlockingPlateToPreviousSector[0] = (std::move(pObject));

	// Forest to Desert 왼쪽 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 15200 * 0.5f)));
	pObject->SetPosition({ 10000,-2000, 7600 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Forest 지역 내 못가는 지형 
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(2000 * 0.5f, 10000 * 0.5f, 7000 * 0.5f)));
	pObject->SetPosition({ 4000 + 1000, -2000, 11100 });
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Desrt to DryDesrt and Rock 왼쪽 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(400 * 0.5f, 10000 * 0.5f, 12800 * 0.5f)));
	pObject->SetPosition({ 13800, -2000, 7200 + 6400 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// boss 지역 중간 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f)));
	pObject->SetPosition({ 15200 + 400,-2000, 2800 + 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f)));
	pObject->SetPosition({ 17600 + 400,-2000, 2800 + 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// 사막 지역 가로 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(4000 * 0.5f, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 2000 + 9600,-2000, 15600 });
	m_BlockingPlateToPreviousSector[1] = (std::move(pObject));

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(4000 * 0.5f, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 2000 + 9600,-3000, 3600 });
	m_BlockingPlateToPreviousSector[2] = (std::move(pObject));

	// 보스 지역 입구 가로 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(2400 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 1200 + 13600,-2000, 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(2400 * 0.5f, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 1200 + 13600 + 1600 + 2400,-2000, 8000 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);
}

void CGameRoom::EnterNewSector(int sectorNum)
{
	if (m_BlockingPlateToPreviousSector.size() <= 0) return;

	CGameObject* obj = m_BlockingPlateToPreviousSector[sectorNum - 1];
	m_BlockingPlateToPreviousSector.erase(sectorNum - 1);

	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(obj);
	EnterNewSector(sectorNum - 1);
}

void CGameRoom::ResetPlayer(int player_id)
{
	delete m_Players[player_id];
	delete m_Cameras[player_id];

	m_Players[player_id] = new CPlayer();
	m_Players[player_id]->Scale(200, 200, 200);
	m_Players[player_id]->SetPosition(PLAYER_START_POSITIONS[player_id]);
	m_Players[player_id]->SetExistence(false);
	m_Players[player_id]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0.6, 0), XMFLOAT3(0.2, 0.6, 0.2)));
	int nCameras = MAX_ROOM_PLAYER;
	const float PI = 3.141592;

	CCamera* pCamera = new CCamera;
	pCamera->SetLens(0.25f * PI, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1.0f, 60000.0f);
	m_Cameras[player_id] = pCamera;
	m_Cameras[player_id]->SetPosition(m_Players[player_id]->GetPosition());
	m_Cameras[player_id]->Pitch(XMConvertToRadians(15));
	m_Cameras[player_id]->SetOffset(XMFLOAT3(0.0f, 1.5f, -4.0f));
	m_Cameras[player_id]->SetTarget(m_Players[player_id]);
	m_Players[player_id]->SetCamera(m_Cameras[player_id]);
	 
	m_Clients[player_id] = nullptr;
}
 
void CGameRoom::SendPacket(int p_id, void* p)
{
	unsigned char p_size = reinterpret_cast<unsigned char*>(p)[0];
	unsigned char p_type = reinterpret_cast<unsigned char*>(p)[1];

	EX_OVER* s_over = new EX_OVER;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_packetbuf, p, p_size);
	s_over->m_wsabuf.buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
	s_over->m_wsabuf.len = p_size;

	int id = m_IdIndexMatcher[p_id];
	auto ret = WSASend(m_Clients[id]->m_socket, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) {
			error_display("WSASend : ");
			Disconnect(id);
		}
	}
}

void CGameRoom::SendPacket(SOCKET& socket, void* p)
{
	unsigned char p_size = reinterpret_cast<unsigned char*>(p)[0];
	unsigned char p_type = reinterpret_cast<unsigned char*>(p)[1];

	EX_OVER* s_over = new EX_OVER;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_packetbuf, p, p_size);
	s_over->m_wsabuf.buf = reinterpret_cast<CHAR*>(s_over->m_packetbuf);
	s_over->m_wsabuf.len = p_size;
	 
	auto ret = WSASend(socket, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) {
			error_display("WSASend : "); 
		}
	}
}

void CGameRoom::SendSyncUpdatePacket()
{
	P_S2C_UPDATE_SYNC p_syncUpdate;
	p_syncUpdate.type = PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE;
	p_syncUpdate.size = sizeof(p_syncUpdate);

	p_syncUpdate.playerNum = m_CurrentPlayerNum;

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		p_syncUpdate.id[i] = i;// static_cast<char>(m_Players[i]->GetId());
		p_syncUpdate.hp[i] = m_Players[i]->GetHP();
		//p_syncUpdate.Sp[i] = m_Players[i]->GetSP();
		XMFLOAT3 pos = m_Players[i]->GetPosition();
		XMFLOAT3 look = Vector3::Normalize(m_Players[i]->GetLook());
		p_syncUpdate.posX[i] = FloatToInt(pos.x);
		p_syncUpdate.posY[i] = FloatToInt(pos.y);
		p_syncUpdate.posZ[i] = FloatToInt(pos.z);

		p_syncUpdate.lookX[i] = FloatToInt(look.x);
		p_syncUpdate.lookY[i] = FloatToInt(look.y);
		p_syncUpdate.lookZ[i] = FloatToInt(look.z);

		p_syncUpdate.weaponType[i] = m_Players[i]->GetWeaponType();
		p_syncUpdate.states[i] = m_Players[i]->GetAnimationSet();  
	}
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		p_syncUpdate.existance[i] = m_Players[i]->IsExist();
	}
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state == PL_STATE::PLST_CONNECTED) {
			SendPacket(m_Clients[i]->id, &p_syncUpdate);
		}
	}
}

void CGameRoom::SendMonsterActPacket()
{
	if (m_CurrentPlayerNum == 0) return;

	P_S2C_MONSTERS_UPDATE_SYNC p_monsterUpdate[MAX_MONSTER_COUNT];
	ZeroMemory(&p_monsterUpdate, sizeof(p_monsterUpdate));

	for (int i = 0; i < m_ObjectLayers[(int)OBJECT_LAYER::Enemy].size(); ++i) {
		auto mon = m_ObjectLayers[(int)OBJECT_LAYER::Enemy][i];

		XMFLOAT3 pos = mon->GetPosition();
		XMFLOAT3 look = Vector3::Normalize(mon->GetLook());

		p_monsterUpdate[i].type = PACKET_PROTOCOL::S2C_INGAME_MONSTER_ACT;
		p_monsterUpdate[i].size = sizeof(p_monsterUpdate);
		p_monsterUpdate[i].id = i;

		p_monsterUpdate[i].posX = FloatToInt(pos.x);
		p_monsterUpdate[i].posY = FloatToInt(pos.y);
		p_monsterUpdate[i].posZ = FloatToInt(pos.z);

		p_monsterUpdate[i].lookX = FloatToInt(look.x);
		p_monsterUpdate[i].lookY = FloatToInt(look.y);
		p_monsterUpdate[i].lookZ = FloatToInt(look.z);
		p_monsterUpdate[i].state = mon->GetAnimationType();
	}
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}
		for (int j = 0; j < m_ObjectLayers[(int)OBJECT_LAYER::Enemy].size(); ++j) {
			SendPacket(m_Clients[i]->id, &p_monsterUpdate[j]);
		}
	}
}

void CGameRoom::Disconnect(int packet_id)
{ 
	cout << "로그 아웃\n"; 
	m_Players[packet_id]->SetExistence(false);
	m_Clients[packet_id]->m_state = PLST_FREE;

	P_S2C_DELETE_PLAYER p_deletePlayer;
	p_deletePlayer.type = PACKET_PROTOCOL::S2C_DELETE_PLAYER;
	p_deletePlayer.size = sizeof(P_S2C_DELETE_PLAYER);
	p_deletePlayer.id = packet_id;

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i]->m_state != PLST_FREE) {
			SendPacket(m_Clients[i]->id, &p_deletePlayer);
		}
	}
	m_CurrentPlayerNum--;
	ResetPlayer(packet_id);

	if (m_CurrentPlayerNum <= 0) {
		m_IsActive = false;
	}
	m_IdIndexMatcher.erase(packet_id);
}
 
void CGameRoom::Disconnect(CLIENT& client)
{
	int player_id = m_IdIndexMatcher[client.id];
	if (player_id == -1) {
		return;
	}
	if (m_Clients[player_id] == nullptr) {
		return;
	}
	m_Players[player_id]->SetExistence(false);
	m_Clients[player_id]->m_state = PLST_FREE;

	P_S2C_DELETE_PLAYER p_deletePlayer;
	p_deletePlayer.type = PACKET_PROTOCOL::S2C_DELETE_PLAYER;
	p_deletePlayer.size = sizeof(P_S2C_DELETE_PLAYER);
	p_deletePlayer.id = player_id;

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if ( m_Clients[i] == nullptr ) {
			continue;
		}
		if (m_Clients[i]->m_state != PLST_FREE) {
			SendPacket(m_Clients[i]->id, &p_deletePlayer);
		}
	}
	m_CurrentPlayerNum--;
	ResetPlayer(player_id);

	if (m_CurrentPlayerNum <= 0) {
		m_IsActive = false;
	}
	m_IdIndexMatcher.erase(client.id);
}

void CGameRoom::DeleteObject(CGameObject* pObject, int layerIdx)
{
	auto res = std::find(m_ObjectLayers[layerIdx].begin(), m_ObjectLayers[layerIdx].end(), pObject);
	if (res != m_ObjectLayers[layerIdx].end()) {
		//cout << " 객체 삭제\n";
		m_ObjectLayers[layerIdx].erase(res);
	}
}
   
void CGameRoom::EnterPlayer(CLIENT& client, int weapontType)
{
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			m_Clients[i] = &client;
			m_IdIndexMatcher.emplace(client.id, i); 
			m_IsActive = true; 
			return;
		} 
	}
}

bool CGameRoom::CanEnter()
{
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) { 
			return true;
		}
	}
	return false;
}

void CGameRoom::InitPrevUserData(int c_id)
{ 
	m_Clients[m_IdIndexMatcher[c_id]]->m_prev_size = 0;
}

void CGameRoom::DoRecv(int s_id)
{
	int id = m_IdIndexMatcher[s_id];

	m_Clients[id]->m_recv_over.m_wsabuf.buf = reinterpret_cast<char*>(m_Clients[id]->m_recv_over.m_packetbuf) + m_Clients[id]->m_prev_size;
	m_Clients[id]->m_recv_over.m_wsabuf.len = MAX_BUFFER - m_Clients[id]->m_prev_size;

	memset(&m_Clients[id]->m_recv_over.m_over, 0, sizeof(m_Clients[id]->m_recv_over.m_over));

	DWORD r_flag = 0;
	auto ret = WSARecv(m_Clients[id]->m_socket, &m_Clients[id]->m_recv_over.m_wsabuf, 1, NULL,
		&r_flag, &m_Clients[id]->m_recv_over.m_over, recv_callback);

	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) {
			error_display("Error in RecvPacket: ");
			Disconnect(id);
		}
	}
}

void CGameRoom::ProcessPacket(int p_id, unsigned char* p_buf)
{
	int id = m_IdIndexMatcher[p_id];
	char buf[10000];
	int copyPos = 0;
	int retval;
	// buffer[0]의 값은 packet protocol size
	// buffer[1]의 값은 packet protocol type
	PACKET_PROTOCOL type = (PACKET_PROTOCOL)p_buf[1];
	switch (type) {
	case PACKET_PROTOCOL::C2S_LOGIN:
	{
		P_S2C_PROCESS_LOGIN p_processLogin;
		p_processLogin.size = sizeof(p_processLogin);
		p_processLogin.type = PACKET_PROTOCOL::S2C_LOGIN_HANDLE;

		P_C2S_LOGIN p_login = *reinterpret_cast<P_C2S_LOGIN*>(p_buf);
		// p_id mean packet_id... 
		cout << "player_id : " << p_id << " p_id : " << p_id << endl;
		if (m_CurrentPlayerNum > MAX_ROOM_PLAYER) {
			p_processLogin.isSuccess = false;
		}
		else {
			p_processLogin.isSuccess = true;

			m_Players[id]->SetExistence(true);
			m_Players[id]->SetId(p_id);
			m_Players[id]->SetWeaponType(p_login.weaponType);
			XMFLOAT3 pos = m_Players[id]->GetPosition();

			p_processLogin.x = FloatToInt(pos.x);
			p_processLogin.y = FloatToInt(pos.y);
			p_processLogin.z = FloatToInt(pos.z);
		}
		p_processLogin.id = p_id;
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			p_processLogin.existPlayer[i] = m_Players[i]->IsExist();
		}
		SendPacket(p_id, &p_processLogin);

		m_CurrentPlayerNum++;
	}
	break;
	case PACKET_PROTOCOL::C2S_LOGOUT:
	{
		P_C2S_LOGOUT p_logout =
			*reinterpret_cast<P_C2S_LOGOUT*>(p_buf);
		m_CurrentlyDeletedPlayerId = p_logout.id;
		m_Players[p_logout.id]->SetExistence(false);
	}
	break;
	case PACKET_PROTOCOL::C2S_INGAME_KEYBOARD_INPUT:
	{
		P_C2S_KEYBOARD_INPUT p_keyboard =
			*reinterpret_cast<P_C2S_KEYBOARD_INPUT*>(p_buf);

		XMFLOAT3 pos = m_Players[id]->GetPosition();
		XMFLOAT3 shift = XMFLOAT3(0, 0, 0);
		float distance = PLAYER_RUN_SPEED;
		
#ifdef _DEBUG 
		m_Players[id]->m_moveTime = p_keyboard.move_time;
#endif
		switch (p_keyboard.keyInput)
		{
		case VK_W:
			m_Players[id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[id]->GetLook3f(), distance));
			break;
		case VK_S:
			m_Players[id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[id]->GetLook3f(), -distance));
			break;
		case VK_A:
			m_Players[id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[id]->GetRight3f(), -distance));
			break;
		case VK_D:
			m_Players[id]->SetVelocity(Vector3::Add(shift,
				m_Cameras[id]->GetRight3f(), distance));
			break;
		case VK_J:
			if (m_Players[id]->IsCanAttack()) {
				m_Players[id]->Attack(0);
			}
			break;
		case VK_U:
			for (int i = m_DoorStartIndex; i < m_DoorStartIndex + 5; ++i) {
				CDoorWall* pDoorWall = reinterpret_cast<CDoorWall*>(
					m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][i]);
				pDoorWall->OpenDoor();
			}
			break;
		case VK_I:
			for (int i = m_DoorStartIndex; i < m_DoorStartIndex + 5; ++i) {
				CDoorWall* pDoorWall = reinterpret_cast<CDoorWall*>(
					m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][i]);
				pDoorWall->CloserDoor();
			}
			break;
		case VK_F1:
		{
			m_Players[p_keyboard.id]->SetPosition({ 1622 * MAP_SCALE_SIZE, 0, 10772 * MAP_SCALE_SIZE });
			m_Players[p_keyboard.id]->FixPositionByTerrain(g_Heights);
		}
		break;
		case VK_F2:
		{
			m_Players[p_keyboard.id]->SetPosition({ 5500 * MAP_SCALE_SIZE,  -1000, 18000 * MAP_SCALE_SIZE });
			m_Players[p_keyboard.id]->FixPositionByTerrain(g_Heights);
		}
		break;
		case VK_F3:
		{
			m_Players[p_keyboard.id]->SetPosition({ 11838.8 * MAP_SCALE_SIZE,  -1000, 10428.2 * MAP_SCALE_SIZE });
			m_Players[p_keyboard.id]->FixPositionByTerrain(g_Heights);
		}
		break;
		case VK_F4:
		{
			m_Players[p_keyboard.id]->SetPosition({ 17000 * MAP_SCALE_SIZE,  -6000, 5500 * MAP_SCALE_SIZE });
			m_Players[p_keyboard.id]->FixPositionByTerrain(g_Heights);
		}
		break;
		case VK_F5:
		{
			m_Players[p_keyboard.id]->SetPosition({ 16749.9 * MAP_SCALE_SIZE,  -6000, 8500.78 * MAP_SCALE_SIZE });
			m_Players[p_keyboard.id]->FixPositionByTerrain(g_Heights);
		}
		break;
		case VK_F6:
		{
			m_Players[p_keyboard.id]->SetPosition({ 16958.4 * MAP_SCALE_SIZE,  -6000, 14861.1 * MAP_SCALE_SIZE });
			m_Players[p_keyboard.id]->FixPositionByTerrain(g_Heights);
		} 
		break;
		}

		P_S2C_PROCESS_KEYBOARD p_keyboardProcess;
		p_keyboardProcess.size = sizeof(p_keyboardProcess);
		p_keyboardProcess.type = PACKET_PROTOCOL::S2C_INGAME_KEYBOARD_INPUT;

		p_keyboardProcess.posX = FloatToInt(pos.x);
		p_keyboardProcess.posY = FloatToInt(pos.y);
		p_keyboardProcess.posZ = FloatToInt(pos.z);

		XMFLOAT3 look = Vector3::Normalize(m_Players[id]->GetLook());
		//DisplayVector3(look);
		p_keyboardProcess.lookX = FloatToInt(look.x);
		p_keyboardProcess.lookY = FloatToInt(look.y);
		p_keyboardProcess.lookZ = FloatToInt(look.z);

#ifdef _DEBUG 
		p_keyboardProcess.id = p_id;
		p_keyboardProcess.move_time = m_Players[id]->m_moveTime;
#endif
		SendPacket(p_id, &p_keyboardProcess);
	}
	break;
	case PACKET_PROTOCOL::C2S_INGAME_UPDATE_SYNC:
	{
		P_S2C_UPDATE_SYNC p_syncUpdate;
		p_syncUpdate.type = PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE;
		p_syncUpdate.size = sizeof(p_syncUpdate);

		p_syncUpdate.playerNum = m_CurrentPlayerNum;

		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			p_syncUpdate.id[i] = i;// static_cast<char>(m_Players[i]->GetId());

			XMFLOAT3 pos = m_Players[i]->GetPosition();
			XMFLOAT3 look = Vector3::Normalize(m_Players[i]->GetLook());
			p_syncUpdate.posX[i] = FloatToInt(pos.x);
			p_syncUpdate.posY[i] = FloatToInt(pos.y);
			p_syncUpdate.posZ[i] = FloatToInt(pos.z);

			p_syncUpdate.lookX[i] = FloatToInt(look.x);
			p_syncUpdate.lookY[i] = FloatToInt(look.y);
			p_syncUpdate.lookZ[i] = FloatToInt(look.z);

			//p_syncUpdate.states[i] = m_Players[i]->GetAnimationSet();
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			p_syncUpdate.existance[i] = m_Players[i]->IsExist();
		}
		SendPacket(p_id, &p_syncUpdate);
	}
	break;
	case PACKET_PROTOCOL::C2S_INGAME_MOUSE_INPUT:
	{
		P_C2S_MOUSE_INPUT p_mouse = *reinterpret_cast<P_C2S_MOUSE_INPUT*>(p_buf);

		BYTE size;
		PACKET_PROTOCOL type;
		MOUSE_INPUT_TYPE InputType;
		short inputNum;
		int xInput[MAX_MOUSE_INPUT];
		int yInput[MAX_MOUSE_INPUT];

		P_S2C_PROCESS_MOUSE p_mouseProcess;
		ZeroMemory(&p_mouseProcess, sizeof(P_S2C_PROCESS_MOUSE));
		p_mouseProcess.size = sizeof(P_S2C_PROCESS_MOUSE);
		p_mouseProcess.type = PACKET_PROTOCOL::S2C_INGAME_MOUSE_INPUT;
		float playerRotateY = 0.0f;
		float cameraRotateY = 0.0f;
		float cameraOffset = 0.0f;

		for (int i = 0; i < p_mouse.inputNum; ++i) {
			if (p_mouse.InputType[i] == MOUSE_INPUT_TYPE::M_LMOVE) {
				float dx = IntToFloat(p_mouse.xInput[i]);

				m_Cameras[p_mouse.id]->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 75);
				cameraRotateY += dx * 75;
				if (m_Players[p_mouse.id]->IsMoving())
				{
					playerRotateY += dx * 150;
					m_Players[p_mouse.id]->Rotate(XMFLOAT3(0, 1, 0), dx * 150);
				}

			}
			else if (p_mouse.InputType[i] == MOUSE_INPUT_TYPE::M_RMOVE) {
				float offset = IntToFloat(p_mouse.yInput[i]);
				cameraOffset += offset * 0.25f;
				m_Cameras[p_mouse.id]->MoveOffset(XMFLOAT3(0, 0, offset * 0.25f));
			}
		}

		p_mouseProcess.cameraRotateY = FloatToInt(cameraRotateY);
		p_mouseProcess.playerRotateY = FloatToInt(playerRotateY);
		p_mouseProcess.cameraOffset = FloatToInt(cameraOffset);
		SendPacket(p_id, &p_mouseProcess);
	}
	break;
	default:
		cout << "Unknown Packet Type from Client[" << p_id << "] Packet Type [" << +p_buf[1] << "]" << endl;
		while (true) {
			// 멈춰
		}
	}
}