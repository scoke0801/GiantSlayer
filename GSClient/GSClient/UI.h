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
		float width, float height, float depth, 
		bool IsHalfSize = false); 
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float radius);
	virtual ~UI();

public:
	void Rotate(float angle);
};

class Minimap : public UI
{
public:
	Minimap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float radius);
	~Minimap();
};

class MinimapArrow : public UI
{
public:
	MinimapArrow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float width, float height, float depth);
	~MinimapArrow();
};
class HpSpPercentUI : public UI
{
public:
	HpSpPercentUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float width, float height, float depth,
		bool isHp = true);
	~HpSpPercentUI();
};
