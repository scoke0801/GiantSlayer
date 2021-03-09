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

enum class HELP_TEXT_INFO
{
	QuestAccept = 0,
	MonsterKill,
	PuzzleDevice,
	PuzzleMedusa,
	BossKill,
};
class HelpTextUI : public UI
{
	float m_RenderingTime = 3.0f;
public:
	HelpTextUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float width, float height, float depth,
		HELP_TEXT_INFO helpTextInfo);

	~HelpTextUI();

public:
	void Update(double fTimeElapsed) override;
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
};