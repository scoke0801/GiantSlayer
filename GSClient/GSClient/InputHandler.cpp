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
	if (GetAsyncKeyState(VK_3) & 0x8000)		m_Inputs.KEY_3 = true;
	if (GetAsyncKeyState(VK_4) & 0x8000)		m_Inputs.KEY_4 = true;
	if (GetAsyncKeyState(VK_5) & 0x8000)		m_Inputs.KEY_5 = true;

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

	/////////////////////////////////////////
	//
	if (GetAsyncKeyState(VK_B) & 0x8000)		m_Inputs.KEY_B = true;
}
