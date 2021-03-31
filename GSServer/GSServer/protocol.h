#pragma once

enum class OP_PLAYER_INPUT {
	move,
	attack
};
#pragma pack (push, 1)
struct test_packet {
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
	XMFLOAT3 xmf3Dir; 
};
#pragma pack (pop)