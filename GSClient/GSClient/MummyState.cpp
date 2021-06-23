#include "stdafx.h"
#include "MummyState.h"
#include "Mummy.h"
#include "Player.h"
#include "SceneYJ.h"



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
  // 0 idle, 1run 2attack, 3attacked, 4Death 
void WaitState_M::Enter(CMummy* pEnemy)
{
    pEnemy->SetAnimationSet(1);
    m_StateName = ObjectState::Wait;
    //cout << "WaitState::Enter" << endl;
}

void WaitState_M::Execute(CMummy* pEnemy, float elapsedTime)
{
}

void WaitState_M::Exit(CMummy* pEnemy)
{
    // cout << "WaitState::Exit" << endl;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void IdleState_M::Enter(CMummy* pEnemy)
{
    pEnemy->SetAnimationSet(1);
    m_StateName = ObjectState::Idle;
    // cout << " IdleState::Enter" << endl;
}

void IdleState_M::Execute(CMummy* pEnemy, float elapsedTime)
{
}

void IdleState_M::Exit(CMummy* pEnemy)
{
    // cout << "IdleState::Exit" << endl;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void RunAwayState_M::Enter(CMummy* pEnemy)
{
    pEnemy->SetAnimationSet(1);
    m_StateName = ObjectState::RunAway;
}

void RunAwayState_M::Execute(CMummy* pEnemy, float elapsedTime)
{

}

void RunAwayState_M::Exit(CMummy* pEnemy)
{

}

void PatrolState_M::Enter(CMummy* enemy)
{
    enemy->SetAnimationSet(1);
    m_StateName = ObjectState::Patrol;
}

void PatrolState_M::Execute(CMummy* enemy, float elapsedTime)
{
    enemy->SetAnimationSet(1);
    if (false == enemy->IsOnMoving()) {
        enemy->FindNextPosition();
        enemy->SetIsOnMoving(true);
    }
    else {
        enemy->MoveToNextPosition(elapsedTime);
    }
    if (enemy->IsMummyInSight()) {
        enemy->ChangeState(new TraceState_M(enemy));
    }
}

void PatrolState_M::Exit(CMummy* enemy)
{
    // cout << " PatrolState::Exit\n";
}

void AttackState_M::Enter(CMummy* enemy)
{
    m_StateName = ObjectState::Attack;
    //  cout << "AttackState::Enter \n";
    m_LifeTime = MELLE_ENEMY_ATTACK_TIME;
}

void AttackState_M::Execute(CMummy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime;
    if (m_LifeTime < m_ElapsedTime) {
        enemy->ChangeState(new PatrolState_M(enemy));
    }
    else {
        enemy->SetAnimationSet(2);
        enemy->Attack(elapsedTime);
    }
}

void AttackState_M::Exit(CMummy* enemy)
{
    enemy->SetIsOnMoving(false);
    // cout << "공격 끝 \n";
}

void TraceState_M::Enter(CMummy* enemy)
{
    enemy->SetAnimationSet(1);
    m_StateName = ObjectState::Trace;
    m_LifeTime = 0.5f;
    //   cout << "TraceState::Enter \n";
    m_AttackRange = enemy->GetAttackRange();
    m_TargetPlayer = enemy->GetTargetPlayer();
    if (m_TargetPlayer == nullptr) {
        enemy->ChangeState(new PatrolState_M(enemy));
    }
}

void TraceState_M::Execute(CMummy* enemy, float elapsedTime)
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
        enemy->LookTarget(enemy->GetEnemyAttackType() == MummyAttackType::Melee);
        enemy->ChangeState(new AttackState_M(enemy));
    }
    else {
        enemy->MoveToNextPosition(elapsedTime);
    }
}

void TraceState_M::Exit(CMummy* enemy)
{
    // cout << "TraceState::Exit\n";
}

void AttackedState_M::Enter(CMummy* enemy)
{
    enemy->SetAnimationSet(3);
    const int PLAYER_DAMAGE = 15;
    int hp = enemy->GetHP();
    hp -= PLAYER_DAMAGE;
    enemy->SetHP(hp);
    if (hp <= 0) {
        // cout << " 삭제 해 !!\n";
        MAIN_GAME_SCENE_Y->DeleteMummy(enemy);
        return;
    }
    //enemy->SetHP(hp);
}

void AttackedState_M::Execute(CMummy* enemy, float elapsedTime)
{
}

void AttackedState_M::Exit(CMummy* enemy)
{
}
