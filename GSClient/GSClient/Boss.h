#pragma once
#include "Enemy.h"

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

// 가장 초기 상태? idle 상태
// 플레이어가 범위 안에 들어온다면? idle -> born1
class CBoss :
    public CEnemy
{
public: 
    CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature);
    ~CBoss();

public:
    void Update(float elapsedTime) override;
    void UpdateOnServer(float fTimeElapsed) override;

public:
    void PlayerEnter(CPlayer* target); 

    void ChangeAnimation(ObjectState stateInfo) override;
private:
    // 시야 범위 안에 플레이어가 없으면 깨어있지 않은 상태
    bool m_isOnAwaken = false; 
};

