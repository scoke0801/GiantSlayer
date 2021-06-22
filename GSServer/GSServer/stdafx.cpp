#include "stdafx.h" 

int* g_GridHeights[25][25] = { nullptr, };
XMFLOAT3 PLAYER_START_POSITIONS[MAX_PLAYER] = {
   { 550.0f,   230.0f,  1850.0f },
   { 850.0f,   230.0f,  1850.0f },
   { 1250.0f,  230.0f,  1850.0f },
   { 850.0f,   230.0f,  2200.0f },
   { 850.0f,   230.0f,  1500.0f }
};
void BernsteinCoeffcient5x5(float t, float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float CubicBezierSum5x5_C(float uB[5], float vB[5], int xIndex, int zIndex)
{
	float f3Sum = 0.0f;

	int* pHeights = g_GridHeights[zIndex / 4][xIndex / 4];
	f3Sum = vB[0] * (uB[0] * pHeights[0] + uB[1] * pHeights[1] + uB[2] * pHeights[2] + uB[3] * pHeights[3] + uB[4] * pHeights[4]);
	f3Sum += vB[1] * (uB[0] * pHeights[5] + uB[1] * pHeights[6] + uB[2] * pHeights[7] + uB[3] * pHeights[8] + uB[4] * pHeights[9]);
	f3Sum += vB[2] * (uB[0] * pHeights[10] + uB[1] * pHeights[11] + uB[2] * pHeights[12] + uB[3] * pHeights[13] + uB[4] * pHeights[14]);
	f3Sum += vB[3] * (uB[0] * pHeights[15] + uB[1] * pHeights[16] + uB[2] * pHeights[17] + uB[3] * pHeights[18] + uB[4] * pHeights[19]);
	f3Sum += vB[4] * (uB[0] * pHeights[20] + uB[1] * pHeights[21] + uB[2] * pHeights[22] + uB[3] * pHeights[23] + uB[4] * pHeights[24]);

	return(f3Sum);
}

float GetDetailHeight(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1], float xPosition, float zPosition)
{
	// 1. center
	// 2. left end
	// 3. right end
	const float SCALE_SIZE = 200.0f;
	float fx = xPosition / SCALE_SIZE;
	float fz = zPosition / SCALE_SIZE;

	int x = (int)fx;
	int z = (int)fz;
	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다.
	높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면
	지형의 높이는0이다.*/
	if ((fx < 0.0f)
		|| (fz < 0.0f)
		|| (fx >= 100)
		|| (fz >= 100))
		return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.

	bool found = false;
	float minX, maxX, minZ, maxZ;
	float gridX = 4;
	float gridZ = 4;
	float height = 0.0f;
	for (int i = 0; i < 100; ++i) {
		minX = i * gridX;
		maxX = (i + 1) * gridX;

		if (!(minX <= fx && fx <= maxX)) {
			continue;
		}

		for (int j = 0; j < 100; ++j) {
			minZ = j * gridZ;
			maxZ = (j + 1) * gridZ;

			if (!(minZ <= fz && fz <= maxZ)) {
				continue;
			}
			float n_x = InvLerp(minX, maxX, fx);
			float n_z = InvLerp(minZ, maxZ, fz);

			height = GetHeighty(n_x, n_z, x, z);
			found = true;
			break;
		}
		break;
	}
	if (found) {
		return height;
	}
	else {
		return 0.0f;
	}
}

float GetHeighty(float nx, float nz, int xIndex, int zIndex)
{
	float uB[5], vB[5];
	BernsteinCoeffcient5x5(nx, uB);
	BernsteinCoeffcient5x5(1.0f - nz, vB);

	return CubicBezierSum5x5_C(uB, vB, xIndex, zIndex);
}
