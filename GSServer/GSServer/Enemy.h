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

	// 공격범위
	float					m_AttackRange;

	// 활동 범위
	XMFLOAT3				m_xmf3ActivityScopeCenter;
	XMFLOAT3				m_xmf3ActivityScope;

	// 활동 범위 충돌체
	BoundingBox				m_ActivityRegionBox;

	// 시야 범위 충돌체
	BoundingBox				m_SightBox;
	BoundingBox				m_SightAABB;

	float					m_AttackDelayTime = 0.0f;

	float					m_Speed;

public:
	CEnemy();
	~CEnemy();

	virtual void Update(float elapsedTime) override;

	// 활동범위 설정
	void SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center);
	XMFLOAT3 GetActivityScope() const { return m_xmf3ActivityScope; }

	// 시야 범위 설정
	void SetSightBoundingBox(const XMFLOAT3& sight);


	void ConnectPlayer(CPlayer** pPlayers, int playerCount);

	void ChangeState(CState<CEnemy>* nextState);

	void FixCollision(CGameObject* pCollideObject) override;

	void ChangeState(ObjectState stateInfo, void* pData) override;

public:
	// 자식 클래스와 관련 코드
	virtual void Attack(float elapsedTime);

	virtual void FindNextPosition();
	virtual void FindClosePositionToTarget();

	void LookTarget(bool isRotatedModel);

	void MoveToNextPosition(float elapsedTime);

	// 공격 대상 탐색 
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
