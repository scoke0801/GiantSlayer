#pragma once

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44
#define VK_B 0x42
#define VK_C 0x43
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36 
#define VK_7 0x37 
#define VK_8 0x38 
#define VK_9 0x39


#define VK_U 0x55
#define VK_I 0x49
#define VK_O 0x4F
#define VK_P 0x50
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C 
#define VK_R 0x52


struct GameInputs
{
	bool KEY_1 = false;
	bool KEY_2 = false;
	bool KEY_3 = false;
	bool KEY_4 = false;
	bool KEY_5 = false;
	bool KEY_6 = false;
	bool KEY_7 = false;
	bool KEY_8 = false;
	bool KEY_9 = false;

	bool KEY_W = false;
	bool KEY_A = false;
	bool KEY_S = false;
	bool KEY_D = false;
	 
	bool KEY_SPACE = false;

	// for Debuging Keys
	bool KEY_F1 = false;
	bool KEY_F2 = false;	
	bool KEY_F3 = false;
	bool KEY_F4 = false;
	bool KEY_F5 = false;
	bool KEY_F9 = false;

	bool KEY_ADD = false;
	bool KEY_SUBTRACT = false;

	bool KEY_B = false;
	bool KEY_C = false; 
	bool KEY_R = false;

	bool KEY_U = false;
	bool KEY_I = false;
	bool KEY_O = false;
	bool KEY_P = false;
	bool KEY_J = false;
	bool KEY_K = false;
	bool KEY_L = false;
	bool KEY_T = false;
	bool KEY_Y = false;
};
enum class CommandType
{
	None = 0,
	Move,
	MoveAttack,
	Attack
};
class CInputHandler
{
private :
	HWND m_hTargetWND;

public:
	GameInputs m_Inputs;
	bool TestingMouseClick = false;
	POINT m_PrevMousePos;

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

	POINT GetMousePoint() const { return m_PrevMousePos; }
	void ResetMousePos() { m_PrevMousePos = POINT(); }

	int GetCommandType() const;
};
#define GAME_INPUT CInputHandler::GetInstance().m_Inputs
#define GET_MOUSE_POS CInputHandler::GetInstance().m_PrevMousePos