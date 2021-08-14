#pragma once 
#include "AnimationObject.h"
#include "EnemyState.h"
class CPlayer;

enum class EnemyAttackType {
	Melee,
	Ranged,
	Mummy1,
	Mummy2,
	Mummy3,
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
class CEnemy : public CAnimationObject
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

	void ChangeAnimation(ObjectState stateInfo);
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

	void SetEnemyAttackType(EnemyAttackType attackType) { m_AttackType = attackType; }
	EnemyAttackType GetEnemyAttackType() const { return m_AttackType; }

	float GetAttackRange() const { return m_AttackRange; }

	CPlayer* GetTargetPlayer() const { return m_TargetPlayer; }

	ObjectState GetStateInfo() const override { return m_State->GetStatename(); }

	BYTE GetAnimationType() const override;

	EnemyType GetEnemyType() const { return m_EnemyType; }
	void SetEnemyType(EnemyType enemyType) { m_EnemyType = enemyType; }

	bool IsCanDamaged() { return m_isCanDamaged; }
	void SetCanDamaged(bool param) { m_isCanDamaged = param; } 

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

class CNpc : public CEnemy
{
public:
	CNpc();
	~CNpc();
};

class CMummyLaser;
class CParticle;

class CMummy :public CEnemy
{
private:
	vector<CMummy*> m_Friends;
	vector<CMummyLaser*> m_Friends_Laser;

	bool PlayerCheck = false;
	bool LaserDelayTime[3] = { false,false,false };
	bool shotLaser[3] = { false,false,false };
	float dir[3] = { 15.0f,15.0f,15.0f };
	bool ScaleCheck = false;
	bool MummyScale[3] = { false,false,false };
	bool MummyDie[3] = { false,false,false };
	 
	float					m_Mummy_Die;

	float					m_LaserAttackDelayTime[3] = { 7.0f,9.0f,11.0f };
	bool					m_LaserAttack[3] = { false,false,false };

public: 
	CParticle* m_Particles;
	 
public: 
	CMummy();
	~CMummy();

	void Attack(float elapsedTime) override;
	void Update(float elapsedTime) override;
	void AddFriends(CMummy* mummy);
	void AddFriends_Laser(CMummyLaser* Laser);
	void SendDieInfotoFriends();
	void RemoveFriends(CMummy* mummy);
	void OneDie();

	void SetMummyDie(int die) { MummyDie[0] = die; };
	int GetMummyDie() const { return MummyDie[0]; };
	void SetMummyDie2(int die) { MummyDie[1] = die; };
	int GetMummyDie2() const { return MummyDie[1]; };
	void SetMummyDie3(int die) { MummyDie[2] = die; };
	int GetMummyDie3() const { return MummyDie[2]; };
};

