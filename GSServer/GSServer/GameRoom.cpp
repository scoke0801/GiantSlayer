#include "stdafx.h"
#include "GameRoom.h"
#include "protocol.h" 
#include "MapObjects.h"
#include "Enemy.h"
#include "Boss.h"
#include "Arrow.h"
#include "PacketProcessor.h"
#include "AnimationObject.h"
#include "MummyLaser.h"
#include "FireBall.h"

#define ChessPuzzleSize 7


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
	if (m_MummyExist[0] || m_MummyExist[1] || m_MummyExist[2]) { 
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			// 미라 보고있으면 데미지
			if (m_Players[i]->IsExist() == false) continue;

			// 미라 보고있으면 데미지
			if (false == m_Players[i]->IsPowerOverWhelm()) {
				if (m_Players[i]->GetPlayerExistingSector() == 3 && m_Players[i]->killit == false)
				{
					if (m_Players[i]->GetPosition().x > 20970 && m_Players[i]->GetPosition().x < 30000) {
						if ((m_Players[i]->GetLook().x > -150.0f && m_Players[i]->GetLook().x < 150.0f) && m_Players[i]->GetLook().z > 0.0f)
						{
							if (m_Players[i]->GetHP() > 0)
								m_Players[i]->SetHP(m_Players[i]->GetHP() - 1);
							else
								m_Players[i]->Death();
						}
					}
				}
			}
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


	// 번개랑 충돌
	for (auto pObstacle : m_ObjectLayers[(int)OBJECT_LAYER::Thunder]) {
		if (pObstacle->IsUsable()) {
			// 모두가 같은 상태
			break;
		}
		if (false == pObstacle->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pObstacle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pObstacle);

				if (m_Players[i]->Attacked(pObstacle))
				{
					m_Players[i]->FixCollision();
				}
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

	for (auto pPlayer : m_Players) {
		if (pPlayer->killit == true)
			break;

		if (pPlayer->IsPowerOverWhelm()) {
			break;
		}
		XMFLOAT3 pos = pPlayer->GetPosition();
		if (pos.x > 1500 && pos.x < 18000 && pos.z > 24000 && pos.z < 30000 && pos.y < -1330) {
			if (pPlayer->GetHP() > 0)
				pPlayer->SetHP(pPlayer->GetHP() - 1);
			else
				pPlayer->Death();
		}
	}


	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser]) {
		// 변수명 변경으로 인한 true/false 반전..
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
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (false == pEnemy->CollisionCheck(m_Players[i])) {
				continue;
			}

			if (false == m_Players[i]->IsCanAttack()) {
				if (false == m_Players[i]->IsAleradyAttack()) {
					pEnemy->ChangeState(ObjectState::Attacked, m_Players[i]);
					m_Players[i]->SetAleradyAttack(true);
				}
			}
			else if (m_Players[i]->Attacked(pEnemy))
			{
				m_Players[i]->FixCollision();
			}

		}
	}

	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser]) {
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pArrow->IsDrawable()) {
			continue;
		}
		for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Mummy])
		{
			if (pArrow->CollisionCheck(pEnemy)) {
				pEnemy->SetHP(pEnemy->GetHP() - 1.0f);

			}
			if (pEnemy->GetHP() == 0)
			{
				CMummy* thisEnemy = reinterpret_cast<CMummy*>(pEnemy);

				thisEnemy->SendDieInfotoFriends();

				DeleteEnemy(thisEnemy);
			}
		}
	}

	for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Mummy]) {
		if (false == pEnemy->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (false == pEnemy->CollisionCheck(m_Players[i])) {
				continue;
			}
			if (m_Players[i]->Attacked(pEnemy))
			{
				m_Players[i]->FixCollision();

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
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (false == pEnemy->CollisionCheck(m_Players[i])) {
				continue;
			}
			m_Players[i]->FixCollision(pEnemy);

			if (false == m_Players[i]->IsCanAttack() && pEnemy->IsCanDamaged()) {
				if (pEnemy->CollisionCheck(m_Players[i]->m_pWeapon)) {
					pEnemy->ChangeState(ObjectState::Attacked, m_Players[i]);
					m_Players[i]->SetAleradyAttack(true);
					break;
				}
			}
			else if (pEnemy->GetStateInfo() == ObjectState::Attack)
			{
				if (pEnemy->CollisionCheck(m_Players[i])) {
					m_Players[i]->Attacked(pEnemy);
					m_Players[i]->FixCollision();
				}
			}
		} 
	}

	for (auto pMummy : m_ObjectLayers[(int)OBJECT_LAYER::Mummy])
	{
		// 왼쪽벽
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
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pArrow->IsUsable()) {
			continue;
		}

		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;
			if (pArrow->CollisionCheck(m_Players[i])) {
				if (m_Players[i]->Attacked(pArrow)) {
					pArrow->SetIsUsable(true);
					RecyleObject(pArrow, (int)OBJECT_LAYER::MonsterArrow);
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
				//pArrow->SetIsUsable(true);
				RecyleObject(pArrow, (int)OBJECT_LAYER::PlayerArrow);
				//cout << "충돌 : 플레이어 화살 - 적\n";
				break;
			}
		}
	}
	for (auto pFireball : m_ObjectLayers[(int)OBJECT_LAYER::FireBall]) {
		// 변수명 변경으로 인한 true/false 반전..
		if (true == pFireball->IsDrawable()) {
			continue;
		}
		for (auto pEnemy : m_ObjectLayers[(int)OBJECT_LAYER::Enemy])
		{
			if (pFireball->CollisionCheck(pEnemy)) {
				pEnemy->ChangeState(ObjectState::Attacked, pFireball);
				pFireball->SetDrawable(true);

				RecyleObject(pFireball, (int)OBJECT_LAYER::FireBall);
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
	    
	for (auto pChessPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle]) {
		if (false == pChessPuzzle->IsInSameSector(m_PlayerExistingSector)) {
			continue;
		}
		for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
			if (m_Players[i]->IsExist() == false) continue;

			if (pChessPuzzle->CollisionCheck(m_Players[i])) {
				m_Players[i]->FixCollision(pChessPuzzle);
				m_isPlayerBoxCollide[i] = true;
				m_Players[i]->UpdateCamera();
				break;
			}
		}
	}

	for (int playerIdx = 0; playerIdx < MAX_ROOM_PLAYER; ++playerIdx) {

		if (false == m_PlayerExistingSector[2]) {
			break;
		}
		if (m_Players[playerIdx]->IsExist() == false) continue;

		// 퍼즐 위치 선정
		XMFLOAT3 lookVec = Vector3::Normalize(m_Players[playerIdx]->GetLook());
		XMFLOAT3 Final_Vec = Vector3::Multifly(lookVec, 5.0f);
		XMFLOAT3 Speed = { 0.3f,0.3f,0.3f };

		// 체스 퍼즐 충돌처리
		for (auto pPlayerChessPuzzle : m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle]) {

			if (pPlayerChessPuzzle->CollisionCheck(m_Players[playerIdx]))
			{
				m_Players[playerIdx]->FixCollision(pPlayerChessPuzzle);
				m_isPlayerBoxCollide[playerIdx] = true;
				m_Players[playerIdx]->UpdateCamera();

				break;
			}
		}

		// npc 자체랑 충돌처리
		if (
			((m_Npc->GetPosition().x + 215.0f > m_Players[playerIdx]->GetPosition().x)
				&& (m_Npc->GetPosition().x - 215.0f < m_Players[playerIdx]->GetPosition().x))
			&&
			((m_Npc->GetPosition().z + 225.0f > m_Players[playerIdx]->GetPosition().z)
				&& (m_Npc->GetPosition().z - 225.0f < m_Players[playerIdx]->GetPosition().z))
			)
		{
			m_Npc_Event = true;
		}
		else
		{
			m_Npc_Event = false;
		}

		// 체스 그 자체랑 충돌처리
		for (int i = 0; i < ChessType::Count; i++)
		{
			if (
				(
					(m_Chess[i]->GetPosition().x + 215.0f > m_Players[playerIdx]->GetPosition().x)
					&& (m_Chess[i]->GetPosition().x - 215.0f < m_Players[playerIdx]->GetPosition().x)
					)
				&&
				(
					(m_Chess[i]->GetPosition().z + 225.0f > m_Players[playerIdx]->GetPosition().z)
					&& (m_Chess[i]->GetPosition().z - 225.0f < m_Players[playerIdx]->GetPosition().z)
					)

				)
			{
				m_ChessCollide_Check[i] = true;
				m_ChessChangeFlag = true;
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
				m_Players[playerIdx]->Box_Pull(TRUE);
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
				m_Players[playerIdx]->Box_Pull(FALSE);
			}
		}

		// 1 킹 2 나이트 3 폰 4 룩 // 퍼즐 체크 
		if ((m_Chess[King]->GetPosition().x > 18320.0f && m_Chess[King]->GetPosition().x < 18720.0f)
			&& (m_Chess[King]->GetPosition().z > 17150.0f && m_Chess[King]->GetPosition().z < 17550.0f))
		{
			m_ChessPlate_Check[King] = true;
		}
		if ((m_Chess[Knight]->GetPosition().x > 18920.0f && m_Chess[Knight]->GetPosition().x < 19320.0f)
			&& (m_Chess[Knight]->GetPosition().z > 15950.0f && m_Chess[Knight]->GetPosition().z < 16350.0f))
		{
			m_ChessPlate_Check[Knight] = true;
		}
		if ((m_Chess[Pawn]->GetPosition().x > 17720.0f && m_Chess[Pawn]->GetPosition().x < 18120.0f)
			&& (m_Chess[Pawn]->GetPosition().z > 16550.0f && m_Chess[Pawn]->GetPosition().z < 16950.0f))
		{
			m_ChessPlate_Check[Pawn] = true;
		}

		if ((m_Chess[Rook]->GetPosition().x > 17040.0f && m_Chess[Rook]->GetPosition().x < 17520.0f)
			&& (m_Chess[Rook]->GetPosition().z > 15950.0f && m_Chess[Rook]->GetPosition().z < 16350.0f))
		{
			m_ChessPlate_Check[Rook] = true;
		}

		if (m_ChessPlate_Check[King] && m_ChessPlate_Check[Knight] && m_ChessPlate_Check[Pawn] && m_ChessPlate_Check[Rook])
		{
			//m_SoundManager->PlayEffect(Sound_Name::EFFECT_Chess_Success);
			//CDoorWall* p = reinterpret_cast<CDoorWall*>(m_ObjectLayers[(int)OBJECT_LAYER::Obstacle][m_DoorIdx + 1]);
			//p->OpenDoor();
		}
	}

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if ((int)SECTOR_POSITION::SECTOR_3!= m_Players[i]->GetPlayerExistingSector()) {
			continue;
		}
		XMFLOAT3 plPos = m_Players[i]->GetPosition();

		// 입구 계단
		if ((plPos.x > 17553.5 && plPos.x < 18283.8) &&
			(plPos.z > 17907.7 && plPos.z < 18509.6))
		{
			float min = -1980;
			float max = -1760;

			float t = (plPos.z / 18509.6);

			float y = Lerp(max, min, Rate(17907.7, 18509.6, plPos.z));

			m_Players[i]->SetPosition(XMFLOAT3(plPos.x, y, plPos.z));
			m_Players[i]->UpdateCamera();
		}
		// 출구 계단
		if ((plPos.x > 17527.7 && plPos.x < 18319.5) &&
			(plPos.z > 13328 && plPos.z < 13887.7))
		{
			float min = -1950;
			float max = -1760;

			float t = (plPos.z / 13887.7);

			float y = Lerp(min, max, Rate(13328, 13887.7, plPos.z)); 
			m_Players[i]->SetPosition(XMFLOAT3(plPos.x, y, plPos.z));
			m_Players[i]->UpdateCamera();
		}

		// 퍼즐
		if ((plPos.x > 16450.0f && plPos.x < 19450.0f) &&
			(plPos.z > 13900.0f && plPos.z < 17950.0f))
		{
			m_Players[i]->SetPosition(XMFLOAT3(plPos.x, -1760.0f, plPos.z));
			m_Players[i]->UpdateCamera();
		}
	}
}
void CGameRoom::InitAll()
{  
	InitPlayers();
	InitCameras();
	InitMonsters();
	InitNPCs();
	InitObstacle();
	InitArrows();
	InitFireBall();
	InitMummyLaser();
	InitThunderColliders();
	BuildBlockingRegionOnMap();

	for (int i = 0; i < m_ObjectLayers.size(); ++i) {
		for (auto pObject : m_ObjectLayers[i]) { 
			pObject->ConnectRoom(this);
		}
	}
	

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
		m_Players[i]->SetWeapon(PlayerWeaponType::Sword);
		m_Players[i]->SetWeaponPointer(); 
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
	{
		// boss
		XMFLOAT3 scale = { 120.0f, 120.0f, 120.0f };
		CBoss* pBoss = new CBoss();
		pBoss->SetPosition({ 17166 * MAP_SCALE_SIZE, -6070, 17166 * MAP_SCALE_SIZE });
		pBoss->FixPositionByTerrain(g_Heights);
		pBoss->Scale(120, 120, 120);
		pBoss->Rotate({ 0,1,0 }, 180);
		pBoss->ConnectPlayer(m_Players, MAX_ROOM_PLAYER);

		XMFLOAT3 centerPos = pBoss->GetPosition();
		XMFLOAT3 scopeSize = { 4100 * 2, 0, 4100 * 2 };
		pBoss->SetActivityScope({ scopeSize.x, 0, scopeSize.z }, { centerPos });
		pBoss->SetSightBoundingBox({ scopeSize.x / scale.x, 15, scopeSize.z / scale.z });
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(pBoss);
	}

	{ 
		// Mummy
		XMFLOAT3 scale = { 600.0f,600.0f,600.0f };
		CAnimationObject* pMummyModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
			"resources/FbxExported/Mummy.bin", true); 

		m_Mummy[0] = new CMummy();
		m_Mummy[0]->Scale(scale.x, scale.y, scale.z, true);
		m_Mummy[0]->SetChild(pMummyModel, true); 
		 
		m_Mummy[0]->SetPosition({ 18900.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 18900.0f, 6250.0f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[0]->SetActivityScope({ 1200.0f, 0, 250.0f }, { 18900.f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 18900.0f, 6250.0f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[0]->SetEnemyAttackType(EnemyAttackType::Mummy1);
		m_Mummy[0]->SetMummyDie(0);
		m_Mummy[0]->ConnectPlayer(m_Players, 5);

		m_Mummy[0]->SetSightBoundingBox({ 5020.0f * 0.75f / scale.x, 3, 2250 * 0.75f / scale.z });
		m_Mummy[0]->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0, 0.25f,0 }, XMFLOAT3(1020.0f * 0.25f / scale.x, 0.5f, 225.0f / scale.z)));

		m_ObjectLayers[(int)OBJECT_LAYER::Mummy].push_back(reinterpret_cast<CGameObject*>(std::move(m_Mummy[0])));
		m_Mummy[0]->AddFriends(m_Mummy[0]);

		scale = { 600.0f,600.0f,600.0f };
		pMummyModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
			"resources/FbxExported/Mummy.bin", true);

		m_Mummy[1] = new CMummy();
		m_Mummy[1]->Scale(scale.x, scale.y, scale.z);
		m_Mummy[1]->SetChild(pMummyModel, true); 

		m_Mummy[1]->SetPosition({ 16900.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 16900.0f, 6250), 6250 * MAP_SCALE_SIZE });
		m_Mummy[1]->SetActivityScope({ 1200.0f, 0, 250.0f }, { 16900.f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 16900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[1]->SetEnemyAttackType(EnemyAttackType::Mummy2);
		m_Mummy[1]->SetMummyDie(0);
		m_Mummy[1]->ConnectPlayer(m_Players, 5);

		m_Mummy[1]->SetSightBoundingBox({ 5020.0f * 0.75f / scale.x, 3, 2250 * 0.75f / scale.z });
		m_Mummy[1]->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0, 0.25f,0 }, XMFLOAT3(1020.0f * 0.25f / scale.x, 0.5f, 225.0f / scale.z)));

		m_ObjectLayers[(int)OBJECT_LAYER::Mummy].push_back(reinterpret_cast<CGameObject*>(std::move(m_Mummy[1])));
		m_Mummy[1]->AddFriends(m_Mummy[1]);

		scale = { 600.0f,600.0f,600.0f };

		pMummyModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
			"resources/FbxExported/Mummy.bin", true);

		m_Mummy[2] = new CMummy();
		m_Mummy[2]->Scale(scale.x, scale.y, scale.z);
		m_Mummy[2]->SetChild(pMummyModel, true); 

		m_Mummy[2]->SetPosition({ 14900.0f * MAP_SCALE_SIZE,  GetDetailHeight(g_Heights, 14900.0f, 6250), 6250 * MAP_SCALE_SIZE });
		m_Mummy[2]->SetActivityScope({ 1200.0f, 0, 250.0f }, { 14900.f * MAP_SCALE_SIZE,  GetDetailHeight(g_Heights, 14900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
		m_Mummy[2]->SetEnemyAttackType(EnemyAttackType::Mummy3);
		m_Mummy[2]->SetMummyDie(0);
		m_Mummy[2]->ConnectPlayer(m_Players, 5);
		m_Mummy[2]->SetSightBoundingBox({ 5020.0f * 0.75f / scale.x, 3, 2250 * 0.75f / scale.z });

		m_Mummy[2]->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0, 0.25f,0 }, XMFLOAT3(1020.0f * 0.25f / scale.x, 0.5f, 225.0f / scale.z)));

		m_ObjectLayers[(int)OBJECT_LAYER::Mummy].push_back(reinterpret_cast<CGameObject*>(std::move(m_Mummy[2])));
		m_Mummy[2]->AddFriends(m_Mummy[2]);
	}

	CEnemy* pEnemy;
	CAnimationObject* pMonsterModel;
	XMFLOAT3 scale = { 300.0f,300.0f,300.0f };
	{	// Monster Area1
		for (int i = 0; i < 3; ++i) {
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

		pMonsterModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
			"resources/FbxExported/Skeleton_Archer.bin", true);
		pEnemy = new CRangedEnemy();
		pEnemy->Scale(scale.x, scale.y, scale.z);
		pEnemy->SetChild(pMonsterModel, true); 
		pEnemy->SetPosition({ 2005.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 2005.0f * MAP_SCALE_SIZE, 11650.0f * MAP_SCALE_SIZE), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
		pEnemy->ConnectPlayer(m_Players, m_CurrentPlayerNum); 
		pEnemy->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0, 0,0 }, XMFLOAT3(0.5f, 0.75f, 0.4f)));
		pEnemy->SetSightBoundingBox({ 1825 * 0.75f / scale.x, 3, 3050 * 0.75f / scale.z }); 
		m_ObjectLayers[(int)OBJECT_LAYER::Enemy].push_back(reinterpret_cast<CGameObject*>(std::move(pEnemy)));

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
}

