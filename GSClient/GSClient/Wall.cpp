#include "stdafx.h"
#include "Wall.h"

CWall::CWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	m_Name = OBJ_NAME::Wall;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth, true);

	SetMesh(pCubeMeshTex);
}

CWall::~CWall()
{
}

CDoor::CDoor(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float width, float height, float depth
	, bool isLeft)
	:m_IsLeft(isLeft)
{
	m_Name = OBJ_NAME::Door;
	CDoorMesh* pCubeMeshTex = new CDoorMesh(pd3dDevice, pd3dCommandList,
		width, height, depth, isLeft);

	SetMesh(pCubeMeshTex);
}

CDoor::~CDoor()
{
}

void CDoor::Update(double fTimeElapsed)
{
	if (m_IsLeft)
	{
		Rotate(XMFLOAT3(0, 1, 0), -50 * fTimeElapsed);
	}
	else
	{
		Rotate(XMFLOAT3(0, 1, 0), 50 * fTimeElapsed);
	}
}
