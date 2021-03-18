#pragma once
#include "GameObject.h"

class CSign : public CGameObject
{
private: 
    CGameObject* m_Pillar;          // ���
    CGameObject* m_Board;           // ǥ����

public:
    CSign(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        bool isFrontSide,
        CShader* pShader);
    ~CSign();
     
public:
    virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

public:
    virtual void SetPosition(XMFLOAT3 pos) override;
    void RotateAll(const XMFLOAT3& axis, float angle);
};