void CGameRoom::InitArrows()
{
	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->Scale(25.0f, 25.0f, 25.0f);
		pArrow->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		pArrow->SetIsPlayerArrow(true);
		m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow].push_back(pArrow);
	}

	for (int i = 0; i < 10; ++i) {
		CArrow* pArrow = new CArrow();
		pArrow->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pArrow->SetTargetPosition({ 500.0f, 100.0f, 5000.0f });
		pArrow->Scale(100.0f, 100.0f, 50.0f);
		pArrow->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 5), XMFLOAT3(0.25f, 0.25f, 7.5f)));
		pArrow->SetIsPlayerArrow(false);
		m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow].push_back(pArrow);
	}
}

void CGameRoom::InitNPCs()
{
	CAnimationObject* pNpcModel = CAnimationObject::LoadGeometryAndAnimationFromFile(
		"resources/FbxExported/Elf_Mesh.bin", true);

	XMFLOAT3 scale = { 300.0f,300.0f,300.0f };
	m_Npc = new CNpc();
	m_Npc->Scale(scale.x / 1.5f, scale.y / 1.5f, scale.z / 1.5f);
	m_Npc->Rotate(XMFLOAT3(0, 1, 0), 180);
	m_Npc->SetChild(pNpcModel, true); 
	m_Npc->SetPosition({ 2305.0f * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 2305.0f, 4650.0f), 4650.0f * MAP_SCALE_SIZE });
	//pNpc->SetActivityScope({ 1825, 0, 3050 }, { 2005.0f * MAP_SCALE_SIZE, m_Terrain->GetDetailHeight(2005.0f, 11650.0f), 11650.0f * MAP_SCALE_SIZE });
	m_Npc->ConnectPlayer(m_Players, 5);
	m_Npc->SetExistingSector(SECTOR_POSITION::SECTOR_1);
	m_ObjectLayers[(int)OBJECT_LAYER::Npc].push_back(reinterpret_cast<CGameObject*>(std::move(m_Npc)));
}

