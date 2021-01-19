#include "stdafx.h"
#include "Enemy.h"

void CEnemy::Update()
{
	statemachine->Update();
}

void CEnemy::Draw(HDC hdc)
{
	TCHAR t[100];
	XMFLOAT3 pos = GetPosition();

	wsprintf(t, TEXT("%d , %d"), pos.x, pos.y);

	Rectangle(hdc, pos.x - 50, pos.y + 50, pos.x + 50, pos.y - 50);
	TextOut(hdc, pos.x, pos.y, t, lstrlen(t));

	if (target != nullptr) {
		XMFLOAT3 tpos = target->GetPosition();
		wsprintf(t, TEXT("%d , %d"), tpos.x, tpos.y);
		TextOut(hdc, pos.x, pos.y + 15, t, lstrlen(t));
	}
}

bool CEnemy::IsEnemyInSight() // Chase State
{
	XMFLOAT3 pos = GetPosition();
	XMFLOAT3 tpos = target->GetPosition();

	if (abs(pos.x - tpos.x) <= 300 && abs(pos.y - tpos.y) <= 300)
		return TRUE;
	else
		return FALSE;
}

void CEnemy::MoveRandom()
{
	srand((unsigned int)time(NULL));

	int num = rand() % 5;

	switch (num)
	{
	case 0:
		SetVelocity({ +1, 0, 0 });
		break;
	case 1:
		SetVelocity({ -1, 0, 0 });
		break;
	case 2:
		SetVelocity({ 0, +1, 0 });
		break;
	case 3:
		SetVelocity({ 0, -1, 0 });
		break;
	case 4:
		break;
	}

	Move();
}

void CEnemy::Walking() // Moving State
{
	srand((unsigned int)time(NULL));

	int num = rand() % 5;

	switch (num)
	{
	case 0: 
		SetVelocity({+1, 0, 0});
		break;
	case 1: 
		SetVelocity({ -1, 0, 0 });
		break;
	case 2: 
		SetVelocity({ 0, +1, 0 });
		break;
	case 3: 
		SetVelocity({ 0, -1, 0 });
		break;
	case 4: 
		break;
	}

	Move();
}