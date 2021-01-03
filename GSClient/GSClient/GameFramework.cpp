#include "stdafx.h"
#include "GameFramework.h"
#include "Scene.h"
#include "InputHandler.h"

CFramework::CFramework()
{
	m_GameTimer.Init();
	m_FPSTimer.Init();
}

void CFramework::Init(HWND hWnd, HINSTANCE hInst)
{ 
	m_hWnd = hWnd; 
	m_hInst = hInst; 	

	m_CurrentScene = new CNullScene;

	_tcscpy_s(m_pszFrameRate, _T("Giant Slayer"));
	LoadString(m_hInst, IDS_APP_TITLE, m_captionTitle, TITLE_LENGTH);

#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_captionTitle, TEXT(" ("));
#endif
	m_titleLength = lstrlen(m_captionTitle);
	SetWindowText(m_hWnd, m_captionTitle);
}

void CFramework::Update()
{
	m_GameTimer.Tick();

	CInputHandler::GetInstance().ProcessInput();

	
	double lag = 0.0f;
	double fps = 0.0f;

	if (m_GameTimer.GetElapsedTime() > FPS)				//������ �ð��� �귶�ٸ�
	{
		//m_currentTime = std::chrono::system_clock::now();//����ð� ����

		if (m_GameTimer.GetElapsedTime() > 0.0) fps = 1.0 / m_GameTimer.GetElapsedTime();

		//���� �ð��� �ʾ��� ��� �̸� �������� �� ���� ������Ʈ ��ŵ�ϴ�.
		lag += m_GameTimer.GetElapsedTime();
		for (int i = 0; lag > FPS && i < MAX_LOOP_TIME; ++i)
		{
			//Communicate();
			//update(FPS);	
			m_CurrentScene->Update(FPS);
			lag -= FPS;
		}
	}
	// �ִ� FPS �̸��� �ð��� ����ϸ� ���� ����(Frame Per Second)
	else
		return;

#if defined(SHOW_CAPTIONFPS)

	std::chrono::system_clock::time_point lastUpdateTime = m_FPSTimer.CurrentTime();
	
	double updateElapsed = m_FPSTimer.GetElapsedTime(lastUpdateTime);

	if (updateElapsed > MAX_UPDATE_FPS)
		m_FPSTimer.UpdateCurrentTime();
	else
		return;
	fps = 1.0 / m_GameTimer.GetElapsedTime();

	_itow_s(fps + 0.1f,
		 m_captionTitle + m_titleLength,
		TITLE_LENGTH - m_titleLength, 10);
	wcscat_s(m_captionTitle +  m_titleLength, TITLE_LENGTH - m_titleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd,  m_captionTitle);
#endif
}

void CFramework::Animate()
{
}

void CFramework::Draw()
{
	m_CurrentScene->Draw();
}
