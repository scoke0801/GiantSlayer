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
    cout << "��Ȳ��..." << endl;
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
    cout << "�÷��̾� �߰�!" << endl;
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
    cout << "�÷��̾� ���� ����" << endl;
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
    cout << "�÷��̾� ���� ����" << endl;
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