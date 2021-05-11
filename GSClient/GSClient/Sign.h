#pragma once
#include "GameObject.h"

enum class SignBoardInfos 
{
    None = 0x02,
    Scroll = 0x04,
    NumPuzzle = 0x08,
    Medusa = 0x10
};
class CSign : public CGameObject
{
private: 
    CGameObject* m_Pillar;          // ±âµÕ
    CGameObject* m_Board;           // Ç¥ÁöÆÇ

public:
    CSign(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        SignBoardInfos boardInfo,
        bool isRotated, bool isFrontSide,
        CShader* pShader);
    ~CSign();
     
public:
    virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
    void Draw_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

    void UpdateColliders() override;

    bool CollisionCheck(Collider* pCollider) override;

public:
    virtual void SetPosition(XMFLOAT3 pos) override;
    void RotateAll(const XMFLOAT3& axis, float angle);
};

