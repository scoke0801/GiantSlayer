#include "stdafx.h"

float GetDetailHeight(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1], float xPosition, float zPosition)
{
	// 1. center
	// 2. left end
	// 3. right end
	const float SCALE_SIZE = 200.0f;
	float fx = xPosition / SCALE_SIZE;
	float fz = zPosition / SCALE_SIZE;

	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다.
	높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면
	지형의 높이는0이다.*/
	if ((fx < 0.0f)
		|| (fz < 0.0f)
		|| (fx > 100)
		|| (fz > 100))
		return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	int BottomLeft = heightsMap[z][x];
	int BottomRight = heightsMap[z][x + 1];
	int TopLeft = heightsMap[z + 1][x];
	int TopRight = heightsMap[z + 1][x + 1];

	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = TopLeft * (1 - fxPercent) + TopRight * fxPercent;
	float fBottomHeight = BottomLeft * (1 - fxPercent) + BottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}
