#include "stdafx.h"
#include "Player.h" 

CPlayer::CPlayer()
{
	m_HP = 100;
	m_SP = 100;

	m_SpareCollisionBox = BoundingBox(XMFLOAT3(0, 0.6, 0.2f), XMFLOAT3(0.2, 0.6, 1.4));
	m_SpareAABB = BoundingBox(XMFLOAT3(0, 0.6, 0.2f), XMFLOAT3(0.2, 0.6, 1.4));
}

CPlayer::~CPlayer()
{

}

void CPlayer::Update(float fTimeElapsed)
{
	if (false == m_IsCanAttack) {
		m_AttackWaitingTime -= fTimeElapsed;
		m_StateName = AnimationType::SWORD_ATK;
		if (m_AttackWaitingTime < 0.0f) { 
			m_IsAlreadyAttack = false;
			m_AttackWaitingTime = 0.0f;
			m_IsCanAttack = true; 

			auto temp = m_SpareCollisionBox;
			m_SpareCollisionBox = m_BoundingBox[0];
			m_BoundingBox[0] = temp;

			temp = m_SpareAABB;
			m_SpareAABB = m_AABB[0];
			m_AABB[0] = temp;

			UpdateColliders();
		}
	}

	// ÇÇ°Ý
	else if (m_AttackedDelay > 0.0f) {
		m_AttackedDelay = max(m_AttackedDelay - fTimeElapsed, 0.0f);
	}
	else {
		if (m_xmf3Velocity.x == 0 && m_xmf3Velocity.z == 0)
			m_StateName = AnimationType::IDLE;
		else
			m_StateName = AnimationType::SWORD_RUN;
	}
	float Friction = (m_MovingType == Player_Move_Type::Run) ? PLAYER_RUN_SPEED : PLAYER_WALK_SPEED;
	
	XMFLOAT3 vel = Vector3::Multifly(m_xmf3Velocity, fTimeElapsed);

	Move(vel);  

	UpdateCamera(); 

	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed); 
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	//m_xmf3Velocity.x = m_xmf3Velocity.y = m_xmf3Velocity.z = 0.0f;
}

void CPlayer::UpdateCamera()
{
	if (m_Camera != nullptr) {
		m_Camera->Update(m_xmf3Position);
		m_Camera->LookAt(m_Camera->GetPosition3f(), m_xmf3Position, GetUp());
		m_Camera->UpdateViewMatrix();
	}
}

void CPlayer::FixCameraByTerrain(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1])
{
	if (m_Camera == nullptr) {
		return;
	}
	XMFLOAT3 xmf3CameraPosition = m_Camera->GetPosition3f();
	  
	float offsetHeight = m_Camera->GetOffset().y;
	float fHeight = GetDetailHeight(heightsMap, xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	if (fHeight - 1500.0f > m_xmf3Position.y) {
		fHeight = m_xmf3Position.y + 1500.0f;
	}
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_Camera->SetPosition(xmf3CameraPosition);
		m_Camera->LookAt(m_xmf3Position, GetUp());
	}
}

void CPlayer::FixPositionByTerrain(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1])
{
	m_xmf3Position.y = GetDetailHeight(heightsMap, m_xmf3Position.x, m_xmf3Position.z)
		+ m_HeightFromTerrain;
	//SetPosition(m_xmf3Position);
}

int CPlayer::GetPlayerExistingSector() const
{
	if (m_xmf3Position.x < 14379 && m_xmf3Position.z < 22824) {
		return 0;
	}
	if (m_xmf3Position.x < 14379 && m_xmf3Position.z >= 22824) {
		return 1;
	}
	if (m_xmf3Position.x >= 14379 && m_xmf3Position.x < 20422 &&
		(m_xmf3Position.z > 10764)) {
		return 2;
	}
	if ((m_xmf3Position.x >= 14379 && m_xmf3Position.x < 20422) &&
		m_xmf3Position.z <= 10764) {
		return 3;
	}

	if (m_xmf3Position.x >= 20422 && m_xmf3Position.z < 12077) {
		return 3;
	}
	if (m_xmf3Position.x >= 20422 && m_xmf3Position.z >= 12077) {
		return 4;
	}
	return -1;
}

void CPlayer::SetVelocity(const XMFLOAT3& dir)
{
	if (false == IsCanAttack()) {
		return;
	}
	XMFLOAT3 xmf3Dir = dir;
	xmf3Dir.y = 0;
	XMFLOAT3 normalizedDir = Vector3::Normalize(xmf3Dir); 
	XMFLOAT3 targetPosition = Vector3::Multifly(normalizedDir, 150000.0f);
	LookAt(m_xmf3Position, targetPosition, XMFLOAT3{ 0,1,0 });
	Rotate({ 0,1,0 }, 180.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, m_xmf3Velocity.y, 0.0f);
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity,
		Vector3::Multifly(normalizedDir, PLAYER_RUN_SPEED));

	float speed = m_MovingType == (Player_Move_Type::Run) ? PLAYER_RUN_SPEED : PLAYER_WALK_SPEED;

	if (m_xmf3Velocity.x > speed) m_xmf3Velocity.x = speed;
	if (m_xmf3Velocity.y > speed) m_xmf3Velocity.y = speed;
	if (m_xmf3Velocity.z > speed) m_xmf3Velocity.z = speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
}

bool CPlayer::Attacked(CGameObject* pObject)
{
	if (m_AttackedDelay > 0.0f) {
		return false;
	}
	m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	m_AttackedDelay += 0.6666667f;
	m_HP -= 5;
	m_StateName = AnimationType::DAMAGED;
	if (m_HP <= 5) {
		m_HP = 0;
	}
	return true;
}

void CPlayer::Attack()
{
	SetCanAttack(false);
	IncreaseAttackWaitingTime(PLAYER_SWORD_ATTACK_TIME);
	m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	 
	auto temp = m_BoundingBox[0];
	m_BoundingBox[0] = m_SpareCollisionBox;
	m_SpareCollisionBox = temp;

	temp = m_AABB[0];
	m_AABB[0] = m_SpareAABB;
	m_SpareAABB = temp; 

	UpdateColliders();
}
