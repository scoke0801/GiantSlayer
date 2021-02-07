#include "stdafx.h"
#include "Bridge.h"
#include "Shader.h" 
CBridge::CBridge(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height, float depth)
{
	m_Name = OBJ_NAME::Bridge;
	  
// 바닥 생성
	for (int i = 0; i < 10; ++i)
	{
		CPlaneMeshTextured* pPlaneMeshTex = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, -450.0f + 100.0f * i, 0.0f),
			500, 100.0, 250.0f, false); 
		m_Meshes.push_back(pPlaneMeshTex);
	}
// 난간 생성
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(-250.0f, 250.0f+12.5f, 0.0f),
		50, 25, 1000);
	m_Meshes.push_back(pCubeMeshTex); 

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(250.0f, 250.0f + 12.5f, 0.0f),
		50, 25, 1000);
	m_Meshes.push_back(pCubeMeshTex);

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(-250.0f, 250.0f + 137.5f, 0.0f),
		50, 25, 1000);
	m_Meshes.push_back(pCubeMeshTex);

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(250.0f, 250.0f + 137.5f, 0.0f),
		50, 25, 1000);
	m_Meshes.push_back(pCubeMeshTex);

// 난간 받침 생성
	for (int i = 0; i < 5; ++i)
	{
		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-250.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			50, 100, 50);
		m_Meshes.push_back(pCubeMeshTex);

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(250.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			50, 100, 50);
		m_Meshes.push_back(pCubeMeshTex);
	}
// 다리 받침 생성
	for (int i = 0; i < 3; ++i)
	{
		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, 250.0f - 25, -350 + 350 * i),
			550, 50, 150);
		m_Meshes.push_back(pCubeMeshTex);

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, 100, -350 + 350 * i),
			50, 200, 50);
		m_Meshes.push_back(pCubeMeshTex);

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-250.0f, 100, -350 + 350 * i),
			50, 200, 50);
		m_Meshes.push_back(pCubeMeshTex);
		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(250.0f, 100, -350 + 350 * i),
			50, 200, 50);
		m_Meshes.push_back(pCubeMeshTex);
	}
}

CBridge::~CBridge()
{
}

void CBridge::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();
	 
	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	for (CMesh* mesh : m_Meshes)
	{
		mesh->Render(pd3dCommandList);
	}
	//if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}
