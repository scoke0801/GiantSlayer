#include "stdafx.h"
#include "EnemyState.h"
#include "Enemy.h"
#include "Player.h"

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
        enemy->MoveToNextPosition(elapsedTime);
	}
    if (enemy->IsEnemyInSight()) {
        {
            enemy->ChangeState(new TraceState(enemy));
        } 
    }
}

void PatrolState::Exit(CEnemy* enemy)
{
    cout << " PatrolState::Exit\n";
}

void AttackState::Enter(CEnemy* enemy)
{
    cout << "AttackState::Enter \n";
    m_LifeTime = MELLE_ENEMY_ATTACK_TIME;
    enemy->SetAttackDelayTime(MELLE_ENEMY_ATTACK_TIME + 1.0f);
}

void AttackState::Execute(CEnemy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime; 
    if (m_LifeTime < m_ElapsedTime) {
        
        enemy->ChangeState(new PatrolState(enemy));
    }
    else {  
        enemy->Attack(elapsedTime);
    }
}

void AttackState::Exit(CEnemy* enemy)
{
    cout << "°ø°Ý ³¡ \n";
}

void TraceState::Enter(CEnemy* enemy)
{
    m_LifeTime = 0.5f;
    cout << "TraceState::Enter \n";
    m_AttackRange = enemy->GetAttackRange();
    m_TargetPlayer = enemy->GetTargetPlayer();
    if (m_TargetPlayer == nullptr) {
        enemy->ChangeState(new PatrolState(enemy));
    }
}

void TraceState::Execute(CEnemy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime;
    if (m_ElapsedTime > m_LifeTime) {
        enemy->FindClosePositionToTarget(); 
        m_ElapsedTime = 0.0f;
    }

    XMFLOAT3 targetPos = m_TargetPlayer->GetPosition();
    XMFLOAT3 enemyPos = enemy->GetPosition();

    enemyPos.y = targetPos.y;
    if (Vector3::Length(Vector3::Subtract(targetPos, enemyPos)) < m_AttackRange) {
        enemy->LookTarget(enemy->GetEnemyAttackType() == EnemyAttackType::Melee);
        enemy->ChangeState(new AttackState(enemy));
    }
    else {
        enemy->MoveToNextPosition(elapsedTime);
    }
}

void TraceState::Exit(CEnemy* enemy)
{
    cout << "TraceState::Exit\n";
}
