#include "stdafx.h"
#include "FireBall.h"


CFireBall::CFireBall()
{
	m_isDrawable = true;
	m_ATK = 35;
}

CFireBall::~CFireBall()
{

}


void CFireBall::Update(float fTimeElapsed)
{
	if (false == m_isDrawable) {
		m_ElapsedTime += fTimeElapsed;

		if (m_ElapsedTime > FIREBALL_LIFE_TIME) {
			m_ElapsedTime = 0.0f;
			m_isDrawable = true;

			XMFLOAT3 effectPos = m_xmf3Position;
			/*auto curScene = MAIN_GAME_SCENE;

			curScene->UseEffects((int)EffectTypes::FireBallExplosion, effectPos);*/
		}

		SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, FIREBALL_SPEED * fTimeElapsed)));
		Rotate(XMFLOAT3(0.3f, 1.0f, 0.0f), 360.0f * fTimeElapsed);
	}
}

void CFireBall::SetSkill(CGameObject* owner)
{
	m_Owner = owner;

	XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Owner->GetPosition() }, { 0,180,0 });
	SetPosition(pos);
	 
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Multifly(m_Owner->GetLook(), 1));
	XMFLOAT3 targetPos = Vector3::Multifly(dirVector, 150000);
	m_xmf3Velocity = dirVector;
	LookAt(m_xmf3Position, targetPos, XMFLOAT3(0, 1, 0));
}

void CFireBall::SetTargetPosition(const XMFLOAT3& targetPos)
{
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Subtract(targetPos, m_xmf3Position));

	m_xmf3TargetPosition = targetPos;
	m_xmf3Velocity = dirVector;
}

void CFireBall::TrackingTarget(CGameObject target)
{

}

void CFireBall::SetDrawable(bool drawable)
{
	m_isDrawable = drawable; 
}