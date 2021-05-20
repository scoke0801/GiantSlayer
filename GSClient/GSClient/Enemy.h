#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"

// ���ʹ��� ����..
// Ž�� - Ȱ�� ������ Ž��
// :: if(Ž�� ����) toState(����)
// :: else ��� Ž��
// ����
// :: if(���� ����� Ȱ�� ���� �ȿ� �����ϸ�) ��� ����
// :: else toState(Ž��)
class CPlayer;

class CEnemy : public CGameObject
{
protected:
	CState<CEnemy>*			m_State;

	vector<CPlayer*>		m_ConnectedPlayers;
	 						
	USHORT					m_TargetPlayerIdx = -1; 
	bool					m_IsOnMoving = false;

	XMFLOAT3				m_ToMovePosition;

	// Ȱ�� ����
	XMFLOAT3				m_xmf3ActivityScopeCenter;
	XMFLOAT3				m_xmf3ActivityScope; 
	
	// Ȱ�� ���� �浹ü
	BoundingBox				m_ActivityRegionBox;

	// �þ� ���� �浹ü
	BoundingBox				m_SightBox;
	BoundingBox				m_SightAABB;

	float					m_AttackDelayTime = 0.0f;

public:
	CEnemy(); 
	~CEnemy();
	  
	virtual void Update(float elapsedTime) override;

	// Ȱ������ ����
	void SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center);
	XMFLOAT3 GetActivityScope() const { return m_xmf3ActivityScope; } 

	// �þ� ���� ����
	void SetSightBoundingBox(const XMFLOAT3& sight);

	// ���� ��� Ž�� 
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