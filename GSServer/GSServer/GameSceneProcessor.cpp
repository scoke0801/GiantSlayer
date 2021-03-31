#include "stdafx.h"
#include "GameSceneProcessor.h"
#include "protocol.h"

bool GameSceneProcessor::ProcessGameScene(SOCKET& socket)
{
	char buffer[BUFSIZE + 1] = {};
	int receivedSize = 0;
	int count = 0;
	int retval = 0;
	RecvFrameData(socket, buffer, receivedSize);

	test_packet tp = *reinterpret_cast<test_packet*>(buffer); 
	  
	SendFrameData(socket, tp, retval);

	return true;
}
