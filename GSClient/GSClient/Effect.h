#pragma once

class CEffect : public CGameObject
{
public:
	CEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float x, float z);
	~CEffect();

public:
	void Update(float elapsedTime) override;
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
};

