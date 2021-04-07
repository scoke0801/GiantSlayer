#pragma once

constexpr int FRAME_BUFFER_WIDTH = 1280;
constexpr int FRAME_BUFFER_HEIGHT = 768;

constexpr int MAX_PLAYER = 5;

constexpr int MAX_PLAYER_NAME = 200;
constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 4096;
constexpr int F_TO_I = 10000;
constexpr int I_TO_F = F_TO_I;

// x,y,z 크기를 short로 보내면 맵 크기 20000에서 
// int, float 형 변환 계산하기에 크기가 작아서 int형으로 사용
inline int FloatToInt(float num)
{ 
	return (int)(num * F_TO_I);
}
inline float IntToFloat(int num) 
{ 
	return (float)num / I_TO_F; 
}

enum class OBJECT_ID : USHORT {
	TERRAIN = 0,
	PLAYER = 1,		// 1 ~ 6 
	
	BRIDEGE_SEC2_SEC3 = 10,
	
	SIGN_SCROLL = 15,
	SIGN_PUZZLE,
	SIGN_MEDUSA,
	SIGN_BOSS,

	DOOR_WALL_SEC1 = 20,
	DOOR_WALL_SEC2,
	DOOR_WALL_SEC3,
	DOOR_WALL_SEC4,
	DOOR_WALL_SEC5,
	 
	MONSTER_MELEE_1 = 30,
	MONSTER_MELEE_2,
	MONSTER_MELEE_3,
	MONSTER_MELEE_4,
	MONSTER_MELEE_5,
	MONSTER_MELEE_6,
	MONSTER_MELEE_7,
	MONSTER_MELEE_8,
	MONSTER_MELEE_9,
	MONSTER_MELEE_10,

	MONSTER_RANGE_1	= 70,	
	MONSTER_RANGE_2,
	MONSTER_RANGE_3,
	MONSTER_RANGE_4,
	MONSTER_RANGE_5,
	MONSTER_RANGE_6,
	MONSTER_RANGE_7,
	MONSTER_RANGE_8,
	MONSTER_RANGE_9,
	MONSTER_RANGE_10,

	BOSS = 100
};
enum class GAME_STATE : short {
	S_STANDBY = 0,	// 로딩하게 된다면..
	S_LOGIN,  
	S_INGAME,
	S_RESULT
};

enum class MOUSE_INPUT_TYPE : short {
	M_LMOVE,
	M_RMOVE,
	M_LBUTTON_UP,
	M_LBUTTON_DOWN,
	M_RBUTTON_UP,
	M_RBUTTON_DOWN
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

	C2S_INGAME_UPDATE_SYNC,

	C2S_LOGOUT,

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

/////////////////////////////////////////////////////////////////
// client protocol packet  
struct P_C2S_LOGIN {
	BYTE size;
	PACKET_PROTOCOL type;
	char name[MAX_PLAYER_NAME];
};

struct P_C2S_LOGOUT {
	BYTE size;
	PACKET_PROTOCOL type;
	short id;
	char name[MAX_PLAYER_NAME];
};

struct P_C2S_KEYBOARD_INPUT {
	BYTE size;
	PACKET_PROTOCOL type;
	short keyInput;
	short id;
};

struct P_C2S_MOUSE_INPUT {
	BYTE size; 
	PACKET_PROTOCOL type;
	MOUSE_INPUT_TYPE InputType;
	short xInput;
	short yInput;
}; 

struct P_C2S_UPDATE_SYNC_REQUEST {
	BYTE size;
	PACKET_PROTOCOL type;
	char playerNum;
};

/////////////////////////////////////////////////////////////////
// server protocol packet

struct P_S2C_PROCESS_LOGIN {
	BYTE size; 
	PACKET_PROTOCOL type;
	short id;
	bool isSuccess;
	int x, y, z;

	bool existPlayer[MAX_PLAYER];
};
struct P_S2C_ADD_PLAYER {
	BYTE size;
	PACKET_PROTOCOL type;

	char id;
	
	int x, y, z;
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

	int posX, posY, posZ;
	short rotateAngle;

	WEAPON_TYPE weaponType;
};

struct P_S2C_PROCESS_MOUSE {
	BYTE size;
	PACKET_PROTOCOL type;

	int posX, posY, posZ;

	ROTATION_AXIS axis;
	short angle;
};

struct P_S2C_UPDATE_SYNC {
	BYTE size;
	PACKET_PROTOCOL type;
	char playerNum;
	 
	char id[MAX_PLAYER]; 

	int posX[MAX_PLAYER];
	int posY[MAX_PLAYER];
	int posZ[MAX_PLAYER];

	short angle[MAX_PLAYER];

	char hp[MAX_PLAYER];
	char sp[MAX_PLAYER];

	WEAPON_TYPE weaponType[MAX_PLAYER];  
};
#pragma pack (pop)