void CGameRoom::InitThunderColliders()
{
	for (int i = 0; i < 7; ++i) {
		CGameObject* pObject = new CGameObject();
	 	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(75, 750, 75)));
		pObject->SetPosition({ 500, 50, 500 });
		pObject->SetATK(20);
		pObject->SetIsUsable(true);
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_5);
		m_ObjectLayers[(int)OBJECT_LAYER::Thunder].push_back(pObject);
	}
}

void CGameRoom::InitMummyLaser()
{
	for (int i = 0; i < 3; i++)
	{
		m_MummyLaser[i] = new CMummyLaser(); 
		m_MummyLaser[i]->SetLaserType(Laser_TYPE::Laser1); 
		m_MummyLaser[i]->Scale(100.0f, 100.0f, 1000.0f);
		m_MummyLaser[i]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 1), XMFLOAT3(0.5f, 0.5f, 1.0f)));
		m_MummyLaser[i]->SetATK(20);
		m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser].push_back(m_MummyLaser[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		m_MummyLaser2[i] = new CMummyLaser();
		m_MummyLaser2[i]->SetLaserType(Laser_TYPE::Laser2); 
		m_MummyLaser2[i]->Scale(100.0f, 100.0f, 1000.0f);
		m_MummyLaser2[i]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 1), XMFLOAT3(0.5f, 0.5f, 1.0f)));
		m_MummyLaser2[i]->SetATK(20);
		m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser].push_back(m_MummyLaser2[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		m_MummyLaser3[i] = new CMummyLaser(); 
		m_MummyLaser3[i]->SetLaserType(Laser_TYPE::Laser3); 
		m_MummyLaser3[i]->Scale(100.0f, 100.0f, 1000.0f);
		m_MummyLaser3[i]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 1), XMFLOAT3(0.5f, 0.5f, 1.0f)));
		m_MummyLaser3[i]->SetATK(20);
		m_ObjectLayers[(int)OBJECT_LAYER::Mummylaser].push_back(m_MummyLaser3[i]);
	}
}

