#include "stdafx.h"
#include "Sign.h"

CSign::CSign(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList, 
	bool isFrontSide, CShader* pShader)
{
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		75, 500, 50);
	
	m_Pillar = new CGameObject();
	m_Pillar->SetMesh(pCubeMeshTex);
	m_Pillar->SetShader(pShader);
	m_Pillar->SetTextureIndex(0x01);

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		500, 300, 10);
	
	CPlaneMeshTextured* pMesh = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList, 500, 300, 10);

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