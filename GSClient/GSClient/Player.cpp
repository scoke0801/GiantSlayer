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
	//m_Camera->Update(m_xmf3Position, fTimeElapsed, this);
	//m_Camera->UpdateViewMatrix();

	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed);
	cout << "Length : " << fLength << " " << "Decl : " << fDeceleration << "\n";
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}
