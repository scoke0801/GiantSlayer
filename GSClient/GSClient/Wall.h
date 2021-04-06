#pragma once
#include "GameObject.h"
class CWall :
    public CGameObject
{
public:
    CWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        float width, float height, float depth);
    ~CWall();
};

class CDoor : public CGameObject
{
private:
    bool m_IsLeft;

    float m_fAngle = 0.0f;
    float m_IsOpening = true;

    float m_Height;
public:
    CDoor(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        float width, float height, float depth, bool isLeft = true);
    ~CDoor();

public:
    virtual void Update(double fTimeElapsed);

    float GetHeight() const { return m_Height; }
};
 
class CDoorWall : public CGameObject
{
private:
    CDoor*          m_LeftDoor;
    CDoor*          m_RightDoor;
    vector<CWall*>  m_Walls;

public:
    CDoorWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        float width, float height, float depth,
        CShader* pShader);
    CDoorWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        float width, float height, float depth,
        bool isRotated,
        CShader* pShader);
    ~CDoorWall();

public:
    virtual void Update(float fTimeElapsed) override;
    virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override; 

    void SetPosition(XMFLOAT3 xmf3Position);

    void SetTextureIndexes(int idx);

    void RotateAll(XMFLOAT3 xmf3Axis, float angle);
};