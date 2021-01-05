#include "stdafx.h"
#include "InputHandler.h"

CInputHandler::CInputHandler()
{

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
}

void CInputHandler::MouseMove(WPARAM btnState, int x, int y)
{
	//cout << "마우스 이동\n";
}

void CInputHandler::MouseDown(WPARAM btnState, int x, int y)
{
	cout << "마우스 클릭 <Down>\n"; 
	TestingMouseClick = true;
	SetCapture(m_hTargetWND);
}

void CInputHandler::MouseUp(WPARAM btnState, int x, int y)
{
	cout << "마우스 클릭 <Up>\n";
	TestingMouseClick = false;
	ReleaseCapture();
}
