#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"

class CEnemy : public CGameObject
{
private:
	CStateMachine<CEnemy>* m_Statemachine;
	float m_Sight;

public:
	CGameObject* m_Target;

public:
	CEnemy();
	CEnemy(CGameObject* target);
	~CEnemy();

	void SetTarget(CGameObject* t) { m_Target = t; }

	CStateMachine<CEnemy>* GetFSM()const { return m_Statemachine; }

	virtual void Update(float elapsedTime) override;

	bool IsEnemyInSight();
	void MoveRandom();
	void TrackingTarget();
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