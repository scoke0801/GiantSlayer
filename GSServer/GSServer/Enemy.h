#pragma once
#include "GameObject.h"

#include "EnemyState.h"
class CPlayer;

enum class EnemyAttackType {
	Melee,
	Ranged,
	BossSkill_1,
	BossSkill_2,
	BossSkill_3,
	BossSkill_4
};

enum class EnemyType {
	None = 0,
	Skeleton,
	Mummy,
	Boss
};
class CEnemy : public CGameObject
{
protected:
	EnemyAttackType			m_AttackType;
	CState<CEnemy>*			m_State;

	EnemyType				m_EnemyType;

	vector<CPlayer*>		m_ConnectedPlayers;
	CPlayer* m_TargetPlayer;

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

public:
	CEnemy();
	~CEnemy();

	virtual void Update(float elapsedTime) override;

	// Ȱ������ ����
	void SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center);
	XMFLOAT3 GetActivityScope() const { return m_xmf3ActivityScope; }

	// �þ� ���� ����
	void SetSightBoundingBox(const XMFLOAT3& sight);


	void ConnectPlayer(CPlayer** pPlayers, int playerCount);

	void ChangeState(CState<CEnemy>* nextState);

	void FixCollision(CGameObject* pCollideObject) override;

	void ChangeState(ObjectState stateInfo, void* pData) override;

public:
	// �ڽ� Ŭ������ ���� �ڵ�
	virtual void Attack(float elapsedTime);

	virtual void FindNextPosition();
	virtual void FindClosePositionToTarget();

	void LookTarget(bool isRotatedModel);

	void MoveToNextPosition(float elapsedTime);

	// ���� ��� Ž�� 
	bool IsEnemyInSight();
public:
	bool IsOnMoving() const { return m_IsOnMoving; }
	void SetIsOnMoving(bool info) { m_IsOnMoving = info; }

	void SetAttackDelayTime(float delayTime) { m_AttackDelayTime = delayTime; }

	EnemyAttackType GetEnemyAttackType() const { return m_AttackType; }

	float GetAttackRange() const { return m_AttackRange; }

	CPlayer* GetTargetPlayer() const { return m_TargetPlayer; }

	ObjectState GetStateInfo() const override { return m_State->GetStatename(); }

	BYTE GetAnimationType() const override;
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
