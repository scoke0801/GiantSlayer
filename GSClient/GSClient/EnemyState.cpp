#include "EnemyState.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

Wandering* Wandering::Instance()
{
    static Wandering instance;

    return &instance;
}

void Wandering::Enter(CEnemy* pCEnemy)
{

}

void Wandering::Execute(CEnemy* pCEnemy)
{
    if (pCEnemy->IsEnemyInSight())
        pCEnemy->GetFSM()->ChangeState(Tracking::Instance());
    else
        pCEnemy->MoveRandom();
}

void Wandering::Exit(CEnemy* pCEnemy)
{

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

Tracking* Tracking::Instance()
{
    static Tracking instance;

    return &instance;
}

void Tracking::Enter(CEnemy* pCEnemy)
{

}

void Tracking::Execute(CEnemy* pCEnemy)
{
    if (!pCEnemy->IsEnemyInSight())
        pCEnemy->GetFSM()->ChangeState(Wandering::Instance());
    else {
        XMFLOAT3 pos = pCEnemy->GetPosition();
        XMFLOAT3 tpos = pCEnemy->target->GetPosition();

        if (pos.x > tpos.x) pCEnemy->SetVelocity({ -2, 0, 0 });
        if (pos.x < tpos.x) pCEnemy->SetVelocity({ +2, 0, 0 });
        if (pos.y > tpos.y) pCEnemy->SetVelocity({ 0, -2, 0 });
        if (pos.y < tpos.y) pCEnemy->SetVelocity({ 0, +2, 0 });
    }
    pCEnemy->Move();
}

void Tracking::Exit(CEnemy* pCEnemy)
{

}