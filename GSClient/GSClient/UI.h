#pragma once
#include "GameObject.h"
class UI :  public CGameObject
{
private:
	CBillboardMesh* m_pBillboardMesh = nullptr; 

public:
	UI() {}
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList); 
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float width, float height, float depth);
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float radius);
	virtual ~UI();
};

class Minimap : public UI
{
public:
	Minimap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float radius);
	~Minimap();
};

