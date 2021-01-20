#include "stdafx.h"
#include "EnemyState.h"
#include "Enemy.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

Wandering* Wandering::Instance()
{
    static Wandering instance;

    return &instance;
}

void Wandering::Enter(CEnemy* pEnemy)
{

}

void Wandering::Execute(CEnemy* pEnemy)
{
    if (pEnemy->IsEnemyInSight())
        pEnemy->GetFSM()->ChangeState(Tracking::Instance());
    else
        pEnemy->Walking();
}

void Wandering::Exit(CEnemy* pEnemy)
{

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

Tracking* Tracking::Instance()
{
    static Tracking instance;

    return &instance;
}

void Tracking::Enter(CEnemy* pEnemy)
{

}

void Tracking::Execute(CEnemy* pEnemy)
{
    /*if (!pEnemy->IsEnemyInSight())
        pEnemy->GetFSM()->ChangeState(Wandering::Instance());
    else {
        if (pEnemy->x > pEnemy->target->x) pEnemy->x -= 2;
        if (pEnemy->x < pEnemy->target->x) pEnemy->x += 2;
        if (pEnemy->y > pEnemy->target->y) pEnemy->y -= 2;
        if (pEnemy->y < pEnemy->target->y) pEnemy->y += 2;
    }*/
}

void Tracking::Exit(CEnemy* pEnemy)
{

}