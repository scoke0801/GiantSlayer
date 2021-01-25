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

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height, float depth)
{
	CPlaneMeshTextured* pPlaneMeshTex = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);

	SetMesh(pPlaneMeshTex);
}

UI::~UI()
{
}
