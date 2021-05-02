#include "stdafx.h"
#include "UI.h"

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CBillboardVertex* pBillboardVertices = new CBillboardVertex;

	pBillboardVertices->m_xmf3Position = XMFLOAT3(0, 0, -100);
	pBillboardVertices->m_xmf2Size = XMFLOAT2(100.0f, 100.0f);
	pBillboardVertices->m_nTexture = 1;

	m_pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList,
		pBillboardVertices, 1);
	SetMesh(m_pBillboardMesh);
	if (pBillboardVertices) delete pBillboardVertices;
}

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth, 
	bool IsHalfSize)
{
	CPlaneMeshTextured* pPlaneMeshTex = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth,
		true,
		IsHalfSize);

	SetMesh(pPlaneMeshTex);
}

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float radius)
{
	CMinimapMesh* pMinimapMeshTex = new CMinimapMesh(pd3dDevice, pd3dCommandList, radius);

	SetMesh(pMinimapMeshTex);
}

UI::~UI()
{
}

void UI::Rotate(float angle)
{
	CGameObject::Rotate(XMFLOAT3(0, 0, 1), angle); 
}

Minimap::Minimap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float radius)
{
	CMinimapMesh* pMinimapMeshTex = new CMinimapMesh(pd3dDevice, pd3dCommandList, radius);

	SetMesh(pMinimapMeshTex);
}

Minimap::~Minimap()
{
}

MinimapArrow::MinimapArrow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	CMinimapAroowMesh* pMinimapArrowMesh = new CMinimapAroowMesh(pd3dDevice, pd3dCommandList,
		width, height, depth);
	SetMesh(pMinimapArrowMesh);
}

MinimapArrow::~MinimapArrow()
{
}

HpSpPercentUI::HpSpPercentUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float width, float height, float depth,
	bool isHp)
{
	CHpSpPercentMesh* pMesh = new CHpSpPercentMesh(pd3dDevice, pd3dCommandList,
		width, height, depth, 
		isHp);
	SetMesh(pMesh);
}

HpSpPercentUI::~HpSpPercentUI()
{
}

HelpTextUI::HelpTextUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float width, float height, float depth, 
	HELP_TEXT_INFO helpTextInfo)
{ 
	CPlaneMeshTextured* pMesh = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList,
		0.0f, ((int)helpTextInfo * 0.1f),
		1.0f, ((int)helpTextInfo * 0.1f) + 0.1f,
		width, height, depth);
	SetMesh(pMesh);
}

HelpTextUI::~HelpTextUI()
{
}

void HelpTextUI::Update(float fTimeElapsed)
{
	if (m_RenderingTime > 0.0f)
	{
		m_RenderingTime -= fTimeElapsed; 
	}
}

void HelpTextUI::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_RenderingTime > 0.0f)
	{
		UI::Draw(pd3dCommandList, pCamera);
	}
}
