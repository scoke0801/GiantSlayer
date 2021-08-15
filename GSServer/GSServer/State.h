#pragma once
#include "GameObject.h"

template <class object_type>
class CState
{
protected:
    float m_ElapsedTime = 0.0f; // ������Ʈ ���� �� �ð�
    float m_LifeTime = 0.0f;    // ������Ʈ ���� �ð�

    ObjectState m_StateName;

    void* m_Data;
public:
    virtual ~CState() {}

    virtual void Enter(object_type*) = 0;
    virtual void Execute(object_type*, float elapsedTime) = 0;
    virtual void Exit(object_type*) = 0;

public:
    float GetLifeTime() const { return m_LifeTime; }
    ObjectState GetStatename() const { return m_StateName; }
};

