#pragma once

#define _CRT_SECURE_NO_WARNINGS         // �ֽ� VC++ ������ �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma warning(disable  : 4996)    // mbstowcs unsafe###
#define SERVERIP   "127.0.0.1"		 // ������
#define SERVERPORT 9000
#define MAX_FILE_NAME_LAN 256

//-------------------------------------------------------------------
//
#define FPS 1 / 60.0f
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
#define FRAME_BUFFER_WIDTH		1024
#define FRAME_BUFFER_HEIGHT		768

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