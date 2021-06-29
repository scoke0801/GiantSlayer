#pragma once
#include "Enemy.h"

enum class BOSS_ANIMATION
{
    Born_1 = 0,
    Idle,
    Skill_1,
    Skill_2,
    Born_2,
    Skile_3,
    Dead,
};

class CBoss :
    public CEnemy
{
};

