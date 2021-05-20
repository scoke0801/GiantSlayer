#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"

// 에너미의 상태..
// 탐색 - 활동 범위를 탐색
// :: if(탐색 성공) toState(공격)
// :: else 계속 탐색
// 공격
// :: if(공격 대상이 활동 범위 안에 존재하면) 계속 공격
// :: else toState(탐색)
class CPlayer;

class CEnemy : public CGameObject
{
protected:
	CState<CEnemy>*			m_State;

	vector<CPlayer*>		m_ConnectedPlayers;
	 						
	USHORT					m_TargetPlayerIdx = -1; 
	bool					m_IsOnMoving = false;

	XMFLOAT3				m_ToMovePosition;

	// 활동 범위
	XMFLOAT3				m_xmf3ActivityScopeCenter;
	XMFLOAT3				m_xmf3ActivityScope; 
	
	// 활동 범위 충돌체
	BoundingBox				m_ActivityRegionBox;

	// 시야 범위 충돌체
	BoundingBox				m_SightBox;
	BoundingBox				m_SightAABB;

	float					m_AttackDelayTime = 0.0f;

public:
	CEnemy(); 
	~CEnemy();
	  
	virtual void Update(float elapsedTime) override;

	// 활동범위 설정
	void SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center);
	XMFLOAT3 GetActivityScope() const { return m_xmf3ActivityScope; } 

	// 시야 범위 설정
	void SetSightBoundingBox(const XMFLOAT3& sight);

	// 공격 대상 탐색 
	bool IsEnemyInSight();

	void ConnectPlayer(CPlayer** pPlayers, int playerCount); 

	void ChangeState(CState<CEnemy>* nextState);

	void FindNextPosition();

	void PatrolToNextPosition(float elapsedTime);

	void FixCollision();

public:
	bool IsOnMoving() const { return m_IsOnMoving; }
	void SetIsOnMoving(bool info) { m_IsOnMoving = info; }
	 
	void SetAttackDelayTime(float delayTime) { m_AttackDelayTime = delayTime; }

private:

};

class CEnemyCloseATK : public CGameObject
{
private:
	CStateMachine<CEnemyCloseATK>* m_Statemachine;
	float m_Sight;

public:
	CGameObject* m_Target;

public:
	CEnemyCloseATK();
	CEnemyCloseATK(CGameObject* target);
	~CEnemyCloseATK();

	void SetTarget(CGameObject* t) { m_Target = t; }

	CStateMachine<CEnemyCloseATK>* GetFSM()const { return m_Statemachine; }

	virtual void Update(float elapsedTime) override; 
};

class CEnemyRangeATK : public CGameObject
{
private:
	CStateMachine<CEnemyRangeATK>* m_Statemachine;
	float sight;

public:
	CGameObject* target;

public:
	CEnemyRangeATK();
	CEnemyRangeATK(CGameObject* target);
	~CEnemyRangeATK();

	void SetTarget(CGameObject* t) { target = t; }

	CStateMachine<CEnemyRangeATK>* GetFSM()const { return m_Statemachine; }

	virtual void Update(double elapsedTime); 
};