void CGameRoom::InitFireBall()
{
	// FireBall
	for (int i = 0; i < 5; ++i) {
		CFireBall* pFireb = new CFireBall(); 
		pFireb->SetPosition({ 500.0f,  100.0f, 1500.0f });
		pFireb->SetTargetPosition({ 500.0f, 100.0f, 5000.0f }); 
		pFireb->Scale(40.0f, 40.0f, 40.0f);
		pFireb->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2.0f, 2.0f, 2.0f)));
		m_ObjectLayers[(int)OBJECT_LAYER::FireBall].push_back(pFireb);
	}
}
  
void CGameRoom::InitObstacle()
{
	// Bridge --------------------------------------------------------------------
	CBridge* pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_1);
	pBridge->SetPositionPlus({ 8000.0f * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_2);
	pBridge->SetPositionPlus({ (10000.0f - 680) * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_3);
	pBridge->SetPositionPlus({ (9000.0f - 340) * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_1);
	pBridge->SetPositionPlus({ (11000.0f - 680 - 340) * MAP_SCALE_SIZE,  -1301.0f,  17400 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_2);
	pBridge->SetPositionPlus({ 8000.0f * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_3);
	pBridge->SetPositionPlus({ (10000.0f - 680) * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_1);
	pBridge->SetPositionPlus({ (9000.0f - 340) * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);

	pBridge = new CBridge(OBJECT_ID::BRIDEGE_SEC2_SEC3_2);
	pBridge->SetPositionPlus({ (11000.0f - 680 - 340) * MAP_SCALE_SIZE,  -1301.0f,  18600 * MAP_SCALE_SIZE });
	pBridge->Rotate({ 0, 1, 0 }, 90);
	pBridge->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::Bridge].push_back(pBridge);
	 
	///////////////////////////////////////////////////////////////////////////////// 

	// PUZZLE----------------------------------------------------------------------
	

	for (int i = 0; i < ChessPuzzleSize; i++)
	{
		for (int j = 0; j < ChessPuzzleSize; j++)
		{
			m_ChessPlate[i][j].x = 16855 + 355 * i;
			m_ChessPlate[i][j].z = 14417 + 450 * j;
			m_ChessPlate[i][j].y = -1750.0f;
		}
	}
	CPlate* pPuzzlePlate = new CPlate(OBJECT_ID::PUZZLE_1);

	pPuzzlePlate->SetPosition({ 11130.0f * MAP_SCALE_SIZE, -2000.0f,(2000.0f + 8000.0f) * MAP_SCALE_SIZE });

	m_ObjectLayers[(int)OBJECT_LAYER::Puzzle].push_back(pPuzzlePlate);

	CGameObject* pObject  = new CPuzzle(OBJECT_ID::PUZZLE_1);
	CGameObject* pObject2 = new CPuzzle(OBJECT_ID::PUZZLE_1);

	// 고정된 체스말
	pObject = new CGameObject(); 
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[1][0]); 
	pObject->Scale(300, 300, 300); 
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);

	pObject = new CGameObject(); 
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[0][2]); 
	pObject->Scale(300, 300, 300); 
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);

	pObject = new CGameObject(); 
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[3][1]); 
	pObject->Scale(300, 300, 300); 
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);

	pObject = new CGameObject(); 
	pObject->Rotate({ 1,0,0 }, -90);
	pObject->SetPosition(m_ChessPlate[5][2]); 
	pObject->Scale(300, 300, 300); 
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::ChessPuzzle].push_back(pObject);


	// 플레이어가 움직일수 있는 퍼즐
	m_Chess[King] = new CGameObject(); 
	m_Chess[King]->Rotate({ 1,0,0 }, -90);
	m_Chess[King]->SetPosition(m_ChessPlate[0][6]); 
	m_Chess[King]->SetChess(Chess_Type::King); 
	m_Chess[King]->Scale(300, 300, 300); 
	m_Chess[King]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[King]->SetExistingSector(SECTOR_POSITION::SECTOR_3);

	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[King]);

	m_Chess[Knight] = new CGameObject(); 
	m_Chess[Knight]->Rotate({ 1,0,0 }, -90);
	m_Chess[Knight]->SetPosition(m_ChessPlate[6][3]); 
	m_Chess[Knight]->SetChess(Chess_Type::Knight); 
	m_Chess[Knight]->Scale(300, 300, 300); 
	m_Chess[Knight]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[Knight]->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[Knight]);

	m_Chess[Pawn] = new CGameObject(); 
	m_Chess[Pawn]->Rotate({ 1,0,0 }, -90);
	m_Chess[Pawn]->SetPosition(m_ChessPlate[3][5]); 
	m_Chess[Pawn]->SetChess(Chess_Type::Pawn);
	m_Chess[Pawn]->Scale(300, 300, 300);
	m_Chess[Pawn]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[Pawn]->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[Pawn]);

	m_Chess[Rook] = new CGameObject(); 
	m_Chess[Rook]->Rotate({ 1,0,0 }, -90);
	m_Chess[Rook]->SetPosition(m_ChessPlate[6][6]); 
	m_Chess[Rook]->SetChess(Chess_Type::Rook); 
	m_Chess[Rook]->Scale(300, 300, 300); 
	m_Chess[Rook]->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0.7), XMFLOAT3(0.25, 0.35, 0.75)));
	m_Chess[Rook]->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::PlayerChessPuzzle].push_back(m_Chess[Rook]);

	 
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// MirrorBox----------------------------------------------------------------------
	for (int i = 0; i < 3; i++)
	{
		pObject = new CGameObject();
		  
		pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);

		pObject->SetPosition({ float(19200 - (2200 * i)) * MAP_SCALE_SIZE , -2300, 200 * MAP_SCALE_SIZE });
		  
		pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 2500.0f * 0.5f * MAP_SCALE_SIZE, 2600.0f * 0.5f, 1.0f * 0.5f }));

		m_ObjectLayers[(int)OBJECT_LAYER::MirrorBox].push_back(pObject);
	}
	
