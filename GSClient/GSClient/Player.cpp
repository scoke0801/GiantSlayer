#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "Terrain.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Type = OBJ_TYPE::Player;

	m_HP = 100;
	m_SP = 100;
}

CPlayer::~CPlayer()
{

}

void CPlayer::Update(float fTimeElapsed)
{
	if (false == m_IsCanAttack) {
		m_AttackWaitingTime -= fTimeElapsed;
	
		if (m_AttackWaitingTime < 0.0f){
			m_AttackWaitingTime = 0.0f;
			m_IsCanAttack = true;
		}
	}
	
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	float Friction = (m_MovingType == PlayerMoveType::Run) ? 360.0f : 50.0f;

	XMFLOAT3 vel = Vector3::Multifly(m_xmf3Velocity, fTimeElapsed);

	Move(vel); 
	
	UpdateCamera(); 

	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed); 
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPlayer::UpdateCamera()
{
	if (m_Camera != nullptr) {
		m_Camera->Update(m_xmf3Position);
		m_Camera->LookAt(m_Camera->GetPosition3f(), m_xmf3Position, GetUp());
		m_Camera->UpdateViewMatrix();
	}
}

void CPlayer::FixPositionByTerrain(CTerrain* pTerrain)
{
	m_xmf3Position.y = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z);
} 

void CPlayer::SetVelocity(OBJ_DIRECTION direction)
{ 	
	//XMFLOAT3 look = GetLook();//m_Camera->GetLook3f();
	//XMFLOAT3 right = GetRight();//m_Camera->GetRight3f();
	XMFLOAT3 look = m_Camera->GetLook3f();
	XMFLOAT3 right = m_Camera->GetRight3f();

	look.y = right.y = 0.0f;
	look = Vector3::Normalize(look);
	right = Vector3::Normalize(right);

	XMFLOAT3 xmf3Dir = Vector3::Normalize(m_xmf3Velocity);
	switch (direction)
	{
	case OBJ_DIRECTION::Front:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(look, PLAYER_RUN_VELOCITY));
		xmf3Dir.z += 500;
		break;
	case OBJ_DIRECTION::Back:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(look, -1), PLAYER_RUN_VELOCITY));
		xmf3Dir.z -= 500;
		break;
	case OBJ_DIRECTION::Left:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(right, -1), PLAYER_RUN_VELOCITY));
		xmf3Dir.x -= 500;
		break;
	case OBJ_DIRECTION::Right:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(right, PLAYER_RUN_VELOCITY));
		xmf3Dir.x += 500;
		break;
	default:
		assert("잘못된 방향으로 이동할 수 없어요~");
		break;
	}	
	XMFLOAT3 playerLookAt = Vector3::Normalize(GetLook());
	float angle = Vector3::GetAngle(xmf3Dir, playerLookAt);
	//cout << "각도 : " << angle << "\n"; 
	
	LookAt(m_xmf3Position, xmf3Dir, { 0,1,0 }); 
	float speed = m_MovingType == (PlayerMoveType::Run) ? PLAYER_RUN_VELOCITY : PLAYER_WALK_VELOCITY;
	if (m_xmf3Velocity.x >  speed) m_xmf3Velocity.x =  speed;
	if (m_xmf3Velocity.y >  speed) m_xmf3Velocity.y =  speed;
	if (m_xmf3Velocity.z >  speed) m_xmf3Velocity.z =  speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
} 
void CPlayer::SetVelocity(XMFLOAT3 dir)
{
	dir.y = 0;
	XMFLOAT3 normalizedDir = Vector3::Normalize(dir); 
	    
	XMFLOAT3 targetPosition = Vector3::Multifly(normalizedDir, 150000.0f);
	LookAt(m_xmf3Position, targetPosition, XMFLOAT3{ 0,1,0 }); 

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity,
		Vector3::Multifly(normalizedDir, PLAYER_RUN_VELOCITY));
	float speed = m_MovingType == (PlayerMoveType::Run) ? PLAYER_RUN_VELOCITY : PLAYER_WALK_VELOCITY;
	if (m_xmf3Velocity.x > speed) m_xmf3Velocity.x = speed;
	if (m_xmf3Velocity.y > speed) m_xmf3Velocity.y = speed;
	if (m_xmf3Velocity.z > speed) m_xmf3Velocity.z = speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
}

void CPlayer::RotateToSetDirection(PLAYER_DIRECTION dir)
{
	return;
	if (dir == m_DirectionForRotate) {
		return;
	}
	float angle = 0.0f;
	switch (dir)
	{
	case PLAYER_DIRECTION::Front:
		if (PLAYER_DIRECTION::Back == m_DirectionForRotate) {
			angle = 180.0f;
		}
		else if (PLAYER_DIRECTION::Left == m_DirectionForRotate) {
			angle = 90.0f;
		} 
		else if (PLAYER_DIRECTION::Right == m_DirectionForRotate) {
			angle = -90.0f;
		}
		break;
	case PLAYER_DIRECTION::Back:
		if (PLAYER_DIRECTION::Front == m_DirectionForRotate) {
			angle = 180.0f;
		}
		else if (PLAYER_DIRECTION::Left == m_DirectionForRotate) {
			angle = -90.0f;
		}
		else if (PLAYER_DIRECTION::Right == m_DirectionForRotate) {
			angle = 90.0f;
		} 
		break;
	case PLAYER_DIRECTION::Left:
		if (PLAYER_DIRECTION::Front == m_DirectionForRotate) {
			angle = -90.0f;
		}
		else if (PLAYER_DIRECTION::Back == m_DirectionForRotate) {
			angle = 90.0f;
		}
		else if (PLAYER_DIRECTION::Right == m_DirectionForRotate) {
			angle = 180.0f;
		} 
		break;
	case PLAYER_DIRECTION::Right:
		if (PLAYER_DIRECTION::Front == m_DirectionForRotate) {
			angle = 90.0f;
		}
		else if (PLAYER_DIRECTION::Back == m_DirectionForRotate) {
			angle = -90.0f;
		}
		else if (PLAYER_DIRECTION::Left == m_DirectionForRotate) {
			angle = 180.0f;
		}
		break; 
	}
	Rotate(XMFLOAT3(0,1,0),angle);
	m_DirectionForRotate = dir;
}