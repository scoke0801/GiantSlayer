#pragma once

constexpr int MAX_USER = 10;

constexpr int MAX_PLAYER_NAME = 200;
constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 4096;
constexpr int F_TO_I = 10000;
constexpr int I_TO_F = F_TO_I;

inline int FloatToInt(float num)
{ 
	return (int)(num * F_TO_I);
}
inline float IntToFloat(int num) 
{ 
	return (float)num / I_TO_F; 
}

enum class GAME_STATE : short {
	S_STANDBY = 0,	// 로딩하게 된다면..
	S_LOGIN,  
	S_INGAME,
	S_RESULT
};

enum class ROTATION_AXIS : short {
	ROTATE_AXIS_X = 1,
	ROTATE_AXIS_Y,
	ROTATE_AXIS_Z
};
enum class WEAPON_TYPE : short {
	SWORD = 1,
	ARROW
};
enum class PACKET_PROTOCOL : short
{
	//CLIENT 
	C2S_LOGIN = 1,

	C2S_INGAME_KEYBOARD_INPUT, 
	C2S_INGAME_MOUSE_INPUT,

	C2S_INGAME_LOGOUT,

	//SERVER 
	S2C_LOGIN_HANDLE, 

	S2C_NEW_PLAYER,
	S2C_DELETE_PLAYER,

	S2C_INGAME_KEYBOARD_INPUT, 
	S2C_INGAME_MOUSE_INPUT,
	 
	S2C_INGAME_MONSTER_ACT, 
	 
	S2C_INGAME_UPDATE_PLAYERS_STATE,
	  
	S2C_INGAME_DOOR_EVENT,
	 
	S2C_INGAME_PUZZLE_EVENT,

	S2C_INGAME_END,
};

#pragma pack (push, 1)
struct test_packet {
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
	XMFLOAT3 xmf3Dir; 
}; 

struct P_C2S_LOGIN {
	BYTE size;
	PACKET_PROTOCOL type;
	char name[MAX_PLAYER_NAME];
};
struct P_C2S_KEYBOARD_INPUT_PACKET {
	BYTE size;
	PACKET_PROTOCOL type;
	short keyInput;
};

struct P_C2S_MOUSE_INPUT {
	BYTE size; 
	PACKET_PROTOCOL type;
	short xInput;
	short yInput;
}; 

struct P_S2C_PROCESS_LOGIN {
	BYTE size; 
	PACKET_PROTOCOL type;
	bool isSuccess;
	short x, y, z;
};

struct P_S2C_ADD_PLAYER {
	BYTE size;
	PACKET_PROTOCOL type;

	char id;
	
	short x, y, z;
	short angle;
};

struct P_S2C_DELETE_PLAYER {
	BYTE size;
	PACKET_PROTOCOL type;

	char id; 
};

struct P_S2C_PROCESS_KEYBOARD {
	BYTE size;
	PACKET_PROTOCOL type;

	short posX, posY, posZ;
	short rotateAngle;

	WEAPON_TYPE weaponType;
};

struct P_S2C_PROCESS_MOUSE {
	BYTE size;
	PACKET_PROTOCOL type;

	short posX, posY, posZ;

	ROTATION_AXIS axis;
	short angle;
};

struct P_S2C_UPDATE_PLAYERS_STATE {
	BYTE size;
	PACKET_PROTOCOL type;
	char playerNum;
	 
	char id[MAX_USER];

	short posX[MAX_USER];
	short posY[MAX_USER];
	short posZ[MAX_USER];

	short angle[MAX_USER];

	char hp[MAX_USER];
	char sp[MAX_USER];

	WEAPON_TYPE weaponType[MAX_USER];  
};
#pragma pack (pop)