#include "stdafx.h"
#include "Bridge.h"
#include "Shader.h" 
CBridge::CBridge(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dRootSignature)
{
	m_Name = OBJ_NAME::Bridge; 

	CShader* pShader = new CShader();	
	pShader->CreateVertexShader(L"Shaders\\TerrainAndLight.hlsl", "VSTexturedLighting");
	pShader->CreatePixelShader(L"Shaders\\TerrainAndLight.hlsl", "PSBridgeLight"); 
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, pd3dRootSignature);

	CGameObject* pGameObject = new CGameObject();	 
	 
// �ٴ� ����
	for (int i = 0; i < 10; ++i)
	{
		CPlaneMeshTextured* pPlaneMeshTex = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, -450.0f + 100.0f * i, 0.0f),
			500, 100.0, 250.0f, false); 
		m_Meshes.push_back(pPlaneMeshTex);	

		//pGameObject->SetMesh(pPlaneMeshTex);
		//pGameObject->SetTextureIndex(0x02);
		//pGameObject->SetShader(pShader);
	}
// ���� ����
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

// ���� ��ħ ����
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

// �ٸ� ��ħ ����
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
		//���� ��ü�� ���� ��ȯ ����� ���̴��� ��� ���۷� ����(����)�Ѵ�.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	for (CMesh* mesh : m_Meshes)
	{
		mesh->Render(pd3dCommandList);
	}
	//if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}
