#pragma once

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44
#define VK_C 0x43
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35

struct GameInputs
{
	bool KEY_1 = false;
	bool KEY_2 = false;
	bool KEY_3 = false;
	bool KEY_4 = false;
	bool KEY_5 = false;

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
public:
	void SetTargetWND(HWND hTarget) { m_hTargetWND = hTarget; }
};

#define GAME_INPUT CInputHandler::GetInstance().m_Inputs