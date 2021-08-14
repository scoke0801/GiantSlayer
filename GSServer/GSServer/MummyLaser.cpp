#include "stdafx.h"
#include "MummyLaser.h"

void CMummyLaser::Update(float fTimeElapsed)
{
	if (false == m_isDrawable) { 
		SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, ARROW_SPEED * fTimeElapsed)));
		Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), 5.0f); 
		m_ElapsedTime += fTimeElapsed;
	}
}
void CMummyLaser::SetTargetPosition(const XMFLOAT3& targetPos)
{
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Subtract(targetPos, m_xmf3Position));

	m_xmf3TargetPosition = targetPos;
	m_xmf3Velocity = dirVector;
}

void CMummyLaser::SetTargetVector(const XMFLOAT3& MummyLookAt)
{
	XMFLOAT3 dirVector = Vector3::Normalize(MummyLookAt);
	XMFLOAT3 targetPos = Vector3::Multifly(dirVector, 150000);
	m_xmf3Velocity = dirVector;
	LookAt(m_xmf3Position, targetPos, XMFLOAT3(0, 1, 0));
}

void CMummyLaser::SetDrawable(bool drawable)
{
	m_isDrawable = drawable;
	if (drawable) {
		//if (m_ConnectedParticle != nullptr) {
		//	m_ConnectedParticle->SetDrawable(false);
		//}
	}
}

void CMummyLaser::AddFriends_p(CMummy* mummy)
{
	m_Friends.push_back(mummy);
}

void CMummyLaser::SetDirecionVector(const XMFLOAT3& direction)
{
	XMFLOAT3 normalVec = Vector3::Normalize(direction);
	m_xmf4x4World._21 = normalVec.x;
	m_xmf4x4World._22 = normalVec.y;
	m_xmf4x4World._23 = normalVec.z;
}

void CMummyLaser::SetLaserType(Laser_TYPE laser)
{
	m_LaserType = laser;
}

void CMummyLaser::SetLaser(UINT laser)
{
	m_LaserIdx = laser;
}

void CMummyLaser::LookAtDirection(XMFLOAT3 dir, void* pContext)
{
	dir.y = 0;
	XMFLOAT3 normalizedDir = Vector3::Normalize(dir);
	XMFLOAT3 targetPosition = Vector3::Multifly(normalizedDir, 150000.0f);
	LookAt(m_xmf3Position, targetPosition, XMFLOAT3{ 0,1,0 });
}

void CMummyLaser::LookAtDirections(XMFLOAT3 dir)
{
	dir.y = 0;
	XMFLOAT3 normalizedDir = Vector3::Normalize(dir);
	XMFLOAT3 targetPosition = Vector3::Multifly(normalizedDir, 150000.0f);
	LookAt(m_xmf3Position, targetPosition, XMFLOAT3{ 0,1,0 });
}
