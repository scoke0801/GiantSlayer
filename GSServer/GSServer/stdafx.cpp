#include "stdafx.h"

float GetDetailHeight(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1], float xPosition, float zPosition)
{
	// 1. center
	// 2. left end
	// 3. right end
	const float SCALE_SIZE = 200.0f;
	float fx = xPosition / SCALE_SIZE;
	float fz = zPosition / SCALE_SIZE;

	/*������ ��ǥ (fx, fz)�� �̹��� ��ǥ���̴�.
	���� ���� x-��ǥ�� z-��ǥ�� ���� ���� ������ �����
	������ ���̴�0�̴�.*/
	if ((fx < 0.0f)
		|| (fz < 0.0f)
		|| (fx > 100)
		|| (fz > 100))
		return(0.0f);
	//���� ���� ��ǥ�� ���� �κа� �Ҽ� �κ��� ����Ѵ�.

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	int BottomLeft = heightsMap[z][x];
	int BottomRight = heightsMap[z][x + 1];
	int TopLeft = heightsMap[z + 1][x];
	int TopRight = heightsMap[z + 1][x + 1];

	//�簢���� �� ���� �����Ͽ� ����(�ȼ� ��)�� ����Ѵ�.
	float fTopHeight = TopLeft * (1 - fxPercent) + TopRight * fxPercent;
	float fBottomHeight = BottomLeft * (1 - fxPercent) + BottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}
