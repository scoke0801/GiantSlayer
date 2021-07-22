#include "stdafx.h"
#include "EnemyState.h"
#include "Enemy.h"
#include "Player.h"
#include "SceneJH.h"
#include "SceneYJ.h"
#include "Boss.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// 0 idle, 1run 2attack, 3attacked, 4Death 
void WaitState::Enter(CEnemy* pEnemy)
{
    pEnemy->ChangeAnimation(ObjectState::Idle);
    m_StateName = ObjectState::Wait;
    //cout << "WaitState::Enter" << endl;
}

void WaitState::Execute(CEnemy* pEnemy, float elapsedTime)
{
    if (pEnemy->IsEnemyInSight())
    { 
        pEnemy->ChangeState(new BornState(pEnemy));
    }
}

void WaitState::Exit(CEnemy* pEnemy)
{
   // cout << "WaitState::Exit" << endl;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
  
void IdleState::Enter(CEnemy* pEnemy)
{ 
    pEnemy->ChangeAnimation(ObjectState::Idle);
    m_StateName = ObjectState::Idle;
   // cout << " IdleState::Enter" << endl;
}

void IdleState::Execute(CEnemy* pEnemy, float elapsedTime)
{ 
    EnemyType type = pEnemy->GetEnemyType();
    switch (type)
    {
    case EnemyType::None:
        break;
    case EnemyType::Skeleton:
        break;
    case EnemyType::Mummy:
        break;
    case EnemyType::Boss:
    {

    } 
    break;
    default:
        break;
    }
}

void IdleState::Exit(CEnemy* pEnemy)
{
   // cout << "IdleState::Exit" << endl;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
 
void RunAwayState::Enter(CEnemy* pEnemy)
{
    pEnemy->SetAnimationSet(1);
    m_StateName = ObjectState::RunAway; 
}

void RunAwayState::Execute(CEnemy* pEnemy, float elapsedTime)
{
    
}

void RunAwayState::Exit(CEnemy* pEnemy)
{

}

void PatrolState::Enter(CEnemy* enemy)
{
    cout << " PatrolState::Enter\n";
    enemy->ChangeAnimation(ObjectState::Patrol);
    //enemy->SetAnimationSet(1);
    m_StateName = ObjectState::Patrol;
}

void PatrolState::Execute(CEnemy* enemy, float elapsedTime)
{
    enemy->ChangeAnimation(ObjectState::Patrol);
	if (false == enemy->IsOnMoving()) {
		enemy->FindNextPosition();
		enemy->SetIsOnMoving(true);
	}
	else {
        enemy->MoveToNextPosition(elapsedTime);
	}
    if (enemy->IsEnemyInSight()) {
        if (enemy->GetEnemyType() == EnemyType::Boss) {
            CBoss* pBoss = reinterpret_cast<CBoss*>(enemy);
            pBoss->CalcNextAttackType();
        }
        enemy->ChangeState(new TraceState(enemy));
    }

    CMummy* pMummy = reinterpret_cast<CMummy*>(enemy);
    
    /*enemy->ChangeState(new AttackState(enemy));
    if (enemy->GetEnemyType() == EnemyType::Mummy) {
            CMummy* pMummy = reinterpret_cast<CMummy*>(enemy);
            pMummy->CalcNextAttackType();
            
      }*/
  }


void PatrolState::Exit(CEnemy* enemy)
{
   // cout << " PatrolState::Exit\n";
}

void AttackState::Enter(CEnemy* enemy)
{
    cout << " AttackState::Enter\n";
    m_StateName = ObjectState::Attack;
    auto enemyType = enemy->GetEnemyType();
    switch (enemyType)
    {
    case EnemyType::None:
        break;
    case EnemyType::Skeleton:
        m_AttackType = (int)EnemyAttackType::Melee;
        m_LifeTime = MELLE_ENEMY_ATTACK_TIME;
        break;
    case EnemyType::Mummy:
        m_AttackType = (int)enemy->GetEnemyAttackType();
        if (m_AttackType == (int)EnemyAttackType::Mummy1)
        {
            m_LifeTime = 3.0f;
        }
        else if (m_AttackType == (int)EnemyAttackType::Mummy2)
        {
            m_LifeTime = 5.0f;
        }
        else if (m_AttackType == (int)EnemyAttackType::Mummy3)
        {
            m_LifeTime = 7.0f;
        }
        break;
    case EnemyType::Boss: 
        // 일단 랜덤하게 테스트를 해보는 방향으로
    {
        m_AttackType = (int)enemy->GetEnemyAttackType(); 
        if (m_AttackType == (int)EnemyAttackType::BossSkill_1) { 
            m_LifeTime = BOSS_ATTACK_1_ANIMATION_LENGTH;
            cout << "Atk - 1\n";
        }
        else if (m_AttackType == (int)EnemyAttackType::BossSkill_2) { 
            m_LifeTime = BOSS_ATTACK_2_ANIMATION_LENGTH;
            cout << "Atk - 2\n";
        }
        else if (m_AttackType == (int)EnemyAttackType::BossSkill_3) { 
            m_LifeTime = BOSS_ATTACK_3_ANIMATION_LENGTH;
            cout << "Atk - 3\n";
        }
        else if (m_AttackType == (int)EnemyAttackType::BossSkill_4) {
            m_LifeTime = BOSS_ATTACK_4_ANIMATION_LENGTH;
            cout << "Atk - 4\n";
        }
    } 
        break;
    default:
        break;
    }
}

void AttackState::Execute(CEnemy* enemy, float elapsedTime)
{

    m_ElapsedTime += elapsedTime; 
    if (m_LifeTime < m_ElapsedTime) { 
        enemy->ChangeState(new PatrolState(enemy)); 
    }
    else {
        enemy->ChangeAnimation(ObjectState::Attack);
        enemy->Attack(elapsedTime);
    }
}

void AttackState::Exit(CEnemy* enemy)
{ 
    enemy->SetIsOnMoving(false);
   // cout << "공격 끝 \n";
}

void TraceState::Enter(CEnemy* enemy)
{
    cout << " TraceState::Enter\n";

    enemy->ChangeAnimation(ObjectState::Patrol);
    m_StateName = ObjectState::Trace;
    m_LifeTime = 0.5f;
 //   cout << "TraceState::Enter \n";
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
   // cout << "TraceState::Exit\n";
}

void AttackedState::Enter(CEnemy* enemy)
{
    enemy->ChangeAnimation(ObjectState::Attacked); 
    const int PLAYER_DAMAGE = 15;
    int hp = enemy->GetHP();
    hp -= PLAYER_DAMAGE;
    enemy->SetHP(hp);

    

    if (hp <= 0) {
        cout << " 삭제 해 !!\n";
        
        MAIN_GAME_SCENE_Y->DeleteEnemy(enemy);
        
        return;
    }
    //enemy->SetHP(hp);
}

void AttackedState::Execute(CEnemy* enemy, float elapsedTime)
{
   
}

void AttackedState::Exit(CEnemy* enemy)
{
}

void BornState::Enter(CEnemy* enemy)
{
    m_LifeTime = BOSS_BORN1_ANIMATION_LENGTH;
    enemy->ChangeAnimation(ObjectState::BossBorn);
}

void BornState::Execute(CEnemy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime;
    if (m_ElapsedTime > m_LifeTime) { 
        if (enemy->IsEnemyInSight()) {
            // 공격할 대상이 존재하는 경우 해당 대상에게로
            enemy->ChangeState(new TraceState(enemy));
        }
        else {
            // 공격할 대상이 존재하지 않는다면 랜덤하게 이동하도록
            enemy->ChangeState(new PatrolState(enemy));
        }
    }
}

void BornState::Exit(CEnemy* enemy)
{
}

void Mummy_1_Die_Anger_State::Enter(CEnemy* enemy)
{
    //enemy->ChangeAnimation(ObjectState::Patrol);
    //const int PLAYER_DAMAGE = 15;

    cout << "커져라" << endl;
    enemy->Scale(3.0f, 3.0f, 3.0f);
    //enemy->SetSize(XMFLOAT3(900.f, 900.f, 900.f));
    //enemy->Scale(3.0f, 3.0f, 3.0f);

    //m_StateName = ObjectState::Mummy_1_Die_Anger;
}

void Mummy_1_Die_Anger_State::Execute(CEnemy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime;
    if (m_ElapsedTime > m_LifeTime) {
        enemy->ChangeState(new PatrolState(enemy));
     }
    
}

void Mummy_1_Die_Anger_State::Exit(CEnemy* enemy)
{
    //enemy->ChangeState(new PatrolState(enemy));
}

void Mummy_2_Die_Anger_State::Enter(CEnemy* enemy)
{
}

void Mummy_2_Die_Anger_State::Execute(CEnemy* enemy, float elapsedTime)
{
}

void Mummy_2_Die_Anger_State::Exit(CEnemy* enemy)
{
}
