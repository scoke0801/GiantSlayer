#pragma once
#include "Scene.h"

struct TestData
{
	bool MouseClikced = false;
};

struct TestVertex
{
	TestVertex(
		const DirectX::XMFLOAT3& p,
		const DirectX::XMFLOAT3& n,
		const DirectX::XMFLOAT3& t,
		const DirectX::XMFLOAT2& uv) :
		m_xmf3Position(p),
		m_xmf3Normal(n),
		m_xmf3TangentU(t),
		m_xmf2TexC(uv) {}
	TestVertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v) :
		m_xmf3Position(px, py, pz),
		m_xmf3Normal(nx, ny, nz),
		m_xmf3TangentU(tx, ty, tz),
		m_xmf2TexC(u, v) {}
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT3 m_xmf3TangentU;
	XMFLOAT2 m_xmf2TexC;
};

class CTitleScene : public CScene
{
private:
	ID3D12RootSignature*	m_pd3dGraphicsRootSignature;
	ID3D12PipelineState*	m_pd3dPipelineState;

	// 테스트 임 시 상 수 버 퍼
	ID3D12Resource*			m_pd3dTestData = NULL;
	TestData*				m_pcbMappedTestData = NULL;

	ID3D12DescriptorHeap*	m_pd3dSrvDescriptorHeap = nullptr;

private:	// 서버와 통신하기 위한 데이터 입니다.
	WSADATA m_WSA;
	SOCKET m_Sock;
	SOCKADDR m_ServerAddr;

	bool m_IsServerConnected;

public:
	CTitleScene();
	~CTitleScene();

	virtual void Update(double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;
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

private: // 서버 관련
	void ConnectToServer();
	bool PrepareCommunicate();
};

