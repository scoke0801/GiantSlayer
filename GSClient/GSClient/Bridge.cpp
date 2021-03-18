#include "stdafx.h"
#include "Bridge.h"
#include "Shader.h" 
CBridge::CBridge(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dRootSignature, CShader* pShader)
{
	m_Name = OBJ_NAME::Bridge;  
	 
// 바닥 생성
	for (int i = 0; i < 10; i += 5)
	{
		//CPlaneMeshTextured* pPlaneMeshTex = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList,
		//	XMFLOAT3(0.0f, -451.0f + 100.0f * i, 0.0f),
		//	500, 10.0, 250.0f, false); 
		CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-0.0f, 251.0f, -250.0f + 100.0f * i),
			500, 1, 500);
		//m_Meshes.push_back(pPlaneMeshTex);	

		AddObject(pCubeMeshTex, pShader, 0x01);
	}
// 난간 생성
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,	
		XMFLOAT3(-250.0f, 250.0f+12.5f, 0.0f),
		50, 25, 1000);
	//m_Meshes.push_back(pCubeMeshTex); 
	AddObject(pCubeMeshTex, pShader, 0x02);

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(250.0f, 250.0f + 12.5f, 0.0f),
		50, 25, 1000);
	//m_Meshes.push_back(pCubeMeshTex);
	AddObject(pCubeMeshTex, pShader, 0x02);

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(-250.0f, 250.0f + 137.5f, 0.0f),
		50, 25, 1000);
	//m_Meshes.push_back(pCubeMeshTex);
	AddObject(pCubeMeshTex, pShader, 0x02);

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(250.0f, 250.0f + 137.5f, 0.0f),
		50, 25, 1000);
	//m_Meshes.push_back(pCubeMeshTex);
	AddObject(pCubeMeshTex, pShader, 0x02);

// 난간 받침 생성
	for (int i = 0; i < 5; ++i)
	{
		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-250.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			50, 100, 50);
		//m_Meshes.push_back(pCubeMeshTex);
		AddObject(pCubeMeshTex, pShader, 0x04);

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(250.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			50, 100, 50);
		//m_Meshes.push_back(pCubeMeshTex);
		AddObject(pCubeMeshTex, pShader, 0x04);
	}

// 다리 받침 생성
	for (int i = 0; i < 3; ++i)
	{
		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, 250.0f - 25, -350 + 350 * i),
			550, 50, 150);
		//m_Meshes.push_back(pCubeMeshTex);
		AddObject(pCubeMeshTex, pShader, 0x08);

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, 100, -350 + 350 * i),
			50, 200, 50);
		//m_Meshes.push_back(pCubeMeshTex);
		AddObject(pCubeMeshTex, pShader, 0x08);

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-250.0f, 100, -350 + 350 * i),
			50, 200, 50);
		//m_Meshes.push_back(pCubeMeshTex);
		AddObject(pCubeMeshTex, pShader, 0x08);

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(250.0f, 100, -350 + 350 * i),
			50, 200, 50);
		//m_Meshes.push_back(pCubeMeshTex);
		AddObject(pCubeMeshTex, pShader, 0x08);
	}
}

CBridge::~CBridge()
{
}

void CBridge::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();
	 
	//if (m_pShader)
	//{
	//	//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
	//	m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
	//	m_pShader->Render(pd3dCommandList, pCamera);
	//}
	//for (CMesh* mesh : m_Meshes)
	//{
	//	mesh->Render(pd3dCommandList);
	//}
	//if (m_pMesh) m_pMesh->Render(pd3dCommandList);

	for (CGameObject* pObject : m_Objects)
	{
		pObject->Draw(pd3dCommandList, pCamera);
	}
}

void CBridge::SetPosition(XMFLOAT3 pos)
{
	for (CGameObject* pObject : m_Objects)
	{
		pObject->SetPosition(pos);
	}
	CGameObject::SetPosition(pos);
}

void CBridge::RotateAll(const XMFLOAT3& axis, float angle)
{
	for (CGameObject* pObject : m_Objects)
	{
		pObject->Rotate(axis, angle);
	}
}

void CBridge::AddObject(CMesh* pMesh, CShader* pShader, UINT textureIndex)
{
	CGameObject* pGameObject = new CGameObject();
	pGameObject->SetMesh(pMesh);
	pGameObject->SetTextureIndex(textureIndex);
	pGameObject->SetShader(pShader);
	m_Objects.push_back(pGameObject);
}
