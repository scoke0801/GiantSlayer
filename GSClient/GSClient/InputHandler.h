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
};

class CInputHandler
{
private :
	HWND m_hTargetWND;

public:
	GameInputs m_Inputs;
	bool TestingMouseClick = false;

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
};
#define GAME_INPUT CInputHandler::GetInstance().m_Inputs