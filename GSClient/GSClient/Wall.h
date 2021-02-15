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

