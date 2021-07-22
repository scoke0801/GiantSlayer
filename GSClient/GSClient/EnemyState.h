#pragma once
#include "State.h"

class CEnemy;
class CPlayer;
 
class WaitState : public CState<CEnemy>
{
public: 
    WaitState(CEnemy* enemy) { Enter(enemy); }

public: 
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};

class IdleState : public CState<CEnemy>
{
public:
    IdleState(CEnemy* enemy) { Enter(enemy); }

public:  
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};

class PatrolState : public CState<CEnemy>
{
public:
    PatrolState(CEnemy* enemy) { Enter(enemy); }

public: 
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};

class RunAwayState : public CState<CEnemy>
{
public:
    RunAwayState(CEnemy* enemy) { Enter(enemy); }

public: 
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};

class AttackState : public CState<CEnemy>
{ 
private:
    int     m_AttackType;
    
public:
    float					m_AttackDelayTime = 0.0f;
    float					m_LaserAttackDelayTime[3] = { 3.0f,5.0f,7.0f };
    bool					m_LaserAttack[3] = { false,false,false };

public:
    AttackState(CEnemy* enemy) { Enter(enemy); }

public:  
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};


class AttackedState : public CState<CEnemy>
{
public:
    AttackedState(CEnemy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};


// ¸Ó¹Ì Á×¾úÀ»¶§ ÆøÁÖ»óÅÂ 
class Mummy_1_Die_Anger_State : public CState<CEnemy>
{
public:
    Mummy_1_Die_Anger_State(CEnemy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};

class Mummy_2_Die_Anger_State : public CState<CEnemy>
{
public:
    Mummy_2_Die_Anger_State(CEnemy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};

class TraceState : public CState<CEnemy>
{
private:
    float m_AttackRange;
    CPlayer* m_TargetPlayer;

public:
    TraceState(CEnemy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};
 
class BornState : public CState<CEnemy>
{ 
public:
    BornState(CEnemy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy, float elapsedTime);

    virtual void Exit(CEnemy* enemy);
};
