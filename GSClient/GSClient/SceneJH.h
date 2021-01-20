#pragma once
#include "Scene.h"

struct BasicVertex
{
	XMFLOAT3 xmf3Position;
	XMFLOAT2 m_xmf2TexC;
};

class CSceneJH : public CScene
{
private:
	ID3D12RootSignature*			m_pd3dGraphicsRootSignature;

	// �׽�Ʈ �� �� �� �� �� ��
	ID3D12Resource*					m_pd3dTestData = NULL;
	// TestData*					m_pcbMappedTestData = NULL;

	ID3D12DescriptorHeap*			m_pd3dSrvDescriptorHeap = nullptr;

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

private: // ��ü ���� ����
	void CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildConstantsBuffers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildOBJAboutMinimap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};



class CShader;
class CGameObject;
class CCamera;

class CSceneJH2 : public CScene
{
public:
	CSceneJH2();
	~CSceneJH2();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void BuildCamera(int width, int height) override;

	void ReleaseObjects();

public:	
	virtual void ProcessInput();

	virtual void Update(double elapsedTime) override;
	void AnimateObjects(float fTimeElapsed);
	
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

public:
	virtual void ReleaseUploadBuffers() override;

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override { return(m_pd3dGraphicsRootSignature); }
	
protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CGameObject**			m_ppObjects = NULL;
	int						m_nObjects = 0;

	ID3D12RootSignature*	m_pd3dGraphicsRootSignature = NULL;

	CCamera*				m_Camera;
	CTestCamera*			m_TestCamera;

};
