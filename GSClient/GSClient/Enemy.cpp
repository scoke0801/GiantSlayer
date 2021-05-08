#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"

CEnemy::CEnemy()
{
	m_Type = OBJ_TYPE::Enemy;
	m_Sight = 20.f;
	m_Statemachine = new CStateMachine<CEnemy>(this);
	m_Statemachine->SetCurrentState(Wandering::Instance()); 
}
  
CEnemy::~CEnemy()
{

}

void CEnemy::Update(float elapsedTime)
{
	//m_Statemachine->Update(elapsedTime);
	if (false == m_IsOnMoving) {
		FindNextPosition(); 
		m_IsOnMoving = true;
	}
	else if(m_IsOnMoving){
		SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, 165.0f * elapsedTime)));
		m_ToMovePosition.y = m_xmf3Position.y;
		XMFLOAT3 gap = Vector3::Subtract(m_ToMovePosition, m_xmf3Position);
		if (Vector3::Length(gap) < 30) {
			SetPosition(m_ToMovePosition);
			m_IsOnMoving = false;
		}
	}
}

bool CEnemy::IsEnemyInSight() // Chase State
{ 
	return false;
}

void CEnemy::MoveRandom() // Moving State
{ 
}

void CEnemy::TrackingTarget()
{
	
}

void CEnemy::SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center)
{
	m_xmf3ActivityScope = xmf3ActivityScope;

	m_xmf3ActivityScopeCenter = xmf3Center;
}

void CEnemy::ConnectPlayer(CPlayer** pPlayers, int playerCount)
{
	for (int i = 0; i < playerCount; ++i) {
		m_ConnectedPlayers.push_back( pPlayers[i] );
	}
}

void CEnemy::FindNextPosition()
{
	m_xmf3ActivityScope;
	m_ToMovePosition.x = (((float)rand() / (RAND_MAX)) * (m_xmf3ActivityScope.x* 2 )) + m_xmf3ActivityScopeCenter.x - m_xmf3ActivityScope.x;
	m_ToMovePosition.y = m_xmf3Position.y;
	m_ToMovePosition.z = (((float)rand() / (RAND_MAX)) * (m_xmf3ActivityScope.z * 2)) + m_xmf3ActivityScopeCenter.z - m_xmf3ActivityScope.z;

	//cout << "목표위치 설정 ";
	DisplayVector3(m_ToMovePosition);
	m_xmf3Velocity = Vector3::Subtract(m_ToMovePosition, m_xmf3Position);
	m_xmf3Velocity = Vector3::Normalize(m_xmf3Velocity);

	XMFLOAT3 lookAt = Vector3::Normalize(GetLook()); 

	XMFLOAT3 cross = Vector3::CrossProduct(lookAt, m_xmf3Velocity);
	float dot = Vector3::DotProduct(lookAt, m_xmf3Velocity);

	float angle = atan2(Vector3::Length(cross), dot);

	float test = Vector3::DotProduct({ 0,1,0 }, cross);
	if (test < 0.0) angle = -angle; 

	//cout << "회전 각: " << angle << "\n";
	Rotate(XMFLOAT3(0, 1, 0), (XMConvertToDegrees( angle) ));
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