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
    cout << "방황중..." << endl;
}

void Wandering::Execute(CEnemy* pEnemy)
{
    if (pEnemy->IsEnemyInSight())
        pEnemy->GetFSM()->ChangeState(Tracking::Instance());
    else
        pEnemy->MoveRandom();
}

void Wandering::Exit(CEnemy* pEnemy)
{
    cout << "플레이어 발견!" << endl;
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
    cout << "플레이어 추적 시작" << endl;
}

void Tracking::Execute(CEnemy* pEnemy)
{
    if (!pEnemy->IsEnemyInSight())
        pEnemy->GetFSM()->ChangeState(Wandering::Instance());
    else
        pEnemy->TrackingTarget();
}

void Tracking::Exit(CEnemy* pEnemy)
{
    cout << "플레이어 추적 종료" << endl;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

RunAway* RunAway::Instance()
{
    static RunAway instance;

    return &instance;
}

void RunAway::Enter(CEnemy* pEnemy)
{

}

void RunAway::Execute(CEnemy* pEnemy)
{
    
}

void RunAway::Exit(CEnemy* pEnemy)
{

}