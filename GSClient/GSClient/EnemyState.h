#pragma once
#include "State.h"

class CEnemy;

class Wandering : public CState<CEnemy>
{
private:

    Wandering() {};

    Wandering(const Wandering&);
    Wandering& operator=(const Wandering&);

public:

    static Wandering* Instance();

    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy);

    virtual void Exit(CEnemy* enemy);
};

class Tracking : public CState<CEnemy>
{
private:

    Tracking() {};

    Tracking(const Tracking&);
    Tracking& operator=(const Tracking&);

public:

    static Tracking* Instance();

    virtual void Enter(CEnemy* enemy);

    virtual void Execute(CEnemy* enemy);

    virtual void Exit(CEnemy* enemy);
};

class Running : public CState<CEnemy>
{

};

class Attack : public CState<CEnemy>
{

};