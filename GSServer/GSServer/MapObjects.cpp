#include "stdafx.h"
#include "MapObjects.h"


CBridge::CBridge(OBJECT_ID id)
{
	m_CollisionHandleType = COLLISION_HANDLE_TYPE::NotCollide;
	m_ExistingSector = SECTOR_POSITION::SECTOR_2;
	CGameObject* pObject;// = new CGameObject();
	for (int i = 0; i < 10; i += 5) {
		pObject = new CGameObject();
		pObject->AddBoundingBox(new BoundingBox(
			XMFLOAT3(-0.0f, 251.0f, -250.0f + 100.0f * i),
			XMFLOAT3(1000 * 0.5f, 1 * 0.5f, 500 * 0.5f))); 
		pObject->SetSize({ 1, 251.0f ,1 });
		pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);

		m_Plates.emplace_back(std::move(pObject));
	}

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(
		XMFLOAT3(-500.0f, 250.0f + 12.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f)));
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(
		XMFLOAT3(500.0f, 250.0f + 12.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f)));
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(
		XMFLOAT3(-500.0f, 250.0f + 137.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f)));
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->AddBoundingBox(new BoundingBox(
		XMFLOAT3(500.0f, 250.0f + 137.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f))); 
	m_Objects.emplace_back(std::move(pObject));

	for (int i = 0; i < 5; ++i)
	{
		pObject = new CGameObject();
		pObject->AddBoundingBox(new BoundingBox(
			XMFLOAT3(-500.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			XMFLOAT3(50 * 0.5f, 100 * 0.5f, 50 * 0.5f))); 
		m_Objects.emplace_back(std::move(pObject));

		pObject = new CGameObject();
		pObject->AddBoundingBox(new BoundingBox(
			XMFLOAT3(500.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			XMFLOAT3(50 * 0.5f, 100 * 0.5f, 50 * 0.5f)));
		m_Objects.emplace_back(std::move(pObject));
	}

	for (int i = 0; i < 3; ++i)
	{
		pObject = new CGameObject();
		pObject->AddBoundingBox(new BoundingBox(
			XMFLOAT3(0.0f, 250.0f - 25.0f, -350.0f + 350.0f * i),
			XMFLOAT3(1050.0f * 0.5f, 50.0f * 0.5f, 150.0f * 0.5f)));
		m_Objects.emplace_back(std::move(pObject));

		pObject = new CGameObject();
		pObject->AddBoundingBox(new BoundingBox(
			XMFLOAT3(0.0f, 0.0f, -350.0f + 350.0f * i),
			XMFLOAT3(50.0f * 0.5f, 400.0f * 0.5f, 50.0f * 0.5f)));
		m_Objects.emplace_back(std::move(pObject));
		 
		pObject = new CGameObject();
		pObject->AddBoundingBox(new BoundingBox(
			XMFLOAT3(-500.0f, 0.0f, -350.0f + 350.0f * i),
			XMFLOAT3(50.0f * 0.5f, 400.0f * 0.5f, 50.0f * 0.5f)));
		m_Objects.emplace_back(std::move(pObject));

		pObject = new CGameObject();
		pObject->AddBoundingBox(new BoundingBox(
			XMFLOAT3(-500.0f, 0.0f, -350.0f + 350.0f * i),
			XMFLOAT3(50.0f * 0.5f, 400.0f * 0.5f, 50.0f * 0.5f)));
		m_Objects.emplace_back(std::move(pObject));
	}
}

void CBridge::SetPosition(XMFLOAT3 pos)
{
	for (auto pObj : m_Objects) {
		pObj->SetPosition(pos);
	}
	for (auto pPlate : m_Plates) {
		pPlate->SetPosition(pos);
	}
	CGameObject::SetPosition(pos);
}

void CBridge::Rotate(const XMFLOAT3& axis, float angle)
{
	for (auto pObj : m_Objects) {
		pObj->Rotate(axis, angle);
	}
	for (auto pPlate : m_Plates) {
		pPlate->Rotate(axis, angle);
	}
}

bool CBridge::CollisionCheck(BoundingBox* aabb)
{ 
	/*for (auto pObj : m_Objects) {
		if (pObj->CollisionCheck(aabb)) {
			m_CollideObject = pObj;
			return true; 
		}
	}*/
	for (auto pPlate : m_Plates) {
		if (pPlate->CollisionCheck(aabb)) {
			m_CollideObject = pPlate;
			return true;
		}
	}
	return false;
}

bool CBridge::CollisionCheck(CGameObject* other)
{
	auto otherAABB = other->GetAABB();
	for (int i = 0; i < otherAABB.size(); ++i) {
		bool result = CollisionCheck(otherAABB[i]);
		if (result) {
			other->FixCollision(m_CollideObject);
			return true;
		}
	}

	return false;
}

void CBridge::UpdateColliders()
{
	for (auto pObj : m_Objects) {
		pObj->UpdateColliders();
	}
	for (auto pPlate : m_Plates) {
		pPlate->UpdateColliders();
	} 
}

CSign::CSign(OBJECT_ID id)
{
	m_Pillar = new CGameObject(); 
	m_Pillar->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0, 0, 0 }, 
		XMFLOAT3{ 75 * 0.5f, 500 * 0.5f, 50 * 0.5f })); 	

	m_Board = new CGameObject();
	m_Board->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0, 0, 0 }, 
		XMFLOAT3{ 500.0f * 0.5f, 300.0f * 0.5f, 5.0f * 0.5f })); 
  
	switch (id)
	{ 
	case OBJECT_ID::SIGN_SCROLL:
		m_Board->SetPosition({ 0, 250, -40 });
		break;
	case OBJECT_ID::SIGN_PUZZLE:
		m_Board->SetPosition({ 0, 250, 20 });
		break;
	case OBJECT_ID::SIGN_MEDUSA:
		m_Board->SetPosition({ -40, 250, 0.0f });
		break;
	case OBJECT_ID::SIGN_BOSS:
		break; 
	}
	//"SIGN_SCROLL": [2700, 200, 7000] ,
	//"SIGN_PUZZLE" : [11200, -1800, 8200] ,
	//"SIGN_MEDUSA" : [13000, -3250, 1300] ,
	//"SIGN_BOSS" : [14000, -4500, 8000] ,
}

