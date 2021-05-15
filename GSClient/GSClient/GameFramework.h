#pragma once

#include "Scene.h"

class CCamera;

class CFramework
{
public:// 타이틀바 출력 관련 변수입니다.
	_TCHAR						m_pszFrameRate[50];
	TCHAR						m_captionTitle[50];
	int							m_titleLength;

private:
	HWND						m_hWnd;
	HINSTANCE					m_hInst;

	CGameTimer					m_GameTimer;
	CGameTimer					m_FPSTimer;

	CScene*						m_CurrentScene;

	
	// 다이렉트X 관련 변수입니다.
	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	IDXGIFactory4*				m_pdxgiFactory = NULL;
	IDXGISwapChain3*			m_pdxgiSwapChain = NULL;
	ID3D12Device*				m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = SWAP_BUFFER_COUNT;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource*				m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap*		m_pd3dRtvDescriptorHeap = NULL;

	ID3D12PipelineState*		m_pd3dPipelineState = NULL;

	ID3D12Resource*				m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap*		m_pd3dDsvDescriptorHeap = NULL;

	ID3D12CommandAllocator*		m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue*			m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList*	m_pd3dCommandList = NULL;

	ID3D12Fence*				m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

	ID3D12DescriptorHeap*		m_pd3dCbvSrvUavDescriptorHeap = nullptr;
	ID3D12DescriptorHeap*		m_pd3dCbvSrvUavDescriptorHeapShadow = nullptr;

private:
	string						m_PlayerName;
	int							m_PlayerId = 0;
	 
private:	// 서버와 통신하기 위한 데이터 입니다.
	WSADATA					m_WSA;
	SOCKET					m_Sock;
	SOCKADDR				m_ServerAddr;

	bool					m_IsServerConnected; 

	CRITICAL_SECTION		m_cs;
	bool					m_FrameDirtyFlag = false;
private:
	CFramework();

	CFramework(const CFramework& other) = delete;
	CFramework& operator=(const CFramework& other) = delete;

public: // about GetInstance , init framework
	static CFramework& GetInstance()
	{
		static CFramework self;
		return self;
	}
	void OnCreate(HWND hWnd, HINSTANCE hInst);
	void OnDestroy();

private:
	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void BuildScene();

public:	// about Update
	void SinglePlayUpdate();
	void MultiplayUpdate();
	void SceneUpdate();
	void Animate();
	void Draw();

public: // about Mouse process
	void OnMouseDown(WPARAM btnState, int x, int y) { if (m_CurrentScene) m_CurrentScene->OnMouseDown(btnState, x, y); }
	void OnMouseUp(WPARAM btnState, int x, int y) { if (m_CurrentScene) m_CurrentScene->OnMouseUp(btnState, x, y); }
	void OnMouseMove(WPARAM btnState, int x, int y) { if (m_CurrentScene) m_CurrentScene->OnMouseMove(btnState, x, y); }

public:
	HWND GetHWND() const { return m_hWnd; }

public:	// about server
	bool ConnectToServer();

	void LoginToServer();
	void LogoutToServer();
	void Communicate();

	bool IsOnConntected() const { return m_IsServerConnected; }

	string GetPlayerName() const { return m_PlayerName; }
	void SetPlayerName(const string& id) { m_PlayerName = id; }

	int GetPlayerId() const { return m_PlayerId; }
	void SetPlayerId(int id) { m_PlayerId = id; }

	void SetFrameDirtyFlag(bool flag) { m_FrameDirtyFlag = flag; }
	bool IsFrameDirty() const { return m_FrameDirtyFlag; }

	void SetFocus(bool focusData) { m_CurrentScene->SetFocus(focusData); }

public:	// about scene change
	template <typename SceneName>
	void ChangeScene(void* pContext = nullptr)
	{
		m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

		CScene* scene = new SceneName;
		static CScene* prevScene;
		//scene->BuildCamera(m_pd3dDevice, m_pd3dCommandList,
		//	m_nWndClientWidth, m_nWndClientHeight);
		scene->Init(m_pd3dDevice, m_pd3dCommandList, m_nWndClientWidth, m_nWndClientHeight);
		scene->SendDataToNextScene(pContext);

		if (m_CurrentScene)
		{
			prevScene = m_CurrentScene;
			//delete m_CurrentScene;
			m_CurrentScene = nullptr;
		}

		m_pd3dCommandList->Close();
		ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
		m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

		WaitForGpuComplete();

		m_CurrentScene = scene;
	}

	CScene* GetCurrentScene() const { return m_CurrentScene; }

	SOCKET& GetSocket() { return m_Sock; }

};

DWORD WINAPI ClientMain(LPVOID arg);