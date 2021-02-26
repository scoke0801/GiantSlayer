#include "stdafx.h"
#include "GameSceneProcessor.h"

bool GameSceneProcessor::ProcessGameScene(SOCKET& socket)
{
	char buffer[BUFSIZE + 1];
	int receivedSize = 0;
	int count = 0;
	int retval = 0;
	RecvFrameData(socket, buffer, receivedSize);
	char* token = strtok(buffer, "\n");

	while (token != NULL)
	{
		if (strstr(token, "<PlayerPosition>:"))
		{
			XMFLOAT3 res = GetVectorFromText(token);
			cout << "<PlayerPosition>: ";
			DisplayVector3(res);
		}
		else if (strstr(token, "<PlayerLook>:"))
		{
			XMFLOAT3 res = GetVectorFromText(token);
			cout << "<PlayerLook>: ";
			DisplayVector3(res);
		}
		token = strtok(NULL, "\n");
	}

	string toSendData = "\n";
	XMFLOAT3 xmf3PlayerPos = { 0,0,0 };// m_Player->GetPosition();
	XMFLOAT3 xmf3PlayerLook = { 0,0,0 }; //m_Player->GetLook();
	toSendData += "<PlayerPosition>:\n";
	toSendData += to_string(xmf3PlayerPos.x);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerPos.y);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerPos.z);
	toSendData += "\n";
	toSendData += "<PlayerLook>:\n";
	toSendData += to_string(xmf3PlayerLook.x);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerLook.y);
	toSendData += " ";
	toSendData += to_string(xmf3PlayerLook.z);
	toSendData += "\n";
	SendFrameData(socket, toSendData, retval);

	return true;
}