CSign::~CSign()
{
}

void CSign::SetPosition(XMFLOAT3 pos)
{
	m_Pillar->Move(pos);
	m_Board->Move(pos);
}

void CSign::Rotate(const XMFLOAT3& axis, float angle)
{
	m_Pillar->Rotate(axis, angle);
	m_Board->Rotate(axis, angle);
}

bool CSign::CollisionCheck(BoundingBox * aabb)
{
	if (m_Board->CollisionCheck(aabb)) return true;
	if (m_Pillar->CollisionCheck(aabb)) return true;

	return false;
}

void CSign::UpdateColliders()
{
	m_Board->UpdateColliders();
	m_Pillar->UpdateColliders();
}


CDoor::CDoor(bool isLeft)
{
	if (!m_IsLeft) Rotate(XMFLOAT3(0, 1, 0), 180.0);

	m_IsOnAnimating = false;
	m_IsOnOpening = false;
}

CDoor::~CDoor()
{
}

void CDoor::Update(float fTimeElapsed)
{
	if (false == m_IsOnAnimating) {
		return;
	}
	if (m_IsLeft)
	{
		float rotateAngle = -50;
		if (!m_IsOnOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 0.0f)
		{
			m_IsOnOpening = false;
			m_IsOnAnimating = false;
		}
		else if (m_fAngle < -89.0f)
		{
			m_IsOnOpening = true;
			m_IsOnAnimating = false;
		}
	}
	else
	{
		float rotateAngle = 50;
		if (!m_IsOnOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 89.0f)
		{
			m_IsOnOpening = true;
			m_IsOnAnimating = false;
		}
		else if (m_fAngle < 0.0f)
		{
			m_IsOnOpening = false;
			m_IsOnAnimating = false;
		}
	}
}

void CDoor::Open()
{
	if (m_IsOnOpening) {
		return;
	}
	m_IsOnAnimating = true;
	m_IsOnOpening = true;
}

void CDoor::Close()
{
	if (false == m_IsOnOpening) {
		return;
	}
	m_IsOnAnimating = true;
	m_IsOnOpening = false;
}

