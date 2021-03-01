#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Type = OBJ_TYPE::Player;

	m_HP = 50;
	m_SP = 35;
}

CPlayer::~CPlayer()
{

}

void CPlayer::Update(double fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	float Friction = (m_MovingType == PlayerMoveType::Run) ? 200.0f : 50.0f;

	XMFLOAT3 vel = Vector3::Multifly(m_xmf3Velocity, fTimeElapsed);

	Move(vel);

	//Scale(m_xmf3Size.x * 0.01, m_xmf3Size.y * 0.01, m_xmf3Size.z * 0.01, false);

	m_Camera->Update(m_xmf3Position, fTimeElapsed);
	m_Camera->LookAt(m_Camera->GetPosition3f(), m_xmf3Position, GetUp());
	m_Camera->UpdateViewMatrix();

	//Scale(m_xmf3Size.x, m_xmf3Size.y, m_xmf3Size.z, false);

	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed); 
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

	static bool TestHPDown = false;
	static bool TestSPDown = false;
	if (TestHPDown) {
		m_HP -= 1;
		if (m_HP <= 0) TestHPDown = false;
	}
	else if(!TestHPDown)
	{
		m_HP += 1;
		if (m_HP >= 100) TestHPDown = true;
	}
	if (TestSPDown)
	{
		m_SP -= 1;
		if (m_SP <= 0) TestSPDown = false;
	}
	else if (!TestSPDown)
	{
		m_SP += 1;
		if (m_SP >= 100) TestSPDown = true;
	}
}

void CPlayer::SetVelocity(OBJ_DIRECTION direction)
{ 
	bool isMoving = IsMoving();
	if (!isMoving)
	{
		if (m_Camera != nullptr)
		{
			cout << "이동전 Look벡터 ";
			DisplayVector3(GetLook());
			DisplayVector3(m_Camera->GetLook3f());
			 
			XMFLOAT3 cameraLookAt = Vector3::Normalize(m_Camera->GetLook3f()); 
			XMFLOAT3 playerLookAt = Vector3::Normalize(GetLook());
	
			//float angle = Vector3::Angle(cameraLookAt, playerLookAt);
			float angle = Vector3::AngleAtan(cameraLookAt, playerLookAt) ;
			cout << "각도 : " << angle << "\n";
	
			Rotate(XMFLOAT3(0, 1, 0), XMConvertToRadians(-angle)); 
	
			cout <<  " 이동후Look벡터 ";
			DisplayVector3(GetLook());
		}
	}	
	XMFLOAT3 look = GetLook();// m_Camera->GetLook3f(); 
	XMFLOAT3 right = GetRight();//m_Camera->GetRight3f();
	look = Vector3::Normalize(look);
	right = Vector3::Normalize(right);
	switch (direction)
	{
	case OBJ_DIRECTION::Front:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(look, PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Back:
		//look.z *= -1;
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(look, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Left:
		//right.x *= -1;
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(right, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Right:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(right, PLAYER_RUN_VELOCITY));
		break;
	default:
		assert("잘못된 방향으로 이동할 수 없어요~");
		break;

	}
	float speed = m_MovingType == (PlayerMoveType::Run) ? PLAYER_RUN_VELOCITY : PLAYER_WALK_VELOCITY;
	if (m_xmf3Velocity.x > speed) m_xmf3Velocity.x = speed;
	if (m_xmf3Velocity.y > speed) m_xmf3Velocity.y = speed;
	if (m_xmf3Velocity.z > speed) m_xmf3Velocity.z = speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
}
