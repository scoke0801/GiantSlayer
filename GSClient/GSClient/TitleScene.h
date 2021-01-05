#pragma once
#include "Scene.h"

class CTitleScene : public CScene
{
private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature;
	ID3D12PipelineState* m_pd3dPipelineState;

public:
	CTitleScene();
	~CTitleScene();

	virtual void Update(double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;

public:
	virtual void SendDataToNextScene(void* context) override {}
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	void CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

