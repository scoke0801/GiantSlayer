#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		 50.0f, 165.0f, 50.0f );

	SetMesh(pCubeMeshTex);
}

CPlayer::~CPlayer()
{

}

void CPlayer::Update(double fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	float Friction = (m_MovingType == Player_Move_Type::Run) ? 200.0f : 50.0f;

	XMFLOAT3 vel = Vector3::Multifly(m_xmf3Velocity, fTimeElapsed);
	Move(vel);

	//m_Camera->Update(m_xmf3Position, fTimeElapsed);
	//m_Camera->SetLookAt(m_xmf3Position);
	//m_Camera->RegenerateViewMatrix();

	m_Camera->Update(m_xmf3Position, fTimeElapsed);
	 
	m_Camera->LookAt(m_Camera->GetPosition3f(), m_xmf3Position, GetUp());
	m_Camera->UpdateViewMatrix();

	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed); 
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPlayer::SetVelocity(OBJ_DIRECTION direction)
{
	XMFLOAT3 look = GetLook();
	XMFLOAT3 right = GetRight();
	bool isMoving = IsMoving();
	if (!isMoving)
	{
		if (m_Camera != nullptr)
		{
			cout << " 지 나 감\n";
			XMFLOAT3 xmf3Right = m_Camera->CalcTargetRight();
			XMFLOAT3 xmf3Up    = m_Camera->CalcTargetUp();
			XMFLOAT3 xmf3Look  = m_Camera->CalcTargetLook();
			m_xmf4x4World._11 = xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = xmf3Right.z;
			//m_xmf4x4World._21 = 0;    m_xmf4x4World._22 = 1;	  m_xmf4x4World._23 =0;
			m_xmf4x4World._31 = xmf3Look.x;  m_xmf4x4World._32 = xmf3Look.y;  m_xmf4x4World._33 = xmf3Look.z;
		}
	}
	switch (direction)
	{
	case OBJ_DIRECTION::Front:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(look, PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Back:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(look, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Left:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(right, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Right:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(right, PLAYER_RUN_VELOCITY));
		break;
	default:
		assert("잘못된 방향으로 이동할 수 없어요~");
		break;
	}
	float speed = m_MovingType == (Player_Move_Type::Run) ? PLAYER_RUN_VELOCITY : PLAYER_WALK_VELOCITY;
	if (m_xmf3Velocity.x > speed) m_xmf3Velocity.x = speed;
	if (m_xmf3Velocity.y > speed) m_xmf3Velocity.y = speed;
	if (m_xmf3Velocity.z > speed) m_xmf3Velocity.z = speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
}
