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
	void Init(HWND hWnd, HINSTANCE hInst);

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

		if (m_pCurScene)
		{
			prevScene = m_pCurScene;
			//delete m_pCurScene;
			m_pCurScene = nullptr;
		}

		m_pCurScene = scene;
		//LeaveCriticalSection(&m_cs);
	} 
};