// DoorWall----------------------------------------------------------------------
	m_DoorStartIndex = m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].size();
	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC1);
	pObject->SetPosition({ 0,0, 7500 * MAP_SCALE_SIZE });
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC2);
	pObject->SetPosition({ 10300 * MAP_SCALE_SIZE, -2000, 7500 * MAP_SCALE_SIZE });
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_3);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC3);
	pObject->SetPosition({ 13500 * MAP_SCALE_SIZE, -3500, 0 });
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC4);
	pObject->SetPosition({ 14000 * MAP_SCALE_SIZE,-4500, 8000 * MAP_SCALE_SIZE });
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_5);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CDoorWall(OBJECT_ID::DOOR_WALL_SEC5);
	pObject->SetPosition({ 14000 * MAP_SCALE_SIZE, -7050, 13650 * MAP_SCALE_SIZE });
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_5);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->Rotate({ 0,1,0 }, 90);
	pObject->SetPosition({ 13750 * MAP_SCALE_SIZE, -3500 + 1250, 4750 * MAP_SCALE_SIZE });
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * MAP_SCALE_SIZE * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);

	pObject = new CGameObject();
	pObject->Rotate({ 0,1,0 }, 90);
	pObject->SetPosition({ 13750 * MAP_SCALE_SIZE, -3500 + 1250, 6250 * MAP_SCALE_SIZE });
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1500 * MAP_SCALE_SIZE * 0.5f, 2500 * 0.5f, 500 * 0.5f)));
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_4);
	m_ObjectLayers[(int)OBJECT_LAYER::Obstacle].push_back(pObject);
	////////////////////////////////////////////////////////////////////////////////

	// Sign-------------------------------------------------------------------------
	pObject = new CSign(OBJECT_ID::SIGN_SCROLL);
	pObject->SetPosition(g_ObjectPositions[OBJECT_ID::SIGN_SCROLL]);
	pObject->SetExistingSector(SECTOR_POSITION::SECTOR_1); 
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
	pObject->SetPosition({ 0,-2000,10000 * MAP_SCALE_SIZE });
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0),
		XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * MAP_SCALE_SIZE * 0.5f)));
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(100 * 0.5f, 10000 * 0.5f, 20000 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 19950 * MAP_SCALE_SIZE,-2000,10000 * MAP_SCALE_SIZE }); 
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(20000 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000 * MAP_SCALE_SIZE,-2000,00 });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(20000 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 10000 * MAP_SCALE_SIZE,-2000,19950 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Forest to DryDesrt 아래 방향 벽  
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(9600 * 0.5f * MAP_SCALE_SIZE, 800 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 4800 * MAP_SCALE_SIZE,-1000, 15900 * MAP_SCALE_SIZE });
	m_BlockingPlateToPreviousSector[0] = (std::move(pObject));

	// Forest to Desert 왼쪽 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 15200 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 10000 * MAP_SCALE_SIZE,-2000, 7600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Forest 지역 내 못가는 지형 
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2000 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 7000 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 5000 * MAP_SCALE_SIZE, -2000, 11100 * MAP_SCALE_SIZE });
	pObject->UpdateColliders();
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// Desrt to DryDesrt and Rock 왼쪽 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(400 * 0.5f, 10000 * 0.5f, 12800 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 13800 * MAP_SCALE_SIZE, -2000, 13600 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// boss 지역 중간 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 15600 * MAP_SCALE_SIZE,-2000, 10800 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(800 * 0.5f, 10000 * 0.5f, 5600 * 0.5f * MAP_SCALE_SIZE)));
	pObject->SetPosition({ 18000 * MAP_SCALE_SIZE,-2000, 10800 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	// 사막 지역 가로 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(4000 * 0.5f * MAP_SCALE_SIZE, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 11600 * MAP_SCALE_SIZE,-2000, 15600 * MAP_SCALE_SIZE });
	m_BlockingPlateToPreviousSector[1] = (std::move(pObject));

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(4000 * 0.5f * MAP_SCALE_SIZE, 1000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 11600 * MAP_SCALE_SIZE ,-3000, 3600 * MAP_SCALE_SIZE });
	m_BlockingPlateToPreviousSector[2] = (std::move(pObject));

	// 보스 지역 입구 가로 벽
	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2400 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 14800 * MAP_SCALE_SIZE,-2000, 8000 * MAP_SCALE_SIZE });
	m_ObjectLayers[(int)OBJECT_LAYER::TerrainBoundary].push_back(pObject);

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3(0, 0, 0), XMFLOAT3(2400 * 0.5f * MAP_SCALE_SIZE, 10000 * 0.5f, 100 * 0.5f)));
	pObject->SetPosition({ 18800 * MAP_SCALE_SIZE, -2000, 8000 * MAP_SCALE_SIZE });
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
	ZeroMemory(&p_syncUpdate, sizeof(p_syncUpdate));
	p_syncUpdate.type = PACKET_PROTOCOL::S2C_INGAME_UPDATE_PLAYERS_STATE;
	p_syncUpdate.size = sizeof(p_syncUpdate);

	p_syncUpdate.playerNum = m_CurrentPlayerNum;

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		p_syncUpdate.id[i] = i;// static_cast<char>(m_Players[i]->GetId());
		p_syncUpdate.hp[i] = m_Players[i]->GetHP();
		p_syncUpdate.sp[i] = m_Players[i]->GetSP();
		//p_syncUpdate.Sp[i] = m_Players[i]->GetSP();
		XMFLOAT3 pos = m_Players[i]->GetPosition();
		XMFLOAT3 look = Vector3::Normalize(m_Players[i]->GetLook());

		if (i == 0) {
			//DisplayVector3(look);
		}
		p_syncUpdate.posX[i] = FloatToInt(pos.x);
		p_syncUpdate.posY[i] = FloatToInt(pos.y);
		p_syncUpdate.posZ[i] = FloatToInt(pos.z);

		p_syncUpdate.lookX[i] = FloatToInt(look.x);
		p_syncUpdate.lookY[i] = FloatToInt(look.y);
		p_syncUpdate.lookZ[i] = FloatToInt(look.z);

		p_syncUpdate.weaponType[i] = m_Players[i]->GetWeaponType();
		p_syncUpdate.states[i] = m_Players[i]->GetAnimationSet();  

		if (p_syncUpdate.weaponType[i] == PlayerWeaponType::Bow) {
			p_syncUpdate.animationPause[i] = m_Players[i]->IsAnimationPaused();
			p_syncUpdate.pullString[i] = m_Players[i]->IsOnPullstring();
		}
		
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

	// 0 : boss
	for (int i = 1; i < m_ObjectLayers[(int)OBJECT_LAYER::Enemy].size(); ++i) {
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

		p_monsterUpdate[i].state = reinterpret_cast<CEnemy*>(mon)->GetAnimationSet();
		p_monsterUpdate[i].alive = mon->IsUsable();
	}
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}
		for (int j = 0; j < m_ObjectLayers[(int)OBJECT_LAYER::Enemy].size(); ++j) { 
			if (m_ObjectLayers[(int)OBJECT_LAYER::Enemy][j]->IsInNearSector((SECTOR_POSITION)m_Players[i]->GetPlayerExistingSector())) {
				SendPacket(m_Clients[i]->id, &p_monsterUpdate[j]);
			}
		}
	}
}

void CGameRoom::SendBossActPacket()
{
	if (m_CurrentPlayerNum == 0) return;

	P_S2C_MONSTERS_UPDATE_SYNC p_monsterUpdate;
	ZeroMemory(&p_monsterUpdate, sizeof(p_monsterUpdate));

	{
		auto mon = m_ObjectLayers[(int)OBJECT_LAYER::Enemy][0];

		XMFLOAT3 pos = mon->GetPosition();
		XMFLOAT3 look = Vector3::Normalize(mon->GetLook());

		p_monsterUpdate.type = PACKET_PROTOCOL::S2C_BOSS_ACT;
		p_monsterUpdate.size = sizeof(p_monsterUpdate);
		p_monsterUpdate.id = 0;

		p_monsterUpdate.posX = FloatToInt(pos.x);
		p_monsterUpdate.posY = FloatToInt(pos.y);
		p_monsterUpdate.posZ = FloatToInt(pos.z);

		p_monsterUpdate.lookX = FloatToInt(look.x);
		p_monsterUpdate.lookY = FloatToInt(look.y);
		p_monsterUpdate.lookZ = FloatToInt(look.z);

		//cout << (int)reinterpret_cast<CEnemy*>(mon)->GetAnimationSet() << "\n";
		p_monsterUpdate.state = reinterpret_cast<CEnemy*>(mon)->GetAnimationSet();
		p_monsterUpdate.alive = mon->IsUsable();
	}

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}
		for (int j = 0; j < m_ObjectLayers[(int)OBJECT_LAYER::Enemy].size(); ++j) {
			if (m_ObjectLayers[(int)OBJECT_LAYER::Enemy][j]->IsInNearSector((SECTOR_POSITION)m_Players[i]->GetPlayerExistingSector())) {
				SendPacket(m_Clients[i]->id, &p_monsterUpdate);
			}
		}
	}
}