CDoorWall::CDoorWall(OBJECT_ID id)
{
	float width;
	float height;
	float depth;
	float doorHeight = 2500.0f;
	switch (id)
	{
	case OBJECT_ID::DOOR_WALL_SEC1: 
		width = 4000;
		height = 1000;
		depth = 500;
		doorHeight = height;
		break;
	case OBJECT_ID::DOOR_WALL_SEC2:
		width = 3300;
		height = 1000;
		depth = 500; 
		doorHeight = height;
		break;
	case OBJECT_ID::DOOR_WALL_SEC3:
		width = 4000;
		height = 2500;
		depth = 500;
		break;
	case OBJECT_ID::DOOR_WALL_SEC4:
		width = 5500;
		height = 2000;
		depth = 500;
		doorHeight = height;
		break;
	case OBJECT_ID::DOOR_WALL_SEC5:
		width = 5700;
		height = 4500;
		depth = 800;
		doorHeight = 2500.0f;
		break;  
	}
	 
	float fWidthRatio = width * 0.1f;
	CGameObject* pWall = new CGameObject(); 
	pWall->SetPosition({ fWidthRatio * 2,  height * 0.5f, depth * 0.5f });
	pWall->AddBoundingBox(new BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 4 * 0.5f, height * 0.5f, depth * 0.5f })); 
	m_Walls.push_back(std::move(pWall));

	pWall = new CGameObject();
	pWall->SetPosition({ fWidthRatio * 6 + fWidthRatio * 2,  height * 0.5f, depth * 0.5f });
	pWall->AddBoundingBox(new BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 4 * 0.5f, height * 0.5f, depth * 0.5f }));
	m_Walls.push_back(std::move(pWall));

	m_LeftDoor = new CDoor(true);
	m_LeftDoor->AddBoundingBox(new BoundingBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 0.5f, doorHeight * 0.5f, depth * 0.2f * 0.5f }));

	m_RightDoor = new CDoor(false);
	m_RightDoor->AddBoundingBox(new BoundingBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 0.5f, doorHeight * 0.5f, depth * 0.2f * 0.5f }));

	if (height > doorHeight) {
		m_LeftDoor->SetPosition({ fWidthRatio * 4,  height * 0.5f - doorHeight * 0.5f, depth * 0.5f });
		m_RightDoor->SetPosition({ fWidthRatio * 6,  height * 0.5f - doorHeight * 0.5f, depth * 0.5f });
	}
	else {
		m_LeftDoor->SetPosition({ fWidthRatio * 4,  height * 0.5f, depth * 0.5f });
		m_RightDoor->SetPosition({ fWidthRatio * 6,  height * 0.5f, depth * 0.5f });
	}
}

CDoorWall::~CDoorWall()
{
}

void CDoorWall::Update(float fTimeElapsed)
{
	m_LeftDoor->Update(fTimeElapsed);
	m_RightDoor->Update(fTimeElapsed);  
}

void CDoorWall::SetPosition(XMFLOAT3 pos)
{
	m_LeftDoor->Move(pos);
	m_RightDoor->Move(pos);
	for (auto pWall : m_Walls)
	{
		pWall->Move(pos);
	}
}

void CDoorWall::Rotate(const XMFLOAT3& axis, float angle)
{
	m_LeftDoor->Rotate(axis, angle);
	m_RightDoor->Rotate(axis, angle);

	for (auto pWall : m_Walls)
	{
		pWall->Rotate(axis, angle);
	}
}

bool CDoorWall::CollisionCheck(BoundingBox * aabb)
{
	if (m_LeftDoor->CollisionCheck(aabb)) return true;
	if (m_RightDoor->CollisionCheck(aabb)) return true;

	for (auto pWall : m_Walls) {
		if (pWall->CollisionCheck(aabb)) return true;
	}
	return false;
}

void CDoorWall::UpdateColliders()
{ 
	m_LeftDoor->UpdateColliders();
	m_RightDoor->UpdateColliders();

	for (auto pWall : m_Walls) {
		pWall->UpdateColliders();
	}
}

void CDoorWall::OpenDoor()
{
	m_LeftDoor->Open();
	m_RightDoor->Open();
}

void CDoorWall::CloserDoor()
{
	m_LeftDoor->Close();
	m_RightDoor->Close();
}
CPuzzle::CPuzzle(OBJECT_ID id)
{
	m_ExistingSector = SECTOR_POSITION::SECTOR_3;
	for (int i = 1; i <= 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{ 
			CGameObject* pObject = new CGameObject();
			pObject->SetPosition(XMFLOAT3(500.0f * i + 300.0f, 250.0f, 150.0f + 410.0f + 500.0f * j));
			pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 300 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.emplace_back(std::move(pObject));

			pObject = new CGameObject();
			pObject->SetPosition(XMFLOAT3(500.0f * i + 300.0f, 250.0f, -150.0f + 410.0f + 500.0f * j));
			pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 300 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.emplace_back(std::move(pObject));
			
			pObject = new CGameObject();
			pObject->SetPosition(XMFLOAT3(175.0f + 500.0f * i + 300.0f, 250.0f, 0.0f + 410.0f + 500.0f * j));
			pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
			pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 350 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.emplace_back(std::move(pObject));
			
			pObject = new CGameObject();
			pObject->SetPosition(XMFLOAT3(-175.f + 500.0f * i + 300.0f, 250.0f, 0.0f + 410.0f + 500.0f * j));
			pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
			pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 350 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.emplace_back(std::move(pObject));
		}
	}
}

