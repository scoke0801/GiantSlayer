#pragma once
#include "State.h"

class Enemy;

class Wandering : public CState<CEnemy>
{
private:

    Wandering() {};

    Wandering(const Wandering&);
    Wandering& operator=(const Wandering&);

public:

    static Wandering* Instance();

    virtual void Enter(CEnemy* CEnemy);

    virtual void Execute(CEnemy* CEnemy);

    virtual void Exit(CEnemy* CEnemy);
};

class Tracking : public CState<CEnemy>
{
private:

    Tracking() {};

    Tracking(const Tracking&);
    Tracking& operator=(const Tracking&);

public:

    static Tracking* Instance();

    virtual void Enter(CEnemy* CEnemy);

    virtual void Execute(CEnemy* CEnemy);

    virtual void Exit(CEnemy* CEnemy);
};

class Running : public CState<CEnemy>
{

};

class Attack : public CState<CEnemy>
{

};