#include "stdafx.h"
#include "Bridge.h"
#include "Shader.h" 
CBridge::CBridge(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dRootSignature, CShader* pShader)
{
	m_Name = OBJ_NAME::Bridge;  
	int idx = -1;
// 바닥 생성
	for (int i = 0; i < 10; i += 5)
	{ 
		CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-0.0f, 251.0f, -250.0f + 100.0f * i),
			1000, 1, 500);
		 
		idx = AddObject(pCubeMeshTex, pShader, 0x01);

		m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 1000, 1, 500,
			XMFLOAT3(-0.0f, 251.0f, -250.0f + 100.0f * i));
		m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(-0.0f, 251.0f, -250.0f + 100.0f * i), 
			XMFLOAT3(1000 * 0.5f, 1 * 0.5f, 500 * 0.5f)));
	}
// 난간 생성
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,	
		XMFLOAT3(-500.0f, 250.0f + 12.5f, 0.0f),
		50, 25, 1000); 
	idx = AddObject(pCubeMeshTex, pShader, 0x02);
	m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50, 25, 1000,
		XMFLOAT3(-500.0f, 250.0f + 12.5f, 0.0f));
	m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(-500.0f, 250.0f + 12.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f)));

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(500.0f, 250.0f + 12.5f, 0.0f),
		50, 25, 1000); 
	idx = AddObject(pCubeMeshTex, pShader, 0x02);
	m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50, 25, 1000,
		XMFLOAT3(500.0f, 250.0f + 12.5f, 0.0f));
	m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(500.0f, 250.0f + 12.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f)));
	 
	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(-500.0f, 250.0f + 137.5f, 0.0f),
		50, 25, 1000); 
	idx = AddObject(pCubeMeshTex, pShader, 0x02);
	m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50, 25, 1000,
		XMFLOAT3(-500.0f, 250.0f + 137.5f, 0.0f));
	m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(-500.0f, 250.0f + 137.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f)));

	pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		XMFLOAT3(500.0f, 250.0f + 137.5f, 0.0f),
		50, 25, 1000); 
	idx = AddObject(pCubeMeshTex, pShader, 0x02);
	m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50, 25, 1000,
		XMFLOAT3(500.0f, 250.0f + 137.5f, 0.0f));
	m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(500.0f, 250.0f + 137.5f, 0.0f),
		XMFLOAT3(50 * 0.5f, 25 * 0.5f, 1000 * 0.5f)));

// 난간 받침 생성
	for (int i = 0; i < 5; ++i)
	{
		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-500.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			50, 100, 50); 
		idx = AddObject(pCubeMeshTex, pShader, 0x04);
		m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50, 100, 50,
			XMFLOAT3(-500.0f, 250.0f + 75.0f, -420.0f + 200 * i));
		m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(-500.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			XMFLOAT3(50 * 0.5f, 100 * 0.5f, 50 * 0.5f)));

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(500.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			50, 100, 50); 
		idx = AddObject(pCubeMeshTex, pShader, 0x04);
		m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50, 100, 50,
			XMFLOAT3(500.0f, 250.0f + 75.0f, -420.0f + 200 * i));
		m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(500.0f, 250.0f + 75.0f, -420.0f + 200 * i),
			XMFLOAT3(50 * 0.5f, 100 * 0.5f, 50 * 0.5f)));
	}

// 다리 받침 생성
	for (int i = 0; i < 3; ++i)
	{
		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, 250.0f - 25.0f, -350.0f + 350.0f * i),
			1050.0f, 50.0f, 150.0f);
		idx = AddObject(pCubeMeshTex, pShader, 0x08);
		m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 1050.0f, 50.0f, 150.0f,
			XMFLOAT3(0.0f, 250.0f - 25, -350 + 350 * i));
		m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(0.0f, 250.0f - 25.0f, -350.0f + 350.0f * i),
			XMFLOAT3(1050.0f * 0.5f, 50.0f * 0.5f, 150.0f * 0.5f)));

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(0.0f, 0.0f, -350.0f + 350.0f * i),
			50.0f, 400.0f, 50.0f);
		idx = AddObject(pCubeMeshTex, pShader, 0x08);
		m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50.0f, 400.0f, 50.0f,
			XMFLOAT3(0.0f, 0.0f, -350.0f + 350.0f * i));
		m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, -350.0f + 350.0f * i),
			XMFLOAT3(50.0f * 0.5f, 400.0f * 0.5f, 50.0f * 0.5f)));

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(-500.0f, 0.0f, -350.0f + 350.0f * i),
			50.0f, 400.0f, 50.0f);
		idx = AddObject(pCubeMeshTex, pShader, 0x08);
		m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50.0f, 400.0f, 50.0f,
			XMFLOAT3(-500.0f, 0.0f, -350.0f + 350 * i));
		m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(-500.0f, 0.0f, -350.0f + 350.0f * i),
			XMFLOAT3(50.0f * 0.5f, 400.0f * 0.5f, 50.0f * 0.5f)));

		pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
			XMFLOAT3(500.0f, 0.0f, -350.0f + 350.0f * i),
			50, 400, 50); 
		idx = AddObject(pCubeMeshTex, pShader, 0x08);
		m_Objects[idx]->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center, 50.0f, 400.0f, 50.0f,
			XMFLOAT3(500.0f, 0.0f, -350.0f + 350.0f * i));
		m_Objects[idx]->AddColider(new ColliderBox(XMFLOAT3(-500.0f, 0.0f, -350.0f + 350.0f * i),
			XMFLOAT3(50.0f * 0.5f, 400.0f * 0.5f, 50.0f * 0.5f)));
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

void CBridge::Draw_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (CGameObject* pObject : m_Objects)
	{
		pObject->Draw_Shadow(pd3dCommandList, pCamera);
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

void CBridge::UpdateColliders()
{
	for (CGameObject* pObject : m_Objects)
	{
		pObject->UpdateColliders();	 
	}
}

bool CBridge::CollisionCheck(Collider* pAABB)
{
	for (CGameObject* pObject : m_Objects)
	{
		if (pObject->CollisionCheck(pAABB)) {
			return true;
		}
	} 
	return false;
}

int CBridge::AddObject(CMesh* pMesh, CShader* pShader, UINT textureIndex)
{
	CGameObject* pGameObject = new CGameObject();
	pGameObject->SetMesh(pMesh);
	pGameObject->SetTextureIndex(textureIndex);
	pGameObject->SetShader(pShader);
	m_Objects.push_back(std::move(pGameObject));
	return m_Objects.size() - 1;
}
