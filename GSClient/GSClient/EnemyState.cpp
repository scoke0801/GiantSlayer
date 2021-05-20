#include "stdafx.h"
#include "EnemyState.h"
#include "Enemy.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
  
void WaitState::Enter(CEnemy* pEnemy)
{
    cout << "WaitState::Enter" << endl;
}

void WaitState::Execute(CEnemy* pEnemy, float elapsedTime)
{
 /*   if (pEnemy->IsEnemyInSight())
        pEnemy->GetFSM()->ChangeState(Tracking::Instance());
    else
        pEnemy->MoveRandom();*/
}

void WaitState::Exit(CEnemy* pEnemy)
{
    cout << "WaitState::Exit" << endl;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
  
void IdleState::Enter(CEnemy* pEnemy)
{
    cout << " IdleState::Enter" << endl;
}

void IdleState::Execute(CEnemy* pEnemy, float elapsedTime)
{
    /*if (!pEnemy->IsEnemyInSight())
        pEnemy->GetFSM()->ChangeState(Wandering::Instance());
    else
        pEnemy->TrackingTarget();*/
}

void IdleState::Exit(CEnemy* pEnemy)
{
    cout << "IdleState::Exit" << endl;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
 
void RunAwayState::Enter(CEnemy* pEnemy)
{

}

void RunAwayState::Execute(CEnemy* pEnemy, float elapsedTime)
{
    
}

void RunAwayState::Exit(CEnemy* pEnemy)
{

}

void PatrolState::Enter(CEnemy* enemy)
{
}

void PatrolState::Execute(CEnemy* enemy, float elapsedTime)
{ 
	if (false == enemy->IsOnMoving()) {
		enemy->FindNextPosition();
		enemy->SetIsOnMoving(true);
	}
	else {
        enemy->PatrolToNextPosition(elapsedTime);
	}
    if (enemy->IsEnemyInSight()) {
        enemy->ChangeState(new AttackState(enemy));
    }
}

void PatrolState::Exit(CEnemy* enemy)
{
}

void AttackState::Enter(CEnemy* enemy)
{
    m_LifeTime = MELLE_ENEMY_ATTACK_TIME;
    enemy->SetAttackDelayTime(MELLE_ENEMY_ATTACK_TIME + 1.0f);
}

void AttackState::Execute(CEnemy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime; 
    if (m_LifeTime < m_ElapsedTime) {
        cout << "°ø°Ý ³¡ \n";
        enemy->ChangeState(new PatrolState(enemy));
    }
    else {

        float rotateAnglePerFrame = 360.0f / 2.0f;

        enemy->Rotate({ 0,0,1 }, rotateAnglePerFrame * elapsedTime);
    }
}

void AttackState::Exit(CEnemy* enemy)
{
}
