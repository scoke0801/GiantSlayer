#pragma once

class CScene;



class CFramework
{
private:
	HWND					m_hWnd;
	HINSTANCE				m_hInst;

	CGameTimer				m_GameTimer;
	CGameTimer				m_FPSTimer;

	CScene					*m_CurrentScene;

	// 타이틀바 출력 관련 변수입니다.
	_TCHAR				m_pszFrameRate[50];	
	TCHAR				m_captionTitle[50];
	int					m_titleLength;

	// 다이렉트X 관련 변수입니다.
	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

	ID3D12DescriptorHeap		*m_pd3dCbvSrvUavDescriptorHeap = nullptr;
	ID3D12DescriptorHeap		*m_pd3dCbvSrvUavDescriptorHeapShadow = nullptr;

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

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void WaitForGpuComplete();
	void MoveToNextFrame();

public:	// about Update
	void Update();
	void Animate();
	void Draw();

public:	// about scene change
	template <typename SceneName>
	void ChangeScene(void* pContext = nullptr)
	{
		//EnterCriticalSection(&m_cs);
		CScene* scene = new SceneName;
		static CScene* prevScene;
		scene->Init(m_rtClient, this);
		scene->SendDataToNextScene(pContext);

		if (m_CurrentScene)
		{
			prevScene = m_CurrentScene;
			//delete m_CurrentScene;
			m_CurrentScene = nullptr;
		}

		m_CurrentScene = scene;
		//LeaveCriticalSection(&m_cs);
	} 
};

