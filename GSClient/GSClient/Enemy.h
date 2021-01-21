#pragma once
#include "GameObject.h"
#include "StateMachine.h"
#include "EnemyState.h"

struct Enemy_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

class CEnemy : public CGameObject
{
private:
	CStateMachine<CEnemy>* statemachine;

	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

public:
	CGameObject* target;

public:
	CEnemy(XMFLOAT3 pos);
	~CEnemy();

	void SetTarget(CGameObject* t) { target = t; }

	CStateMachine<CEnemy>* GetFSM()const { return statemachine; }

	virtual void Update();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void OnPrepareRender();


	bool IsEnemyInSight();

	void Walking();
};

