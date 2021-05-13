#pragma once
#include "GameObject.h"

enum class PuzzleType
{
    None = 0,
    Holding,
    Mirror, 
};

// Puzzle Plate
class CPlate :public CGameObject 
{
private:
    vector<CGameObject*> m_Objects;
    CGameObject* m_CollideObject;

public:
    CPlate(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
    ~CPlate();

public:
    void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
    void RotateAll(const XMFLOAT3& axis, float angle);

    void UpdateColliders() override;  
    bool CollisionCheck(Collider* pCollider) override; 
    bool CollisionCheck(CGameObject* other) override;

public:
    void SetPosition(XMFLOAT3 pos) override;
};

class CPuzzle : public CGameObject
{
private:
    vector<CGameObject*> m_Objects;

public:
    CPuzzle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
        PuzzleType type, CShader* pShader);
    ~CPuzzle(); 

public:
    void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
    void Draw_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override; 
   
    bool CollisionCheck(Collider* pCollider) override; 
    //bool CollisionCheck(CGameObject* other) override;
public:
    void SetPosition(XMFLOAT3 pos) override;
};