CPuzzle::~CPuzzle()
{
}

void CPuzzle::SetPosition(XMFLOAT3 pos)
{
	for (auto pObj : m_Objects)
	{
		pObj->Move(pos);
	}
}

bool CPuzzle::CollisionCheck(BoundingBox * aabb)
{
	//for (auto pObj : m_Objects) {
	//	if (pObj->CollisionCheck(aabb)) {
	//		return true;
	//	}
	//}
	return false; 
}

void CPuzzle::UpdateColliders()
{
	for (auto pObj : m_Objects) {
		pObj->UpdateColliders();
	}
}

CPlate::CPlate(OBJECT_ID id)
{
	m_ExistingSector = SECTOR_POSITION::SECTOR_3;
	CGameObject* pObject = new CGameObject();
	pObject->SetPosition({ 1250.0f, 120.f, 900.0f });
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 2500 * 0.5f, 240 * 0.5f, 1800 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On); 
	pObject->SetSize({ 1, 240 * 0.5f, 1 });
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetPosition(XMFLOAT3(1250.0f, 40.0f, -600 * 0.5f));
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 600 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On); 
	pObject->SetSize({ 1, 80 * 0.5f, 1 });
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetPosition(XMFLOAT3(1250.0f, 40.0f, 600 * 0.5f + 1800.0f));
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 600 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->SetSize({ 1, 40.0f * 0.5f, 1 });
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetPosition(XMFLOAT3(1250.0f, 120.0f, -400 * 0.5f));
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 400 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->SetSize({ 1, 80 * 0.5f, 1 });
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetPosition(XMFLOAT3(1250.0f, 120.0f, 400 * 0.5f + 1800.0f));
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 400 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->SetSize({ 1, 80 * 0.5f, 1 });
	m_Objects.emplace_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetPosition(XMFLOAT3(1250.0f, 200.0f, -200 * 0.5f));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 200 * 0.5f }));
	pObject->SetSize({ 1, 80 * 0.5f, 1 });
	m_Objects.emplace_back(std::move(pObject));
	 
	pObject = new CGameObject();
	pObject->SetPosition(XMFLOAT3(1250.0f, 200.0f, 200 * 0.5f + 1800.0f));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->AddBoundingBox(new BoundingBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 200 * 0.5f }));
	pObject->SetSize({ 1, 80 * 0.5f, 1 });
	m_Objects.emplace_back(std::move(pObject));
}

CPlate::~CPlate()
{
}

void CPlate::SetPosition(XMFLOAT3 pos)
{
	for (auto obj : m_Objects) {
		obj->Move(pos);
	}
}

void CPlate::Rotate(const XMFLOAT3& axis, float angle)
{
	for (auto obj : m_Objects) {
		obj->Rotate(axis, angle);
	}
}

bool CPlate::CollisionCheck(BoundingBox * aabb)
{
	bool res = false;
	for (auto pObj : m_Objects) {
		if (pObj->CollisionCheck(aabb)) { 
			m_CollideObject->FixCollision(pObj);
			res = true;
		}
	}
	return res;
}

bool CPlate::CollisionCheck(CGameObject* other)
{
	bool res = false;
	auto otherAABB = other->GetAABB();
	m_CollideObject = other;
	for (int i = 0; i < otherAABB.size(); ++i) {
		bool result = CollisionCheck(otherAABB[i]);
		if (result) {
			other->FixCollision(m_CollideObject);
			//return true;
			res = true;
		}
	}

	return res;
}

void CPlate::UpdateColliders()
{
	for (auto pObj : m_Objects) {
		pObj->UpdateColliders();
	}
}

CPuzzleBox::CPuzzleBox(OBJECT_ID id)
{
}

CPuzzleBox::~CPuzzleBox()
{
}