void CGameRoom::SendMummyActPacket()
{
	P_S2C_MUMMY_UPDATE_SYNC packet;

	ZeroMemory(&packet, sizeof(packet));
	packet.size = sizeof(packet);
	packet.type = PACKET_PROTOCOL::S2C_INGAME_MUMMY_ACT;
	for (int i = 0; i < 3; ++i) {
		if (m_MummyExist[i]) {
			XMFLOAT3 pos = m_Mummy[i]->GetPosition();
			XMFLOAT3 look = m_Mummy[i]->GetLook();
			packet.exist[i] = true;
			packet.posX[i] = FloatToInt(pos.x);
			packet.posY[i] = FloatToInt(pos.y);
			packet.posZ[i] = FloatToInt(pos.z);

			packet.lookX[i] = FloatToInt(look.x);
			packet.lookY[i] = FloatToInt(look.y);
			packet.lookZ[i] = FloatToInt(look.z);
			packet.state[i] = m_Mummy[i]->GetAnimationSet();
		}
		else {
			packet.exist[i] = false;
		}
	}

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue; 
		}

		SECTOR_POSITION sector = (SECTOR_POSITION)m_Players[i]->GetPlayerExistingSector();
		if (sector == SECTOR_POSITION::SECTOR_1 || sector == SECTOR_POSITION::SECTOR_2) {
			continue;
		}
		SendPacket(m_Clients[i]->id, &packet);
	} 
}

void CGameRoom::SendMonsterArrowActPacket()
{
	vector<P_S2C_MONSTER_ARROW_UPDATE_SYNC> packets;
	
	int idx = 0;
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow]) {
		if (pArrow->IsUsable()) {
			++idx;
			continue;
		}

		P_S2C_MONSTER_ARROW_UPDATE_SYNC packet;
		ZeroMemory(&packet, sizeof(packet));
		packet.size = sizeof(packet);
		packet.type = PACKET_PROTOCOL::SC2_INGAME_MONSTER_ARROW_ACT;
		packet.id = idx;

		XMFLOAT3 pos = pArrow->GetPosition();
		XMFLOAT3 look = pArrow->GetLook();
		packet.posX = FloatToInt(pos.x);
		packet.posY = FloatToInt(pos.y);
		packet.posZ = FloatToInt(pos.z);

		packet.lookX = FloatToInt(look.x);
		packet.lookY = FloatToInt(look.y);
		packet.lookZ = FloatToInt(look.z);
		packets.push_back(packet);
		++idx;
	}

	if (packets.empty()) return;
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}
		
		for (auto p : packets) {
			SendPacket(m_Clients[i]->id, &p);
		}
	}
}

void CGameRoom::SendPlayerArrowActPacket()
{
	vector<P_S2C_PLAYER_ARROW_UPDATE_SYNC> packets;

	int idx = 0;
	for (auto pArrow : m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow]) {
		if (pArrow->IsUsable()) {
			++idx;
			continue;
		}

		P_S2C_PLAYER_ARROW_UPDATE_SYNC packet;
		ZeroMemory(&packet, sizeof(packet));
		packet.size = sizeof(packet);
		packet.type = PACKET_PROTOCOL::S2C_INGAME_PLAYER_ARROW_ACT;
		packet.id = idx;

		XMFLOAT3 pos = pArrow->GetPosition();
		XMFLOAT3 look = pArrow->GetLook();
		packet.posX = FloatToInt(pos.x);
		packet.posY = FloatToInt(pos.y);
		packet.posZ = FloatToInt(pos.z);

		packet.lookX = FloatToInt(look.x);
		packet.lookY = FloatToInt(look.y);
		packet.lookZ = FloatToInt(look.z);
		 
		packets.push_back(packet);
		++idx;
	}

	if (packets.empty()) return;
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}

		for (auto p : packets) {
			SendPacket(m_Clients[i]->id, &p);
		}
	}
	
}

void CGameRoom::SendFireballActPacket()
{
	vector<P_S2C_FIREBALL_UPDATE_SYNC> packets;

	int idx = 0;
	for (auto pFireBall : m_ObjectLayers[(int)OBJECT_LAYER::FireBall]) {
		if (pFireBall->IsDrawable()) {
			++idx;
			continue;
		}
		P_S2C_FIREBALL_UPDATE_SYNC packet;
		ZeroMemory(&packet, sizeof(packet));
		packet.size = sizeof(packet);
		packet.type = PACKET_PROTOCOL::S2C_INGAME_FIREBALL_ACT;
		packet.id = idx;
		 
		XMFLOAT3 pos = pFireBall->GetPosition();
		XMFLOAT3 look = pFireBall->GetLook();
		packet.posX = FloatToInt(pos.x);
		packet.posY = FloatToInt(pos.y);
		packet.posZ = FloatToInt(pos.z); 

		packets.push_back(packet);
		++idx;
	}

	if (packets.empty()) return;
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}

		for (auto p : packets) {
			SendPacket(m_Clients[i]->id, &p);
		}
	}
}

void CGameRoom::SendLaserActPacket()
{
	vector<P_S2C_LASER_UPDATE_SYNC> packets;
	packets.reserve(3);
	{
		int count = 0;
		P_S2C_LASER_UPDATE_SYNC packet;
		ZeroMemory(&packet, sizeof(packet));
		packet.size = sizeof(packet);
		packet.type = PACKET_PROTOCOL::S2C_INGAME_LASER_ACT;
		packet.id = 0;
		int idx = 0;

		for (auto pLaser : m_MummyLaser) {
			if (false == pLaser->IsDrawable()) {
				XMFLOAT3 pos = pLaser->GetPosition();
				XMFLOAT3 look = pLaser->GetLook();
				packet.posX[idx] = FloatToInt(pos.x);
				packet.posY[idx] = FloatToInt(pos.y);
				packet.posZ[idx] = FloatToInt(pos.z);
				packet.exist[idx] = true;
				++count;
			}
			else {
				packet.exist[idx] = false;
			}
			++idx;
		}
		if (count != 0) {
			packets.push_back(packet);
		}
	}
	{
		int count = 0;
		P_S2C_LASER_UPDATE_SYNC packet;
		ZeroMemory(&packet, sizeof(packet));
		packet.size = sizeof(packet);
		packet.type = PACKET_PROTOCOL::S2C_INGAME_LASER_ACT;
		packet.id = 1;
		int idx = 0;

		for (auto pLaser : m_MummyLaser2) {
			if (false == pLaser->IsDrawable()) {
				XMFLOAT3 pos = pLaser->GetPosition();
				XMFLOAT3 look = pLaser->GetLook();
				packet.posX[idx] = FloatToInt(pos.x);
				packet.posY[idx] = FloatToInt(pos.y);
				packet.posZ[idx] = FloatToInt(pos.z);
				packet.exist[idx] = true;
				++count;
			}
			else {
				packet.exist[idx] = false;
			}
			++idx;
		}
		if (count != 0) {
			packets.push_back(packet);
		}
	}
	{
		int count = 0;
		P_S2C_LASER_UPDATE_SYNC packet;
		ZeroMemory(&packet, sizeof(packet));
		packet.size = sizeof(packet);
		packet.type = PACKET_PROTOCOL::S2C_INGAME_LASER_ACT;
		packet.id = 2;
		int idx = 0;

		for (auto pLaser : m_MummyLaser3) {
			if (false == pLaser->IsDrawable()) {
				XMFLOAT3 pos = pLaser->GetPosition();
				XMFLOAT3 look = pLaser->GetLook();
				packet.posX[idx] = FloatToInt(pos.x);
				packet.posY[idx] = FloatToInt(pos.y);
				packet.posZ[idx] = FloatToInt(pos.z);
				packet.exist[idx] = true;
				++count;
			}
			else {
				packet.exist[idx] = false;
			}
			++idx;
		}	
		if (count != 0) {
			packets.push_back(packet);
		}
	} 
	
	if (packets.empty()) return;
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}
		SECTOR_POSITION sector = (SECTOR_POSITION)m_Players[i]->GetPlayerExistingSector();
		if (sector == SECTOR_POSITION::SECTOR_1 || sector == SECTOR_POSITION::SECTOR_2) {
			continue;
		}
		for (auto p : packets) {
			SendPacket(m_Clients[i]->id, &p);
		}
	}
}


