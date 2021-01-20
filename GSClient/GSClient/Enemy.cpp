#include "stdafx.h"
#include "Enemy.h"

void CEnemy::Update()
{
	statemachine->Update();
}

//void CEnemy::Draw(HDC hdc)
//{
//	/*TCHAR t[100];
//	wsprintf(t, TEXT("%d , %d"), x, y);
//
//	Rectangle(hdc, x - 50, y + 50, x + 50, y - 50);
//	TextOut(hdc, x, y, t, lstrlen(t));
//
//	if (target != nullptr) {
//		wsprintf(t, TEXT("%d , %d"), target->x, target->y);
//		TextOut(hdc, x, y + 10, t, lstrlen(t));
//	}*/
//}

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

