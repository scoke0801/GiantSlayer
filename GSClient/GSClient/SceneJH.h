#pragma once
#include "Scene.h"

struct BasicVertex
{
	XMFLOAT3 xmf3Position;
	XMFLOAT2 m_xmf2TexC;
};
#pragma region minimap_Test
class CSceneJH : public CScene
{
private:
	ID3D12RootSignature*			m_pd3dGraphicsRootSignature;

	// 테스트 임 시 상 수 버 퍼
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
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void DrawUI(ID3D12GraphicsCommandList* pd3dCommandList) override;

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
#pragma endregion

#pragma region UI_TEST
class CShader;
class CGameObject;
class CCamera;

class CSceneJH2 : public CScene
{
protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CGameObject**			m_ppObjects = NULL;
	int						m_nObjects = 0;

	CGameObject**			m_ppUIs = NULL;
	int						m_nUIs = 0;

	ID3D12RootSignature*	m_pd3dGraphicsRootSignature = NULL;

	CCamera**				m_Cameras;
	CCamera*				m_CurrentCamera = nullptr;

	bool					m_DrawUI = true;
	int						m_nUIStartIndex;

private:
	POINT					m_LastMousePos;

	ID3D12DescriptorHeap*	m_pd3dSrvDescriptorHeap = nullptr;

public:
	CSceneJH2();
	~CSceneJH2();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) override;

	void ReleaseObjects();

public:
	virtual void Update(double elapsedTime) override;
	void AnimateObjects(float fTimeElapsed);

	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void DrawUI(ID3D12GraphicsCommandList* pd3dCommandList) override;

public:
	// about Keyboard process
	virtual void ProcessInput();

	// about Mouse process
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

public:
	virtual void ReleaseUploadBuffers() override;

	//그래픽 루트 시그너쳐를 생성한다.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override { return(m_pd3dGraphicsRootSignature); }
};
#pragma endregion

#pragma region SceneJH3 - About Terrain and Lights
class CSceneJH3 : public CScene
{
protected:
	CGameObject**			m_ppObjects = NULL;
	int						m_nObjects = 0;

	ID3D12RootSignature*	m_pd3dGraphicsRootSignature = NULL;

	CCamera**				m_Cameras;
	CCamera*				m_CurrentCamera = nullptr;

private:
	POINT					m_LastMousePos;

	ID3D12DescriptorHeap*	m_pd3dSrvDescriptorHeap = nullptr; 

public:
	CSceneJH3();
	~CSceneJH3();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height);
	
	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	
	void ReleaseObjects();

public:
	virtual void Update(double elapsedTime) override;
	void AnimateObjects(float fTimeElapsed);

	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;

public:
	virtual void ProcessInput();

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

public:
	virtual void ReleaseUploadBuffers() override;

	//그래픽 루트 시그너쳐를 생성한다.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override { return(m_pd3dGraphicsRootSignature); }
};
#pragma endregion