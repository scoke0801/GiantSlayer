#pragma once
#include "Enemy.h"

constexpr float BOSS_BORN1_ANIMATION_LENGTH = 7.333333;
constexpr float BOSS_DEAD_ANIMATION_LENGTH = 7;
constexpr float BOSS_IDLE_ANIMATION_LENGTH = 3;
constexpr float BOSS_RUN_ANIMATION_LENGTH = 0.5666667;
constexpr float BOSS_ATTACK_1_ANIMATION_LENGTH = 3;
constexpr float BOSS_ATTACK_2_ANIMATION_LENGTH = 3;
constexpr float BOSS_ATTACK_3_ANIMATION_LENGTH = 3.333333;
constexpr float BOSS_ATTACK_4_ANIMATION_LENGTH = 7;

enum class BOSS_ANIMATION
{
    Born_1 = 0,
    Born_2,
    Dead,
    Idle,
    Run,
    Skill_1,
    Skill_2,
    Skill_3,
    Skill_4
};

class CPlayer;


class CBoss : public CEnemy
{
public:
    CBoss();
    ~CBoss();

public:
    void Update(float elapsedTime) override;

    void Attack(float elapsedTime) override;
    void CalcNextAttackType();

public:
    void PlayerEnter(CPlayer* target);

    void ChangeAnimation(ObjectState stateInfo) override;
private:
    static CAnimationObject* LoadGeometryAndAnimationFromFileForBoss(const char* pstrFileName, bool bHasAnimation);

    static CAnimationObject* LoadFrameHierarchyFromFileForBoss(CAnimationObject* pParent, FILE* pInFile);

private:
    // 시야 범위 안에 플레이어가 없으면 깨어있지 않은 상태
    bool m_isOnAwaken = false;
};

