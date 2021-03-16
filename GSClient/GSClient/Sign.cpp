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
	m_Pillar->SetTextureIndex(0x80);

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		500, 300, 10);
	
	CPlaneMeshTextured* pMesh = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList, 500, 300, 10);

	
	m_WallPaper = new CGameObject();
	m_WallPaper->SetMesh(pMesh);
	m_WallPaper->SetShader(pShader);
	m_WallPaper->SetTextureIndex(0x80);
	if (isFrontSide)
	{
		m_WallPaper->SetPosition({ 0, 250, -40 });
	}
	else
	{
		m_WallPaper->SetPosition({ 0, 250, 20 });
	}
}

CSign::~CSign()
{
}

void CSign::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_WallPaper->Draw(pd3dCommandList, pCamera);
	m_Pillar->Draw(pd3dCommandList, pCamera);
}

void CSign::SetPosition(XMFLOAT3 pos)
{
	m_WallPaper->SetPositionPlus(pos);
	m_Pillar->SetPositionPlus(pos);
}

void CSign::RotateAll(const XMFLOAT3& axis, float angle)
{
	m_WallPaper->Rotate(axis, angle);
	m_Pillar->Rotate(axis, angle);
}