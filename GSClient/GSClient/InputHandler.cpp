#include "stdafx.h"
#include "InputHandler.h"

CInputHandler::CInputHandler()
{
	m_IsMouseRBtnDown = m_IsMouseLBtnDown = false; 
	m_CurMousePos = { -1, -1 };
}

void CInputHandler::ProcessInput()
{
	//static UCHAR pKeysBuffer[256];
	//GetKeyboardState(pKeysBuffer);

	m_Inputs = GameInputs();

	if (GetAsyncKeyState(VK_1) & 0x8000)		m_Inputs.KEY_1 = true;
	if (GetAsyncKeyState(VK_2) & 0x8000)		m_Inputs.KEY_2 = true;
	if (GetAsyncKeyState(VK_W) & 0x8000)		m_Inputs.KEY_W = true;
	if (GetAsyncKeyState(VK_A) & 0x8000)		m_Inputs.KEY_A = true;
	if (GetAsyncKeyState(VK_S) & 0x8000)		m_Inputs.KEY_S = true;
	if (GetAsyncKeyState(VK_D) & 0x8000)		m_Inputs.KEY_D = true;
	if (GetAsyncKeyState(VK_C) & 0x8000)		m_Inputs.KEY_C = true;
	if (GetAsyncKeyState(VK_SELECT) & 0x8000)	m_Inputs.KEY_SPACE = true;
	
	if (GetAsyncKeyState(VK_F1) & 0x8000)		m_Inputs.KEY_F1 = true;
	if (GetAsyncKeyState(VK_F2) & 0x8000)		m_Inputs.KEY_F2 = true;
	if (GetAsyncKeyState(VK_F3) & 0x8000)		m_Inputs.KEY_F3 = true;
	if (GetAsyncKeyState(VK_F4) & 0x8000)		m_Inputs.KEY_F4 = true;
}

void CInputHandler::MouseMove(WPARAM btnState, int x, int y)
{
	if (m_IsMouseLBtnDown || m_IsMouseRBtnDown)
	{
		cout << "마우스 이동\n"; 

		m_CurMousePos.x = x;
		m_CurMousePos.y = y;  
	}
}

void CInputHandler::MouseDown(WPARAM btnState, int x, int y)
{	
	m_PrevMousePos.x = x;
	m_PrevMousePos.y = y;
	
	SetCapture(m_hTargetWND);

	if ((btnState & MK_LBUTTON) != 0)
	{
		cout << "마우스 왼쪽 클릭 <Down> - x : " << x << " y : " << y << "\n";
	
		m_IsMouseLBtnDown = true;
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		cout << "마우스 오른쪽 클릭 <Down> - x : " << x << " y : " << y << "\n";

		m_IsMouseRBtnDown = true; 
	}
}

void CInputHandler::MouseUp(WPARAM btnState, int x, int y)
{
	cout << "마우스 클릭 <Up>\n";	
	m_IsMouseRBtnDown = false;
	m_IsMouseLBtnDown = false;
	if ((btnState & MK_LBUTTON) != 0)
	{	
		cout << "마우스 왼쪽 클릭 <Up>\n";
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		cout << "마우스 오른쪽 클릭 <Up>\n";
	} 
	ReleaseCapture();
}
