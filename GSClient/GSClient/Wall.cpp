#include "stdafx.h"
#include "Wall.h"

CWall::CWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	m_Name = OBJ_NAME::Wall;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth, true);
	m_Type = OBJ_TYPE::Obstacle;
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
	m_Type = OBJ_TYPE::Obstacle;
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
 
CDoorWall::CDoorWall(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth,
	CShader* pShader)
{
	float fWidthRatio = width * 0.1f;

	CWall* pWall = new CWall(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth);
	pWall->SetShader(pShader);
	pWall->SetObjectName(OBJ_NAME::Wall);
	pWall->SetTextureIndex(0x200);
	pWall->SetPosition({ fWidthRatio * 2,  height * 0.5f, depth * 0.5f });
	m_Walls.push_back(pWall);
	  
	pWall = new CWall(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth);
	pWall->SetShader(pShader);
	pWall->SetObjectName(OBJ_NAME::Wall);
	pWall->SetTextureIndex(0x200); 
	pWall->SetPosition({ fWidthRatio * 6 + fWidthRatio * 2,  height * 0.5f, depth * 0.5f });
	m_Walls.push_back(pWall);  

	m_LeftDoor = new CDoor(pd3dDevice, pd3dCommandList, fWidthRatio, height, depth * 0.2f, true);
	m_LeftDoor->SetShader(pShader);
	m_LeftDoor->SetObjectName(OBJ_NAME::Wall);
	m_LeftDoor->SetTextureIndex(0x400);
	m_LeftDoor->SetPosition({ fWidthRatio * 4,  height * 0.5f, depth * 0.5f });

	m_RightDoor = new CDoor(pd3dDevice, pd3dCommandList, fWidthRatio, height, depth * 0.2f, false);
	m_RightDoor->SetShader(pShader);
	m_RightDoor->SetObjectName(OBJ_NAME::Wall);
	m_RightDoor->SetTextureIndex(0x400); 
	m_RightDoor->SetPosition({ fWidthRatio * 6,  height * 0.5f, depth * 0.5f });
}

CDoorWall::~CDoorWall()
{
}

void CDoorWall::Update(double fTimeElapsed)
{
	m_LeftDoor->Update(fTimeElapsed);
	m_RightDoor->Update(fTimeElapsed);
	for (CWall* pWall : m_Walls)
	{
		pWall->Update(fTimeElapsed);
	}
}

void CDoorWall::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_LeftDoor->Draw(pd3dCommandList, pCamera);
	m_RightDoor->Draw(pd3dCommandList, pCamera);
	for (CWall* pWall : m_Walls)
	{
		pWall->Draw(pd3dCommandList, pCamera);
	}
}

void CDoorWall::SetPosition(XMFLOAT3 xmf3Position)
{
	m_LeftDoor->SetPositionPlus(xmf3Position);
	m_RightDoor->SetPositionPlus(xmf3Position);
	for (CWall* pWall : m_Walls)
	{
		pWall->SetPositionPlus(xmf3Position);
	}
}
