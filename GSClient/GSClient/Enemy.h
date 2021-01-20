#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"

class CEnemy : public CGameObject
{
private:
	CStateMachine<CEnemy>* statemachine;

public:
	CGameObject* target;
public:
	CEnemy(int a, int b) {
		//x = a;
		//y = b;

		statemachine = new CStateMachine<CEnemy>(this);
		statemachine->SetCurrentState(Wandering::Instance());
		target = nullptr;
	}

	void SetTarget(CGameObject* t) { target = t; }

	CStateMachine<CEnemy>* GetFSM()const { return statemachine; }

	virtual void Update();
	//virtual void Draw(HDC hdc);

	bool IsEnemyInSight();

	void Walking();
};

