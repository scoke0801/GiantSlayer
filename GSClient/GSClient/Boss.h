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

// ���� �ʱ� ����? idle ����
// �÷��̾ ���� �ȿ� ���´ٸ�? idle -> born1
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
    // �þ� ���� �ȿ� �÷��̾ ������ �������� ���� ����
    bool m_isOnAwaken = false; 
};

