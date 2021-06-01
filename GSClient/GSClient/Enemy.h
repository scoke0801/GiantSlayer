#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"
#include "GameObjectVer2.h"
// ���ʹ��� ����..
// Ž�� - Ȱ�� ������ Ž��
// :: if(Ž�� ����) toState(����)
// :: else ��� Ž��
// ����
// :: if(���� ����� Ȱ�� ���� �ȿ� �����ϸ�) ��� ����
// :: else toState(Ž��)
class CPlayer;

enum class EnemyAttackType {
	Melee,
	Ranged
};
class CEnemy : public CGameObjectVer2
{
protected:
	EnemyAttackType			m_AttackType;
	CState<CEnemy>*			m_State;

	vector<CPlayer*>		m_ConnectedPlayers;
	CPlayer*				m_TargetPlayer;

	bool					m_IsOnMoving = false;

	XMFLOAT3				m_ToMovePosition;

	// ���ݹ���
	float					m_AttackRange;

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
	void UpdateOnServer(float fTimeElapsed) override;

	// Ȱ������ ����
	void SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center);
	XMFLOAT3 GetActivityScope() const { return m_xmf3ActivityScope; } 

	// �þ� ���� ����
	void SetSightBoundingBox(const XMFLOAT3& sight);

	// ���� ��� Ž�� 
	bool IsEnemyInSight();

	void ConnectPlayer(CPlayer** pPlayers, int playerCount); 

	void ChangeState(CState<CEnemy>* nextState);

	virtual void FindNextPosition(); 
	virtual void FindClosePositionToTarget();

	void LookTarget(bool isRotatedModel);

	void MoveToNextPosition(float elapsedTime);

	void FixCollision(CGameObject* pCollideObject) override;

	virtual void Attack(float elapsedTime);
	 
	void ChangeState(ObjectState stateInfo, void* pData) override;
public:
	bool IsOnMoving() const { return m_IsOnMoving; }
	void SetIsOnMoving(bool info) { m_IsOnMoving = info; }
	 
	void SetAttackDelayTime(float delayTime) { m_AttackDelayTime = delayTime; }

	EnemyAttackType GetEnemyAttackType() const { return m_AttackType; } 

	float GetAttackRange() const { return m_AttackRange; }

	CPlayer* GetTargetPlayer() const { return m_TargetPlayer; }

	ObjectState GetStateInfo() const override{ return m_State->GetStatename(); }
};

class CMeleeEnemy : public CEnemy
{    
public:
	CMeleeEnemy(); 
	~CMeleeEnemy(); 

	void Attack(float elapsedTime) override;

	void FindNextPosition() override; 
	void FindClosePositionToTarget() override;
	//void Update(float elapsedTime) override {}
};

class CRangedEnemy : public CEnemy
{    
public:
	CRangedEnemy(); 
	~CRangedEnemy();

	void Attack(float elapsedTime) override;
	//void Update(float elapsedTime) override {}
};