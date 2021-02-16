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
	CDoorMeshTest* pCubeMeshTex = new CDoorMeshTest(pd3dDevice, pd3dCommandList,
		width, height, depth, isLeft);

	SetMesh(pCubeMeshTex);
	if (!m_IsLeft) Rotate(XMFLOAT3(0, 1, 0), 180.0);
}

CDoor::~CDoor()
{
}

void CDoor::Update(double fTimeElapsed)
{ 
	if (m_IsLeft)
	{
		float rotateAngle = 50;
		if (!m_IsOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 89.0f)
		{
			m_IsOpening = false;
		}
		else if (m_fAngle < -89.0f)
		{
			m_IsOpening = true;
		}
	}
	else
	{
		float rotateAngle = -50;
		if (!m_IsOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 89.0f)
		{
			m_IsOpening = true;
		}
		else if (m_fAngle < -89.0f)
		{
			m_IsOpening = false;
		}
	}
}
