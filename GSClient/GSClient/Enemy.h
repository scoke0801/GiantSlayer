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

