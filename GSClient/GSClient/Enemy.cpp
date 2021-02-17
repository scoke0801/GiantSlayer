#include "stdafx.h"
#include "Enemy.h"

CEnemy::CEnemy()
{
	sight = 20.f;
	statemachine = new CStateMachine<CEnemy>(this);
	statemachine->SetCurrentState(Wandering::Instance());
	target = nullptr;
}

CEnemy::CEnemy(CGameObject* ptarget)
{
	sight = 20.f;
	statemachine = new CStateMachine<CEnemy>(this);
	statemachine->SetCurrentState(Wandering::Instance());
	target = ptarget;
}

CEnemy::~CEnemy()
{

}

void CEnemy::Update(double elapsedTime)
{
	statemachine->Update(elapsedTime);
}

bool CEnemy::IsEnemyInSight() // Chase State
{
	XMFLOAT3 tpos = target->GetPosition();

	if (abs(m_xmf4x4World._41 - tpos.x) <= sight && 
		abs(m_xmf4x4World._43 - tpos.z) <= sight)
		return TRUE;
	else
		return FALSE;

	return false;
}

void CEnemy::MoveRandom() // Moving State
{
	srand((unsigned int)time(NULL));

	int num = rand() % 5;

	float x = 0;
	float z = 0;

	switch (num)
	{
	case 0: x = +0.1; break;
	case 1: x = -0.1; break;
	case 2: z = -0.1; break;
	case 3: z = +0.1; break;
	case 4: break;
	}

	Move({x, 0, z});
}

void CEnemy::TrackingTarget()
{
	XMFLOAT3 tpos = target->GetPosition();
		
	if (m_xmf4x4World._41 > tpos.x) Move({ -0.1, 0, 0 });
	if (m_xmf4x4World._41 < tpos.x) Move({ +0.1, 0, 0 });
	if (m_xmf4x4World._43 > tpos.z) Move({ 0, 0, -0.1 });
	if (m_xmf4x4World._43 < tpos.z) Move({ 0, 0, +0.1 });
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CEnemyRangeATK::CEnemyRangeATK()
{

}

CEnemyRangeATK::CEnemyRangeATK(CGameObject* ptarget)
{

}

CEnemyRangeATK::~CEnemyRangeATK()
{

}

void CEnemyRangeATK::Update(double elapsedTime)
{
	statemachine->Update(elapsedTime);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CEnemyCloseATK::CEnemyCloseATK()
{

}

CEnemyCloseATK::CEnemyCloseATK(CGameObject* ptarget)
{

}

CEnemyCloseATK::~CEnemyCloseATK()
{

}

void CEnemyCloseATK::Update(double elapsedTime)
{
	statemachine->Update(elapsedTime);
}