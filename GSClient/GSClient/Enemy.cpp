#include "stdafx.h"
#include "Enemy.h"

CEnemy::CEnemy()
{
	m_Type = OBJ_TYPE::Enemy;
	m_Sight = 20.f;
	m_Statemachine = new CStateMachine<CEnemy>(this);
	m_Statemachine->SetCurrentState(Wandering::Instance());
	m_Target = nullptr;
}

CEnemy::CEnemy(CGameObject* ptarget)
{
	m_Type = OBJ_TYPE::Enemy;
	m_Sight = 20.f;
	m_Statemachine = new CStateMachine<CEnemy>(this);
	m_Statemachine->SetCurrentState(Wandering::Instance());
	m_Target = ptarget;
}

CEnemy::~CEnemy()
{

}

void CEnemy::Update(float elapsedTime)
{
	m_Statemachine->Update(elapsedTime);
}

bool CEnemy::IsEnemyInSight() // Chase State
{
	XMFLOAT3 tpos = m_Target->GetPosition();

	if (abs(m_xmf4x4World._41 - tpos.x) <= m_Sight && 
		abs(m_xmf4x4World._43 - tpos.z) <= m_Sight)
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
	XMFLOAT3 tpos = m_Target->GetPosition();
		
	if (m_xmf4x4World._41 > tpos.x) Move({ -0.1, 0, 0 });
	if (m_xmf4x4World._41 < tpos.x) Move({ +0.1, 0, 0 });
	if (m_xmf4x4World._43 > tpos.z) Move({ 0, 0, -0.1 });
	if (m_xmf4x4World._43 < tpos.z) Move({ 0, 0, +0.1 });
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CEnemyRangeATK::CEnemyRangeATK()
{
	m_Type = OBJ_TYPE::Enemy;
}

CEnemyRangeATK::CEnemyRangeATK(CGameObject* ptarget)
{
	m_Type = OBJ_TYPE::Enemy;
}

CEnemyRangeATK::~CEnemyRangeATK()
{

}

void CEnemyRangeATK::Update(double elapsedTime)
{
	m_Statemachine->Update(elapsedTime);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CEnemyCloseATK::CEnemyCloseATK()
{
	m_Type = OBJ_TYPE::Enemy;
}

CEnemyCloseATK::CEnemyCloseATK(CGameObject* ptarget)
{
	m_Type = OBJ_TYPE::Enemy;
}

CEnemyCloseATK::~CEnemyCloseATK()
{

}

void CEnemyCloseATK::Update(float elapsedTime)
{
	m_Statemachine->Update(elapsedTime);
}