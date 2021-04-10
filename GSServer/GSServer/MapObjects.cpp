#include "stdafx.h"
#include "MapObjects.h"

CSign::CSign(OBJECT_ID id)
{
	m_Pillar = new CGameObject(); 
	m_Pillar->AddBoundingBox(BoundingBox(XMFLOAT3{ 0, 0, 0 }, 
		XMFLOAT3{ 75 * 0.5f, 500 * 0.5f, 50 * 0.5f })); 	

	m_Board = new CGameObject();
	m_Board->AddBoundingBox(BoundingBox(XMFLOAT3{ 0, 0, 0 }, 
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

bool CSign::CollisionCheck(const BoundingBox& aabb)
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
}

CDoor::~CDoor()
{
}

void CDoor::Update(float fTimeElapsed)
{
	if (m_IsLeft)
	{
		float rotateAngle = 50;
		if (!m_IsOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 89.0f)
		{
			m_IsOpening = false;
		}
		else if (m_fAngle < -89.0f)
		{
			m_IsOpening = true;
		}
	}
	else
	{
		float rotateAngle = -50;
		if (!m_IsOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 89.0f)
		{
			m_IsOpening = true;
		}
		else if (m_fAngle < -89.0f)
		{
			m_IsOpening = false;
		}
	}
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
	pWall->AddBoundingBox(BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 4 * 0.5f, height * 0.5f, depth * 0.5f })); 
	m_Walls.push_back(std::move(pWall));

	pWall = new CGameObject();
	pWall->SetPosition({ fWidthRatio * 6 + fWidthRatio * 2,  height * 0.5f, depth * 0.5f });
	pWall->AddBoundingBox(BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 4 * 0.5f, height * 0.5f, depth * 0.5f }));
	m_Walls.push_back(std::move(pWall));

	m_LeftDoor = new CDoor(true);
	m_LeftDoor->AddBoundingBox(BoundingBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 0.5f, doorHeight * 0.5f, depth * 0.2f * 0.5f }));

	m_RightDoor = new CDoor(false);
	m_RightDoor->AddBoundingBox(BoundingBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f),
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

bool CDoorWall::CollisionCheck(const BoundingBox& aabb)
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
