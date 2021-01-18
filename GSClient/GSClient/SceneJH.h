#pragma once
#include "Scene.h"

struct BasicVertex
{
	XMFLOAT3 xmf3Position;
	XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f);
};

class CSceneJH : public CScene
{
private:
	ID3D12RootSignature*			m_pd3dGraphicsRootSignature;
	ID3D12PipelineState*			m_pd3dPipelineState;

	// 테스트 임 시 상 수 버 퍼
	ID3D12Resource*					m_pd3dTestData = NULL;
	// TestData*					m_pcbMappedTestData = NULL;

	ID3D12DescriptorHeap*			m_pd3dSrvDescriptorHeap = nullptr;


	ID3D12PipelineState*			m_pd3dMinimapPSO;

	ID3D12Resource*					m_pd3dVertexBuffer;
	ID3D12Resource*					m_pd3dVertexUploadBuffer;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;

public:
	CSceneJH();
	~CSceneJH();

	virtual void Update(double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	virtual void ProcessInput();

public:
	virtual void SendDataToNextScene(void* context) override {}
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

private: // 객체 생성 관련
	void CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildConstantsBuffers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildOBJAboutMinimap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

