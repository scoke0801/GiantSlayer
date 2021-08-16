#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"
#include "GameObjectVer2.h"
#include "SceneYJ.h"
#include "Player.h"
#include "MummyLaser.h"
#include "Particle.h"
// 에너미의 상태..
// 탐색 - 활동 범위를 탐색
// :: if(탐색 성공) toState(공격)
// :: else 계속 탐색
// 공격
// :: if(공격 대상이 활동 범위 안에 존재하면) 계속 공격
// :: else toState(탐색)
class CPlayer;
class CSceneYJ;
class CParticle;

 
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
	CPlayer*				m_TargetPlay=nullptr;

	bool					m_IsOnMoving = false;
	bool					m_Dying = FALSE;

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

	float					m_Mummy_Die;

	float					m_LaserAttackDelayTime[3] = { 7.0f,9.0f,11.0f };
	bool					m_LaserAttack[3] = { false,false,false };
	
public:
	CEnemy(); 
	~CEnemy();
	  
	void Update(float elapsedTime) override;
	void UpdateOnServer(float fTimeElapsed) override;

	// 활동범위 설정
	void SetActivityScope(const XMFLOAT3& xmf3ActivityScope, const XMFLOAT3& xmf3Center);
	XMFLOAT3 GetActivityScope() const { return m_xmf3ActivityScope; } 


	// 시야 범위 설정
	void SetSightBoundingBox(const XMFLOAT3& sight);

	// 공격 대상 탐색 
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

	// 장애물과 충돌하였으므로 특정 상태에서 상태 재검색을 수행하라
	void CollideToObstacle();

public:
	bool IsOnMoving() const { return m_IsOnMoving; }
	void SetIsOnMoving(bool info) { m_IsOnMoving = info; }
	 
	void SetAttackDelayTime(float delayTime) { m_AttackDelayTime = delayTime; }

	void SetEnemyAttackType(EnemyAttackType attackType) { m_AttackType = attackType; }
	EnemyAttackType GetEnemyAttackType() const { return m_AttackType; } 

	float GetAttackRange() const { return m_AttackRange; }

	void SetTargetPlayer(CPlayer* target) { m_TargetPlayer = target; }
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

class CNpc : public CEnemy
{
public:
	CNpc();
	~CNpc();
};

class CMummy :public CEnemy
{
private:
	vector<CMummy*> m_Friends;
	vector<CMummyLaser*> m_Friends_Laser;

	bool PlayerCheck = false;
	bool LaserDelayTime[3] = { false,false,false };
	bool shotLaser[3] = { false,false,false };
	float					dir[3] = { 15.0f,15.0f,15.0f };
	bool ScaleCheck = false;
	bool MummyScale[3] = { false,false,false };
	bool MummyDie[3] = { false,false,false };

public:

	CParticle* m_Particles;
	
	array<vector<CGameObject*>, (int)OBJECT_LAYER::Count> m_ObjectLayers;

public: 
	CMummy();
	~CMummy();

	void Draw_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override; 
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL) override;
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

