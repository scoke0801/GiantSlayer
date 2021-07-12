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
	Ranged,
	Mummy,
	BossSkill_1,
	BossSkill_2,
	BossSkill_3,
	BossSkill_4
};

enum class EnemyType {
	None = 0,
	Skeleton,
	Mummy,
	Boss,
};
class CEnemy : public CGameObjectVer2
{ 
protected: 
	EnemyAttackType			m_AttackType;
	CState<CEnemy>*			m_State;
	EnemyType				m_EnemyType;
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

	float					m_Speed;

	float					m_LaserAttackDelayTime[3] = { 3.0f,5.0f,7.0f };
	bool					m_LaserAttack[3] = { false,false,false };

public:
	CEnemy(); 
	~CEnemy();
	  
	void Update(float elapsedTime) override;
	void UpdateOnServer(float fTimeElapsed) override;

	// Ȱ������ ����
	void SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center);
	XMFLOAT3 GetActivityScope() const { return m_xmf3ActivityScope; } 

	// �þ� ���� ����
	void SetSightBoundingBox(const XMFLOAT3& sight);

	// ���� ��� Ž�� 
	bool IsEnemyInSight();

	void ConnectPlayer(CPlayer** pPlayers, int playerCount); 

	virtual void ChangeAnimation(ObjectState stateInfo);

	void ChangeState(CState<CEnemy>* nextState);

	virtual void FindNextPosition(); 
	virtual void FindClosePositionToTarget();

	void LookTarget(bool isRotatedModel);

	void MoveToNextPosition(float elapsedTime);

	void FixCollision(CGameObject* pCollideObject) override;

	virtual void Search(float elapsedTime) {}
	virtual void Attack(float elapsedTime);
	 
	void ChangeState(ObjectState stateInfo, void* pData) override;

	// ��ֹ��� �浹�Ͽ����Ƿ� Ư�� ���¿��� ���� ��˻��� �����϶�
	void CollideToObstacle();

public:
	bool IsOnMoving() const { return m_IsOnMoving; }
	void SetIsOnMoving(bool info) { m_IsOnMoving = info; }
	 
	void SetAttackDelayTime(float delayTime) { m_AttackDelayTime = delayTime; }

	void SetEnemyAttackType(EnemyAttackType attackType) { m_AttackType = attackType; }
	EnemyAttackType GetEnemyAttackType() const { return m_AttackType; } 

	float GetAttackRange() const { return m_AttackRange; }

	CPlayer* GetTargetPlayer() const { return m_TargetPlayer; }

	ObjectState GetStateInfo() const override{ return m_State->GetStatename(); }
	 
	EnemyType GetEnemyType() const { return m_EnemyType; }
	void SetEnemyType(EnemyType enemyType) { m_EnemyType = enemyType; } 
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

class CMummy :public CEnemy
{
public:
	CMummy();
	~CMummy();

	void Attack(float elapsedTime) override;
	void Update(float elapsedTime) override;
	bool isAttack0() const { return m_LaserAttack[0]; }
	bool isAttack1() const { return m_LaserAttack[1]; }
	bool isAttack2() const { return m_LaserAttack[2]; }
};