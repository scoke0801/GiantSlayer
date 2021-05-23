#pragma once
#include "State.h"

class CEnemy;
class CPlayer;

constexpr float MELLE_ENEMY_ATTACK_TIME = 2.0f;
constexpr float RANGED_ENEMY_ATTACK_TIME = 2.0f;

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