void CGameRoom::SendChessObjectActPacket()
{
	if (false == m_ChessChangeFlag) {
		return;
	}
	 
	P_S2C_CHESS_OBJ_UPDATE_SYNC packet;
	ZeroMemory(&packet, sizeof(packet));
	packet.size = sizeof(packet);
	packet.type = PACKET_PROTOCOL::S2C_CHESS_OBJ_ACT;

	for (int i = 0; i < 4; ++i) {
		XMFLOAT3 pos = m_Chess[i]->GetPosition();
		packet.posX[i] = FloatToInt(pos.x);
		packet.posY[i] = FloatToInt(pos.y);
		packet.posZ[i] = FloatToInt(pos.z);
	}
	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}

		SendPacket(m_Clients[i]->id, &packet);
	}
	m_ChessChangeFlag = false;
}

void CGameRoom::SendThunderSyncPacket()
{
	if (m_isOnThunderOn == false) {
		return;
	}
	P_S2C_THUNDER_UPDATE_SYNC packet;
	ZeroMemory(&packet, sizeof(packet));
	packet.size = sizeof(packet);
	packet.type = PACKET_PROTOCOL::S2C_THUNDER_ACT;
	 
	for (int i = 0; i < 7; ++i) {
		XMFLOAT3 pos = m_ObjectLayers[(int)OBJECT_LAYER::Thunder][i]->GetPosition();
		packet.posX[i] = FloatToInt(pos.x);
		packet.posY[i] = FloatToInt(pos.y);
		packet.posZ[i] = FloatToInt(pos.z);
	}

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}

		SendPacket(m_Clients[i]->id, &packet);
	}
}

void CGameRoom::SendDoorEvent()
{
}

void CGameRoom::SendDeletePacket(CGameObject* pObj, int layerIdx, int objIdx)
{
	if (false == m_ObjectDeleteFlag) {
		return;
	}
	  
	P_S2C_DELETE_SYNC packet;
	ZeroMemory(&packet, sizeof(packet));
	packet.size = sizeof(packet);
	packet.type = PACKET_PROTOCOL::S2C_DELETE_OBJ;
	 
	packet.idx = objIdx;
	packet.objType = layerIdx;

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i] == nullptr) {
			continue;
		}
		if (m_Clients[i]->m_state != PL_STATE::PLST_CONNECTED) {
			continue;
		}

		SendPacket(m_Clients[i]->id, &packet);
	}
	 
	m_ObjectDeleteFlag = false;
} 

void CGameRoom::Disconnect(int packet_id)
{ 
	cout << "로그 아웃\n"; 
	int id = m_IdIndexMatcher[packet_id];
	m_Players[id]->SetExistence(false);
	m_Clients[id]->m_state = PLST_FREE;

	P_S2C_DELETE_PLAYER p_deletePlayer;
	p_deletePlayer.type = PACKET_PROTOCOL::S2C_DELETE_PLAYER;
	p_deletePlayer.size = sizeof(P_S2C_DELETE_PLAYER);
	p_deletePlayer.id = id;

	for (int i = 0; i < MAX_ROOM_PLAYER; ++i) {
		if (m_Clients[i]->m_state != PLST_FREE) {
			SendPacket(m_Clients[i]->id, &p_deletePlayer);
		}
	}
	m_CurrentPlayerNum--;
	ResetPlayer(id);

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

		(*res)->SetIsUsable(true);
		//cout << " 객체 삭제\n";
		//m_ObjectLayers[layerIdx].erase(res);
	}
}
   
void CGameRoom::RecyleObject(CGameObject* pObject, int layerIdx)
{ 
	for (int i = 0; i < m_ObjectLayers[layerIdx].size(); ++i)
	{
		if (m_ObjectLayers[layerIdx][i] == pObject)
		{
			pObject->SetIsUsable(true);
			m_ObjectDeleteFlag = true;
			SendDeletePacket(pObject, layerIdx, i); 
			return;
		}
	}
	//auto res = std::find(m_ObjectLayers[layerIdx].begin(), m_ObjectLayers[layerIdx].end(), pObject);
	//
	//int idx = 0;
	//if (res != m_ObjectLayers[layerIdx].end()) {  
	//	pObject->SetIsUsable(true);
	//	SendDeletePacket(pObject, layerIdx, idx);
	//	m_ObjectDeleteFlag = true;
	//	++idx;
	//}
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

void CGameRoom::ShotPlayerArrow(int p_id)
{
	int i = 0; 
	for (auto* pObj : m_ObjectLayers[(int)OBJECT_LAYER::PlayerArrow]) {
		CArrow* pArrow = reinterpret_cast<CArrow*>(pObj);
		if (pArrow->IsUsable()) {
			pArrow->SetIsUsable(false);
			XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Players[p_id]->GetPosition() }, { 0,180,0 });
			pArrow->SetPosition(pos);
			pArrow->m_startPos = pos;
			pArrow->SetStringPower(m_Players[p_id]->GetStringPullTime()); 
			pArrow->SetTargetVector(Vector3::Multifly(m_Players[p_id]->GetLook(), 1));
			pArrow->SetExistingSector((SECTOR_POSITION)m_Players[p_id]->GetPlayerExistingSector());
			pArrow->SetATK(m_Players[p_id]->GetATK());
			break;
		}
		++i;
	}
}

void CGameRoom::ShotFireBall(OBJECT_LAYER type, CGameObject* user)
{
	for (auto* pObj : m_ObjectLayers[(int)OBJECT_LAYER::FireBall]) {
		CFireBall* pFireb = reinterpret_cast<CFireBall*>(pObj);
		if (pFireb->IsCanUse()) {
			{
				pFireb->SetUseable(false);
				pFireb->SetSkill(user);
				pFireb->SetATK(user->GetATK());
			}
			break;
		}
	}
}

void CGameRoom::ActiveThunder(const XMFLOAT3& pos, int index)
{
	m_ObjectLayers[(int)OBJECT_LAYER::Thunder][index]->SetIsUsable(false);
	m_ObjectLayers[(int)OBJECT_LAYER::Thunder][index]->SetPosition(pos);
	m_ObjectLayers[(int)OBJECT_LAYER::Thunder][index]->UpdateColliders();
}

