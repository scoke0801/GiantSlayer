#pragma once
#include "Scene.h"

struct TestData
{
	bool MouseClikced = false;
};

struct TestVertex
{
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

class CTitleScene : public CScene
{
private:
	ID3D12RootSignature*	m_pd3dGraphicsRootSignature;
	ID3D12PipelineState*	m_pd3dPipelineState;

	// 테스트 임 시 상 수 버 퍼
	ID3D12Resource*			m_pd3dTestData = NULL;
	TestData*				m_pcbMappedTestData = NULL;

public:
	CTitleScene();
	~CTitleScene();

	virtual void Update(ID3D12GraphicsCommandList* pd3dCommandList, double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;

public:
	virtual void SendDataToNextScene(void* context) override {}
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

private:
	void CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildConstantsBuffers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

