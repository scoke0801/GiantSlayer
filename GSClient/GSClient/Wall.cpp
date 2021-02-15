#include "stdafx.h"
#include "Wall.h"

CWall::CWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	m_Name = OBJ_NAME::Wall;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);

	SetMesh(pCubeMeshTex);
}

CWall::~CWall()
{
}
