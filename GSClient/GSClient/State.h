#pragma once

class CEnemy;

template <class object_type>
class CState
{
public:
    virtual ~CState() {}

    virtual void Enter(object_type*) = 0;
    virtual void Execute(object_type*) = 0;
    virtual void Exit(object_type*) = 0;
};

