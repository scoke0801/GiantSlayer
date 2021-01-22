#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"

class CEnemy : public CGameObject
{
private:
	CStateMachine<CEnemy>* statemachine;
	float sight;

public:
	CGameObject* target;

public:
	CEnemy();
	CEnemy(CGameObject* target);
	~CEnemy();

	void SetTarget(CGameObject* t) { target = t; }

	CStateMachine<CEnemy>* GetFSM()const { return statemachine; }

	virtual void Update();

	bool IsEnemyInSight();
	void MoveRandom();
	void TrackingTarget();
};

class CEnemyCloseATK : public CGameObject
{
private:
	CStateMachine<CEnemyCloseATK>* statemachine;
	float sight;

public:
	CGameObject* target;

public:
	CEnemyCloseATK();
	CEnemyCloseATK(CGameObject* target);
	~CEnemyCloseATK();

	void SetTarget(CGameObject* t) { target = t; }

	CStateMachine<CEnemyCloseATK>* GetFSM()const { return statemachine; }

	virtual void Update();

};

class CEnemyRangeATK : public CGameObject
{
private:
	CStateMachine<CEnemyRangeATK>* statemachine;
	float sight;

public:
	CGameObject* target;

public:
	CEnemyRangeATK();
	CEnemyRangeATK(CGameObject* target);
	~CEnemyRangeATK();

	void SetTarget(CGameObject* t) { target = t; }

	CStateMachine<CEnemyRangeATK>* GetFSM()const { return statemachine; }

	virtual void Update();

};