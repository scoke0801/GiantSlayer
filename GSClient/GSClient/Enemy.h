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
	CEnemy(XMFLOAT3 pos) {

		SetPosition(pos);

		statemachine = new CStateMachine<CEnemy>(this);
		statemachine->SetCurrentState(Wandering::Instance());
		target = nullptr;
	}

	void SetTarget(CGameObject* t) { target = t; }

	CStateMachine<CEnemy>* GetFSM()const { return statemachine; }

	virtual void Update();
	virtual void Draw(HDC hdc);

	void MoveRandom();

	bool IsEnemyInSight();

	void Walking();
};