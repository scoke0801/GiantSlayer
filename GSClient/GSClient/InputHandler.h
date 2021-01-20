#pragma once

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44
#define VK_C 0x43
#define VK_1 0x31
#define VK_2 0x32
 
struct GameInputs
{
	bool KEY_1 = false;
	bool KEY_2 = false;

	bool KEY_W = false;
	bool KEY_A = false;
	bool KEY_S = false;
	bool KEY_D = false;

	bool KEY_C = false;
	 
	bool KEY_SPACE = false;

	bool KEY_F1 = false;
	bool KEY_F2 = false;	
	bool KEY_F3 = false;
	bool KEY_F4 = false;
};

class CInputHandler
{
private :
	HWND			m_hTargetWND;

public:
	GameInputs		m_Inputs;

	bool			m_IsMouseRBtnDown = false;
	bool			m_IsMouseLBtnDown = false;

	POINT			m_PrevMousePos;
	POINT			m_CurMousePos;

private:
	CInputHandler();
	CInputHandler(const CInputHandler& other) = delete;
	CInputHandler& operator=(const CInputHandler& other) = delete;

public:
	static CInputHandler& GetInstance()
	{
		static CInputHandler self;
		return self;
	}
	void ProcessInput();

	void MouseMove(WPARAM btnState, int x, int y);
	void MouseDown(WPARAM btnState, int x, int y);
	void MouseUp(WPARAM btnState, int x, int y);

public:
	void SetTargetWND(HWND hTarget) { m_hTargetWND = hTarget; }

	POINT GetCurrentMousePoint() const { return m_CurMousePos; }
	POINT GetPrevMousePoint() const { return m_PrevMousePos; }

	void ResetMousePos() { m_PrevMousePos = POINT(); }

public:
	bool IsMouseRBtnDonw() const { return m_IsMouseRBtnDown; }
	bool IsMouseLBtnDonw() const { return m_IsMouseLBtnDown; }
};
#define GAME_INPUT CInputHandler::GetInstance().m_Inputs
#define GET_CUR_MOUSE_POS CInputHandler::GetInstance().GetCurrentMousePoint()
#define GET_PREV_MOUSE_POS CInputHandler::GetInstance().GetPrevMousePoint()
#define IS_MOUSE_RBTN_DOWN CInputHandler::GetInstance().IsMouseRBtnDonw()
#define IS_MOUSE_LBTN_DOWN CInputHandler::GetInstance().IsMouseLBtnDonw()	