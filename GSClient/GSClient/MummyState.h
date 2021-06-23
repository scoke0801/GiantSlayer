#pragma once
#include "State.h"

class CMummy;
class CPlayer;

class WaitState_M : public CState<CMummy>
{
public:
    WaitState_M(CMummy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CMummy* enemy);

    virtual void Execute(CMummy* enemy, float elapsedTime);

    virtual void Exit(CMummy* enemy);
};

class IdleState_M : public CState<CMummy>
{
public:
    IdleState_M(CMummy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CMummy* enemy);

    virtual void Execute(CMummy* enemy, float elapsedTime);

    virtual void Exit(CMummy* enemy);
};

class PatrolState_M : public CState<CMummy>
{
public:
    PatrolState_M(CMummy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CMummy* enemy);

    virtual void Execute(CMummy* enemy, float elapsedTime);

    virtual void Exit(CMummy* enemy);
};

class RunAwayState_M : public CState<CMummy>
{
public:
    RunAwayState_M(CMummy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CMummy* enemy);

    virtual void Execute(CMummy* enemy, float elapsedTime);

    virtual void Exit(CMummy* enemy);
};

class AttackState_M : public CState<CMummy>
{
public:
    AttackState_M(CMummy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CMummy* enemy);

    virtual void Execute(CMummy* enemy, float elapsedTime);

    virtual void Exit(CMummy* enemy);
};

class AttackedState_M : public CState<CMummy>
{
public:
    AttackedState_M(CMummy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CMummy* enemy);

    virtual void Execute(CMummy* enemy, float elapsedTime);

    virtual void Exit(CMummy* enemy);
};
class TraceState_M : public CState<CMummy>
{
private:
    float m_AttackRange;
    CPlayer* m_TargetPlayer;

public:
    TraceState_M(CMummy* enemy) { Enter(enemy); }

public:
    virtual void Enter(CMummy* enemy);

    virtual void Execute(CMummy* enemy, float elapsedTime);

    virtual void Exit(CMummy* enemy);
};
