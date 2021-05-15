#pragma once

#define _CRT_SECURE_NO_WARNINGS         // �ֽ� VC++ ������ �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma warning(disable  : 4996)    // mbstowcs unsafe###
#define SERVERIP   "127.0.0.1"		 // ������ 
//#define SERVERIP   "211.207.24.227"// ���� 
//#define SERVERIP	 "192.168.35.229"// ����?
//#define SERVERIP   "192.168.0.15"  // ���� 
 
#define MAX_FILE_NAME_LAN 256

#define UV_TEX 100.0F

#define TERRAIN_HEIGHT_MAP_WIDTH 100
#define TERRAIN_HEIGHT_MAP_HEIGHT 100

#define TERRAIN_DETAIL_HEIGHT_MAP_WIDTH 20000
#define TERRAIN_DETAIL_HEIGHT_MAP_HEIGHT 20000
//-------------------------------------------------------------------
//
#define FPS 1 / 60.0f
#define SERVER_FPS 1 / 30.0f
//�������� ���������� �ִ� ��� ������ �� �������� �����մϴ�.
#define MAX_LOOP_TIME 50

#define TITLE_LENGTH 50

// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 0

#if USE_CAPTIONFPS_ALWAYS
	#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug������ �׻� ����
	#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
	#define MAX_UPDATE_FPS 1.0f / 5.0f
#endif 

#define SHOW_CAPTIONFPS
//-------------------------------------------------------------------
//

#define SWAP_BUFFER_COUNT		2

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE
//#define _WITH_CB_GAMEOBJECT_32BIT_CONSTANTS
//#define _WITH_CB_GAMEOBJECT_ROOT_DESCRIPTOR
#define _WITH_CB_WORLD_MATRIX_DESCRIPTOR_TABLE

#define _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS

#define PARAMETER_STANDARD_TEXTURE		3

#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
	#define PARAMETER_SKYBOX_CUBE_TEXTURE	4
#else
	#define PARAMETER_SKYBOX_CUBE_TEXTURE	4
#endif

#define _WITH_TERRAIN_TESSELATION

#ifdef _WITH_TERRAIN_TESSELATION
	#define _WITH_TERRAIN_PARTITION
#endif
//-------------------------------------------------------------------

//����� ����� ��쿡
//Ŭ���̾�Ʈ�� ����â���� �ܼ�â�� ����ϴ� �ڵ�
#ifdef _DEBUG
#ifdef UNICODE
	#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
	#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

inline UINT CalcCBufferSize(unsigned long long size) { return ((size + 255) & ~255); /*256�� ���*/ }

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#define PI 3.1415926535897

inline double GetRadian(int num)
{
	return num * (PI / 180);
}

struct CB_GAMESCENE_FRAME_DATA
{
	UINT m_PlayerHP = 0;
	UINT m_PlayerSP = 0;
	UINT m_PlayerWeapon = 0x01;
	float m_Time;
};

enum class PulledModel : int { Center = 0, Left = 1, Right, Top, Bottom };

// ��ü ���� �ǰ� ���� ��� ����� ��ƼŬ �ý����� �����մϴ�.
enum class PARTICLE_TYPE : UINT {
	HitParticleTex = 1,	// �ǰݽ� ����� ��ƼŬ
	ArrowParticle,	// ȭ�� �ڿ� ����� ��ƼŬ
	RadialParitcle, // ���������� ������ ��ƼŬ
	StraightParticle, // �������� ������ ��ƼŬ
	RainParticle,
};
constexpr float ARROW_PARTICLE_LIFE_TIME = 10.0f;

constexpr float HIT_PARTICLE_LIFE_TIME = 10.0f;

constexpr float RADIAL_PARTICLE_LIFE_TIME = 6.0f;

constexpr float STRAIGHT_PARTICLE_LIFE_TIME = 10.0f;
