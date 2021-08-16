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
        //m_LifeTime = MELLE_ENEMY_ATTACK_TIME;
        m_LifeTime = enemy->GetAttackAnimLength();
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
        // �ϴ� �����ϰ� �׽�Ʈ�� �غ��� ��������
    {  
        CBoss* pBoss = reinterpret_cast<CBoss*>(enemy);
        pBoss->CalcNextAttackType();
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
        enemy->ChangeState(new TraceState(enemy));
    }
    else if (enemy->GetTargetPlayer()->m_Alive == false) {
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
   // cout << "���� �� \n";
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
    if (m_TargetPlayer == nullptr || m_TargetPlayer->m_Alive == false) {
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
    enemy->SetCanDamaged(false);

    m_LifeTime = 0.8f;
    enemy->ChangeAnimation(ObjectState::Attacked);

    CGameObject* temp = (CGameObject*)m_Data;

    enemy->TakeDamage(temp->GetATK());
    cout << "�ǰ�! ���� ü��: " << enemy->GetHP() << endl;
    /*const int PLAYER_DAMAGE = 15;
    int hp = enemy->GetHP();
    hp -= PLAYER_DAMAGE;
    enemy->SetHP(hp);

    if (hp <= 0) {
        cout << " ���� �� !!\n";
        
        MAIN_GAME_SCENE->DeleteEnemy(enemy);
        
        return;
    }*/
    //enemy->SetHP(hp);
}

void AttackedState::Execute(CEnemy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime;
    if (m_ElapsedTime > m_LifeTime) {
        if (enemy->GetHP() <= 0) {
            enemy->ChangeState(new DeathState(enemy));
        }
        else if (enemy->GetTargetPlayer() != nullptr)
        {
            enemy->SetCanDamaged(true);
            enemy->ChangeState(new TraceState(enemy));
            cout << "���� �ǰ�: Ÿ�� ������ -> TraceState" << endl;
        }
        else
        {
            enemy->SetCanDamaged(true);
            enemy->ChangeState(new PatrolState(enemy));
            cout << "���� �ǰ�: Ÿ�� ���� -> PatrolState" << endl;
        }
        /*else if (enemy->GetTargetPlayer() == nullptr)
            enemy->ChangeState(new PatrolState(enemy));
        else
            enemy->ChangeState(new TraceState(enemy));*/
    }
}

void AttackedState::Exit(CEnemy* enemy)
{
    //m_Data = nullptr;
    //enemy->SetCanDamaged(true);
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
            // ������ ����� �����ϴ� ��� �ش� ��󿡰Է�
            enemy->ChangeState(new TraceState(enemy));
        }
        else {
            // ������ ����� �������� �ʴ´ٸ� �����ϰ� �̵��ϵ���
            enemy->ChangeState(new PatrolState(enemy));
        }
    }
}

void BornState::Exit(CEnemy* enemy)
{
}

void DeathState::Enter(CEnemy* enemy)
{
    cout << "���" << endl; 
    m_LifeTime = ENEMY_DEATH_TIME;

    if (enemy->GetEnemyType() == EnemyType::Boss)
        m_LifeTime = BOSS_DEAD_ANIMATION_LENGTH;

    enemy->ChangeAnimation(ObjectState::Die);
}

void DeathState::Execute(CEnemy* enemy, float elapsedTime)
{
    m_ElapsedTime += elapsedTime;
    if (m_ElapsedTime > m_LifeTime) {
        MAIN_GAME_SCENE->DeleteEnemy(enemy);
    }
}

void DeathState::Exit(CEnemy* enemy)
{

}

void Mummy_1_Die_Anger_State::Enter(CEnemy* enemy)
{
    //enemy->ChangeAnimation(ObjectState::Patrol);
    //const int PLAYER_DAMAGE = 15;

    cout << "Ŀ����" << endl;
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
