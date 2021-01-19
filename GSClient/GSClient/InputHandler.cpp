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
	
	if (GetAsyncKeyState(VK_F1) & 0x8000)		m_Inputs.KEY_F1 = true;
	if (GetAsyncKeyState(VK_F2) & 0x8000)		m_Inputs.KEY_F2 = true;
	if (GetAsyncKeyState(VK_F3) & 0x8000)		m_Inputs.KEY_F3 = true;
	if (GetAsyncKeyState(VK_F4) & 0x8000)		m_Inputs.KEY_F4 = true;
}

void CInputHandler::MouseMove(WPARAM btnState, int x, int y)
{
	//cout << "마우스 이동\n";
	if (IS_MOUSE_DOWN)
	{
		m_PrevMousePos = m_CurMousePos;
		m_CurMousePos.x = x;
		m_CurMousePos.y = y; 
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_PrevMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_PrevMousePos.y));

		// Update angles based on input to orbit camera around box.
		//mTheta += dx;
		//mPhi += dy;
		//
		//// Restrict the angle mPhi.
		//mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
}

void CInputHandler::MouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{ 
	}

	cout << "마우스 클릭 <Down> - x : " << x << " y : " << y << "\n";
	m_PrevMousePos = m_CurMousePos;
	m_CurMousePos.x = x;
	m_CurMousePos.y = y;

	m_IsMouseDown = true;
	SetCapture(m_hTargetWND);
}

void CInputHandler::MouseUp(WPARAM btnState, int x, int y)
{
	cout << "마우스 클릭 <Up>\n";
	m_IsMouseDown = false;
	ReleaseCapture();
}
