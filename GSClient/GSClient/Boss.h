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

// ���� �ʱ� ����? idle ����
// �÷��̾ ���� �ȿ� ���´ٸ�? idle -> born1
class CBoss : public CEnemy
{
public: 
    CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature);
    ~CBoss();

public:
    void Update(float elapsedTime) override;
    void UpdateOnServer(float fTimeElapsed) override;
     
    void Attack(float elapsedTime) override;
    void CalcNextAttackType();

public:
    void PlayerEnter(CPlayer* target); 

    void ChangeAnimation(ObjectState stateInfo) override;
    void ChangeAnimationForServer(BOSS_ANIMATION stateInfo);

private:
    static CGameObjectVer2* LoadGeometryAndAnimationFromFileForBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader, bool bHasAnimation);

    static CGameObjectVer2* LoadFrameHierarchyFromFileForBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObjectVer2* pParent, FILE* pInFile, CShader* pShader);

private:
    // �þ� ���� �ȿ� �÷��̾ ������ �������� ���� ����
    bool m_isOnAwaken = false; 
};

