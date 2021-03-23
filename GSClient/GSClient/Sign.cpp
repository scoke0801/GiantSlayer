#include "stdafx.h"
#include "Sign.h"

CSign::CSign(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList, 
	SignBoardInfos boardInfo, 
	bool isRotated, bool isFrontSide,
	CShader* pShader)
{
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		75, 500, 50);
	
	m_Pillar = new CGameObject();
	m_Pillar->SetMesh(pCubeMeshTex);
	m_Pillar->SetShader(pShader);
	m_Pillar->SetTextureIndex(0x01);

	float width = 500.0f, height = 300.0f, depth = 10.0f; 
	if (isRotated) 
	{
		width = 300.0f; height = 500.0f;
	}
	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);
	CPlaneMeshTextured* pMesh;
	switch (boardInfo)
	{
	case SignBoardInfos::None:
		pMesh = new CPlaneMeshTextured(pd3dDevice,
			pd3dCommandList,
			0.0f, 0.0f,
			1.0f, 0.25f,
			500.0f, 300.0f, 10.0f);
		break;

	case SignBoardInfos::Scroll:
		pMesh = new CPlaneMeshTextured(pd3dDevice,
			pd3dCommandList,
			0.0f, 0.25f,
			1.0f, 0.5f,
			500.0f, 300.0f, 10.0f);
		break;

	case SignBoardInfos::NumPuzzle:
		pMesh = new CPlaneMeshTextured(pd3dDevice,
			pd3dCommandList,
			0.0f, 0.5f,
			1.0f, 0.75f,
			500.0f, 300.0f, 10.0f);
		break;

	case SignBoardInfos::Medusa:
		pMesh = new CPlaneMeshTextured(pd3dDevice,
			pd3dCommandList,
			0.0f, 0.75f,
			1.0f, 1.0f,
			500.0f, 300.0f, 10.0f);
		break;
	}

	m_Board = new CGameObject();
	m_Board->SetMesh(pMesh);
	m_Board->SetShader(pShader);
	
	m_Board->SetTextureIndex(0x02);

	if (isFrontSide)
	{
		m_Board->SetPosition({ 0, 250, -40 });
	}
	else
	{
		m_Board->SetPosition({ 0, 250, 20 });
	}

	if (isRotated) {
		m_Board->SetPosition({ -40, 250, 0.0f });
	}
}

CSign::~CSign()
{
}

void CSign::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_Board->Draw(pd3dCommandList, pCamera);
	m_Pillar->Draw(pd3dCommandList, pCamera);
}

void CSign::SetPosition(XMFLOAT3 pos)
{
	m_Board->SetPositionPlus(pos);
	m_Pillar->SetPositionPlus(pos);
}

void CSign::RotateAll(const XMFLOAT3& axis, float angle)
{
	m_Board->Rotate(axis, angle);
	m_Pillar->Rotate(axis, angle);
}