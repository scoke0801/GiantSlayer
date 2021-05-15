#include "stdafx.h"
#include "Player.h" 

CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{

}

void CPlayer::Update(float fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	float Friction = (m_MovingType == Player_Move_Type::Run) ? 360.0f : 50.0f;

	XMFLOAT3 vel = Vector3::Multifly(m_xmf3Velocity, fTimeElapsed);

	Move(vel);  

	UpdateCamera();
	//cout << "pos : " << m_xmf3Position.x << " , " << m_xmf3Position.y << " " << m_xmf3Position.z << " \n";
	
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

void CPlayer::FixCameraByTerrain(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1])
{
	XMFLOAT3 xmf3CameraPosition = m_Camera->GetPosition3f();
	  
	float offsetHeight = m_Camera->GetOffset().y;
	//float fHeight = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z) + 5.0f;	 
	float fHeight = GetDetailHeight(heightsMap, xmf3CameraPosition.x, xmf3CameraPosition.z);
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_Camera->SetPosition(xmf3CameraPosition);
		m_Camera->LookAt(m_xmf3Position, GetUp());
	}
}

void CPlayer::FixPositionByTerrain(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1])
{
	m_xmf3Position.y = GetDetailHeight(heightsMap, m_xmf3Position.x, m_xmf3Position.z);
}

void CPlayer::SetVelocity(OBJ_DIRECTION direction)
{   
	XMFLOAT3 look = GetLook();// m_Camera->GetLook3f(); 
	XMFLOAT3 right = GetRight();//m_Camera->GetRight3f();
	look = Vector3::Normalize(look);
	right = Vector3::Normalize(right);
	switch (direction)
	{
	case OBJ_DIRECTION::Front:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(XMFLOAT3{0.0f, 0.0f, 1.0f}, PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Back:
		//look.z *= -1;
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(XMFLOAT3{ 0.0f, 0.0f, -1.0f }, PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Left:
		//right.x *= -1;
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(XMFLOAT3{ -1.0f, 0.0f, 0.0f }, PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Right:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(XMFLOAT3{ 1.0f, 0.0f, 1.0f }, PLAYER_RUN_VELOCITY));
		break;
	default:
		assert("잘못된 방향으로 이동할 수 없어요~");
		break;

	}
	float speed = m_MovingType == (Player_Move_Type::Run) ? PLAYER_RUN_VELOCITY  : PLAYER_WALK_VELOCITY;
	if (m_xmf3Velocity.x > speed) m_xmf3Velocity.x = speed;
	if (m_xmf3Velocity.y > speed) m_xmf3Velocity.y = speed;
	if (m_xmf3Velocity.z > speed) m_xmf3Velocity.z = speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
}
