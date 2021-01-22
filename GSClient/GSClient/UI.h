#pragma once
#include "GameObject.h"
class UI :  public CGameObject
{
private:
	CBillboardMesh* m_pBillboardMesh = nullptr; 

public:
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~UI();
};

