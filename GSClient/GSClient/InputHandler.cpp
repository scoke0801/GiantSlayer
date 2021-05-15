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
	if (GetAsyncKeyState(VK_6) & 0x8000)		m_Inputs.KEY_6 = true;
	if (GetAsyncKeyState(VK_7) & 0x8000)		m_Inputs.KEY_7 = true;
	if (GetAsyncKeyState(VK_8) & 0x8000)		m_Inputs.KEY_8 = true;
	if (GetAsyncKeyState(VK_9) & 0x8000)		m_Inputs.KEY_9 = true;

	if (GetAsyncKeyState(VK_W) & 0x8000)		m_Inputs.KEY_W = true;
	if (GetAsyncKeyState(VK_A) & 0x8000)		m_Inputs.KEY_A = true;
	if (GetAsyncKeyState(VK_S) & 0x8000)		m_Inputs.KEY_S = true;
	if (GetAsyncKeyState(VK_D) & 0x8000)		m_Inputs.KEY_D = true; 
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		m_Inputs.KEY_SPACE = true;
	 
#pragma region for Debuging Keys	
	if (GetAsyncKeyState(VK_F1) & 0x8000)		m_Inputs.KEY_F1 = true;
	if (GetAsyncKeyState(VK_F2) & 0x8000)		m_Inputs.KEY_F2 = true;
	if (GetAsyncKeyState(VK_F3) & 0x8000)		m_Inputs.KEY_F3 = true;
	if (GetAsyncKeyState(VK_F4) & 0x8000)		m_Inputs.KEY_F4 = true;
	if (GetAsyncKeyState(VK_F5) & 0x8000)		m_Inputs.KEY_F5 = true;
	if (GetAsyncKeyState(VK_F9) & 0x8000)		m_Inputs.KEY_F9 = true;

	if (GetAsyncKeyState(VK_ADD) & 0x8000)		m_Inputs.KEY_ADD = true;
	if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000) m_Inputs.KEY_SUBTRACT = true;

	if (GetAsyncKeyState(VK_B) & 0x8000)		m_Inputs.KEY_B = true;
	if (GetAsyncKeyState(VK_C) & 0x8000)		m_Inputs.KEY_C = true;

	if (GetAsyncKeyState(VK_U) & 0x8000)		m_Inputs.KEY_U = true;
	if (GetAsyncKeyState(VK_I) & 0x8000)		m_Inputs.KEY_I = true;
	if (GetAsyncKeyState(VK_O) & 0x8000)		m_Inputs.KEY_O = true;
	if (GetAsyncKeyState(VK_P) & 0x8000)		m_Inputs.KEY_P = true;
	if (GetAsyncKeyState(VK_J) & 0x8000)		m_Inputs.KEY_J = true;
	if (GetAsyncKeyState(VK_K) & 0x8000)		m_Inputs.KEY_K = true;
	if (GetAsyncKeyState(VK_L) & 0x8000)		m_Inputs.KEY_L = true;
#pragma endregion
}

void CInputHandler::MouseMove(WPARAM btnState, int x, int y)
{
	//cout << "마우스 이동\n";
}

void CInputHandler::MouseDown(WPARAM btnState, int x, int y)
{
	cout << "마우스 클릭 <Down> - x : " << x << " y : " << y << "\n";
	m_PrevMousePos.x = x; 
	m_PrevMousePos.y = y;
		
	TestingMouseClick = true;
	SetCapture(m_hTargetWND);
}

void CInputHandler::MouseUp(WPARAM btnState, int x, int y)
{
	cout << "마우스 클릭 <Up>\n";
	TestingMouseClick = false;
	ReleaseCapture();
}

int CInputHandler::GetCommandType() const
{
	bool moveFlag = false;
	bool attackFlag = false;
	if (m_Inputs.KEY_W || m_Inputs.KEY_A || m_Inputs.KEY_S || m_Inputs.KEY_D)
		moveFlag = true;

	if (moveFlag && attackFlag) return (int)CommandType::MoveAttack;
	if (attackFlag) return (int)CommandType::Attack;
	if (moveFlag) return (int)CommandType::Move;
	return (int)CommandType::None;
}