void CGameRoom::DisableThunder()
{
	for (int i = 0; i < m_ObjectLayers[(int)OBJECT_LAYER::Thunder].size(); ++i) {
		m_ObjectLayers[(int)OBJECT_LAYER::Thunder][i]->SetIsUsable(true);
		//m_ObjectLayers[(int)OBJECT_LAYER::Thunder][i]->SetPosition({ -10000,-10000, -10000 });
	}
}


void CGameRoom::ShotMonsterArrow(CEnemy* pEmeny, const XMFLOAT3& lookVector)
{
	for (auto* pObj : m_ObjectLayers[(int)OBJECT_LAYER::MonsterArrow]) {
		CArrow* pArrow = reinterpret_cast<CArrow*>(pObj);
		if (pArrow->IsUsable()) {
			pArrow->SetIsUsable(false);
			XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ pEmeny->GetPosition() }, { 0,150,0 });
			pArrow->SetPosition(pos);
			pArrow->SetTargetVector(lookVector);
			pArrow->SetExistingSector(pEmeny->GetExistingSector()); 
			break;
		}
	}
}

void CGameRoom::DeleteEnemy(CEnemy* pEmeny)
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
		(*res)->SetIsUsable(false);
		//m_ObjectLayers[(int)OBJECT_LAYER::Enemy].erase(res);
	}
}

void CGameRoom::ShotMummyLaser(CMummy* pMummy, const XMFLOAT3& lookVector)
{
	XMFLOAT3 scale = { 300.0f,300.0f,300.0f };
	if (pMummy->GetEnemyAttackType() == EnemyAttackType::Mummy1 && m_MummyExist[0] == true)
	{ 
		// 미라가 2마리 남았을때
		if (m_One_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 1.5f, scale.y * 1.5f, scale.z * 1.5f));
				m_Mummy[i]->SetActivityScope({ 2000.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
			}
			m_One_Mira_Die_Laser = false;
		}
		if (m_Two_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 3.f, scale.y * 3.f, scale.z * 3.f));
				m_Mummy[i]->SetActivityScope({ 2800.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });

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
		// 미라가 2마리 남았을때
		if (m_One_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2000.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE, GetDetailHeight(g_Heights, 18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 1.5f, scale.y * 1.5f, scale.z * 1.5f));
			}
			m_One_Mira_Die_Laser = false;
		}
		if (m_Two_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2800.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE,GetDetailHeight(g_Heights, 18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
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
		// 미라가 2마리 남았을때
		if (m_One_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2000.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE,GetDetailHeight(g_Heights,18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
				m_Mummy[i]->SetSize(XMFLOAT3(scale.x * 1.5f, scale.y * 1.5f, scale.z * 1.5f));
			}
			m_One_Mira_Die_Laser = false;
		}
		if (m_Two_Mira_Die_Laser == true)
		{
			for (int i = 0; i < 3; i++)
			{
				m_Mummy[i]->SetActivityScope({ 2800.0f, 0, 250.0f }, { (18900.f - (2000 * i)) * MAP_SCALE_SIZE,GetDetailHeight(g_Heights, 18900.f, 6250.f), 6250.0f * MAP_SCALE_SIZE });
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
		cout << "player_id : " << p_id << " p_id : " << id << endl;
		if (m_CurrentPlayerNum > MAX_ROOM_PLAYER) {
			p_processLogin.isSuccess = false;
		}
		else {
			p_processLogin.isSuccess = true;

			m_Players[id]->SetExistence(true);
			m_Players[id]->SetId(p_id);
			m_Players[id]->SetWeapon((PlayerWeaponType)p_login.weaponType);
			//m_Players[id]->SetWeaponPointer();
			XMFLOAT3 pos = m_Players[id]->GetPosition();

			p_processLogin.x = FloatToInt(pos.x);
			p_processLogin.y = FloatToInt(pos.y);
			p_processLogin.z = FloatToInt(pos.z);
		}
		p_processLogin.id = id;
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
		if (p_keyboard.isKeyDown) {
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

					switch (m_Players[id]->GetWeapon())
					{
					case PlayerWeaponType::Sword:
						m_Players[id]->Attack(0);
						break;
					case PlayerWeaponType::Bow:
						if (m_Players[id]->GetSP() > 20.0f) {
							m_Players[id]->SetSP(m_Players[id]->GetSP() - 20);
							m_Players[id]->Attack(0);
							m_Players[id]->pullString = true;
						}
						break;
					case PlayerWeaponType::Staff:
						if (m_Players[id]->GetSP() > 30.0f) {
							m_Players[id]->SetSP(m_Players[id]->GetSP() - 30);
							m_Players[id]->Attack(0);
							ShotFireBall(OBJECT_LAYER::FireBall, m_Players[id]);
						} 
						break;
					}
				}
				else {
					switch (m_Players[id]->GetWeapon())
					{
					case PlayerWeaponType::Sword:
						if (m_Players[id]->GetAttackWaitTime() < 0.5f) {

							m_Players[id]->SetSwordAttackKeyDown(true);
						}
						break;
					}
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
			case VK_Z:
			{
				m_Chess[King]->SetPosition(m_ChessPlate[5][6]);
				m_Chess[Rook]->SetPosition(m_ChessPlate[1][4]);
				m_Chess[Pawn]->SetPosition(m_ChessPlate[3][5]);
				m_Chess[Knight]->SetPosition(m_ChessPlate[6][4]);
				m_ChessChangeFlag = true;
			}
			break; 
			case VK_N:
			{
				//무적
				m_Players[id]->PowerOverWhelm();
			}
			break;
			case VK_M:
			{ 
				// 한방
				m_Players[id]->SetATK(500);
			}
			break;
			case VK_R:
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

			case VK_F7:
			{
				DeleteEnemy(m_Mummy[0]);
			}
			break;
			case VK_F8:
			{
				DeleteEnemy(m_Mummy[1]);
			}
			break;
			case VK_F9:
			{
				DeleteEnemy(m_Mummy[2]);
			}
			break;
			}
		}
		else {
		if (p_keyboard.keyInput == VK_J) {
			switch (m_Players[id]->GetWeapon())
			{
			case PlayerWeaponType::Sword: 
				break;
			case PlayerWeaponType::Bow:
				if (m_Players[id]->ShotAble()) {
					ShotPlayerArrow(id); 
				}
				else {
					m_Players[id]->IncreaseAttackWaitingTime(0);
					m_Players[id]->SetAnimationSet(IDLE);
				}
				m_Players[id]->ResetBow();
				break;
			case PlayerWeaponType::Staff: 
				break;
			}
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
				 
				if (m_Players[p_mouse.id]->pullString) {
					m_Cameras[p_mouse.id]->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 75);
					cameraRotateY += dx * 75;
					//m_CurrentCamera->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 150);
				}
				else {
					m_Cameras[p_mouse.id]->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 80);
					cameraRotateY += dx * 80; 
				}

				if (m_Players[p_mouse.id]->IsMoving() || m_Players[p_mouse.id]->pullString)
				{
					playerRotateY += dx * 150;
					m_Players[p_mouse.id]->Rotate(XMFLOAT3(0, 1, 0), dx * 150); 
				}
				 

				//m_Cameras[p_mouse.id]->RotateAroundTarget(XMFLOAT3(0, 1, 0), dx * 75);
				//cameraRotateY += dx * 75; 
				//if (m_Players[p_mouse.id]->IsMoving())
				//{
				//	playerRotateY += dx * 150;
				//	m_Players[p_mouse.id]->Rotate(XMFLOAT3(0, 1, 0), dx * 150);
				//}

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