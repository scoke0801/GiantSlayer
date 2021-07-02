#include "stdafx.h"
#include "Boss.h"

CBoss::CBoss()
{
	SetAnimationSet((int)BOSS_ANIMATION::Idle);
}

CBoss::~CBoss()
{
}

void CBoss::Update(float elapsedTime)
{
	CGameObjectVer2::Animate(elapsedTime);
	UpdateTransform(NULL);
}

void CBoss::UpdateOnServer(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CBoss::PlayerEnter(CPlayer* target)
{
	if (m_isOnAwaken == false) {
		SetAnimationSet((int)BOSS_ANIMATION::Born_1);
		m_isOnAwaken = true;
	}
}
