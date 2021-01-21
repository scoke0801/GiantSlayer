#include "stdafx.h"
#include "Enemy.h"

CEnemy::CEnemy(XMFLOAT3 pos)
{
	SetPosition(pos);
	statemachine = new CStateMachine<CEnemy>(this);
	statemachine->SetCurrentState(Wandering::Instance());
	target = nullptr;
}

CEnemy::~CEnemy()
{

}

void CEnemy::Update()
{
	statemachine->Update();
}

void CEnemy::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}

void CEnemy::Draw(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CGameObject::Draw(pd3dCommandList, pCamera);
	/*TCHAR t[100];
	wsprintf(t, TEXT("%d , %d"), x, y);

	Rectangle(hdc, x - 50, y + 50, x + 50, y - 50);
	TextOut(hdc, x, y, t, lstrlen(t));

	if (target != nullptr) {
		wsprintf(t, TEXT("%d , %d"), target->x, target->y);
		TextOut(hdc, x, y + 10, t, lstrlen(t));
	}*/
}

bool CEnemy::IsEnemyInSight() // Chase State
{
	/*if (abs(x - target->x) <= 300 && abs(y - target->y) <= 300)
		return TRUE;
	else
		return FALSE;*/

	return false;
}

void CEnemy::Walking() // Moving State
{
	/*srand((unsigned int)time(NULL));

	int num = rand() % 5;

	switch (num)
	{
	case 0: x += 1; break;
	case 1: x -= 1; break;
	case 2: y += 1; break;
	case 3: y -= 1; break;
	case 4: break;
	}*/
}

