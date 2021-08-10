#include "stdafx.h"
#include "Scene.h"
 
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneJH.h" 
#include "SceneTH.h"
#include "SceneYJ.h" 
#include "GameScene2.h"
CScene::CScene()
{

}

CScene::~CScene()
{

}
void CScene::DoRecv()
{
	g_Client.m_recv_over.m_wsabuf.buf = reinterpret_cast<char*>(g_Client.m_recv_over.m_packetbuf) + g_Client.m_prev_size;
	g_Client.m_recv_over.m_wsabuf.len = MAX_BUFFER - g_Client.m_prev_size;

	memset(&g_Client.m_recv_over.m_over, 0, sizeof(g_Client.m_recv_over.m_over));
	
	DWORD iobyte, ioflag = 0;	
	int ret = WSARecv(g_Client.m_socket, &g_Client.m_recv_over.m_wsabuf, 1,
		&iobyte, &ioflag, NULL, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in RecvPacket: ");
	}

	unsigned char* packet_ptr = g_Client.m_recv_over.m_packetbuf;
	int num_data = iobyte + g_Client.m_prev_size;
	int packet_size = packet_ptr[0];
	 
	while (num_data >= packet_size) {
		CFramework::GetInstance().GetCurrentScene()->ProcessPacket(packet_ptr);
		num_data -= packet_size;
		packet_ptr += packet_size;
		if (0 >= num_data) break;
		packet_size = packet_ptr[0];
	}
	g_Client.m_prev_size = num_data;
	if (0 != num_data) {
		memcpy(g_Client.m_recv_over.m_packetbuf, packet_ptr, num_data);
	}
}

CNullScene::CNullScene()
{

}

CNullScene::~CNullScene()
{

}

void CNullScene::Update(float elapsedTime)
{ 
	ProcessInput(); 
}

void CNullScene::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{   

}

void CNullScene::ProcessInput()
{
	auto gameInput = GAME_INPUT;
	if (gameInput.KEY_F1)
	{
		//ChangeScene<CTitleScene>();
		ChangeScene<CTitleScene2>();
	}
	if (gameInput.KEY_F2)
	{
		ChangeScene<CSceneTH>();
	}
	if (gameInput.KEY_F3)
	{ 
		ChangeScene<CSceneYJ>();
	}
	if (gameInput.KEY_F4)
	{
		ChangeScene<CSceneJH>();
	}  
	if (gameInput.KEY_F5)
	{
		ChangeScene<CGameScene>();
	}
}
