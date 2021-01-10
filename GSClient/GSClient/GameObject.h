#pragma once
#include "stdafx.h"
#include "State.h"
#include "StateMachine.h"

class CGameObject
{
private:
	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Velocity;
public:
	CGameObject();
	~CGameObject();

	void Update();
	void Draw();

	void SetPosition(XMFLOAT3 pos);
	void SetVelocity(XMFLOAT3 pos);

	void Move();

};

