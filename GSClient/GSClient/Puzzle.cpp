#include "stdafx.h"
#include "Puzzle.h"

CPuzzle::CPuzzle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, PuzzleType type, CShader* pShader)
{
	m_ExistingSector = SECTOR_POSITION::SECTOR_3;
	CMesh* pMesh = nullptr;
	switch (type)
	{
	case PuzzleType::Holding:

		break;
	case PuzzleType::Mirror:
		break;
	}
	if (pMesh != nullptr) SetMesh(pMesh);

	CMesh* pXMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 300, 20, 50);
	CMesh* pYMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 350, 20, 50); 
	for (int i = 1; i <= 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			CGameObject* pObject = new CGameObject();
			pObject->SetMesh(pXMesh);
			pObject->SetShader(pShader);
			pObject->SetPosition(XMFLOAT3(600.0f * i + 300.0f, 250.0f, 150.0f + 410.0f + 600.0f * j)); 
			pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 300, 20, 50, XMFLOAT3{ 0,0,0 });
			pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 300 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.push_back(std::move(pObject));

			pObject = new CGameObject();
			pObject->SetMesh(pXMesh);
			pObject->SetShader(pShader);
			pObject->SetPosition(XMFLOAT3(600.0f * i + 300.0f, 250.0f, -150.0f + 410.0f + 600.0f * j));			
			pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 300, 20, 50, XMFLOAT3{ 0,0,0 });
			pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 300 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.push_back(std::move(pObject));

			pObject = new CGameObject();
			pObject->SetMesh(pYMesh);
			pObject->SetShader(pShader);
			pObject->SetPosition(XMFLOAT3(175.0f + 600.0f * i + 300.0f, 250.0f, 0.0f + 410.0f + 600.0f * j));
			pObject->Rotate(XMFLOAT3(0, 1, 0), 90);		
			pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 350, 20, 50, XMFLOAT3{ 0,0,0 }); 
			pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 350 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.push_back(std::move(pObject));

			pObject = new CGameObject();
			pObject->SetMesh(pYMesh);
			pObject->SetShader(pShader);
			pObject->SetPosition(XMFLOAT3(-175.f + 600.0f * i + 300.0f, 250.0f, 0.0f + 410.0f + 600.0f * j));
			pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
			pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 350, 20, 50, XMFLOAT3{ 0,0,0 });
			pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 350 * 0.5f, 20 * 0.5f, 50 * 0.5f }));
			m_Objects.push_back(std::move(pObject));
		} 
	}

	m_CollisionHandleType = COLLISION_HANDLE_TYPE::NotCollide;
}

CPuzzle::~CPuzzle()
{
}

void CPuzzle::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto pObj : m_Objects)
	{
		pObj->Draw(pd3dCommandList, pCamera);
	} 
}

void CPuzzle::Draw_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto pObj : m_Objects)
	{
		pObj->Draw_Shadow(pd3dCommandList, pCamera);
	}
}

bool CPuzzle::CollisionCheck(Collider* pAABB)
{
	for (auto pObj : m_Objects) {
		if (pObj->CollisionCheck(pAABB)) return true;
	}
	return false;
}

void CPuzzle::SetPosition(XMFLOAT3 pos)
{
	for (auto pObj : m_Objects)
	{
		pObj->SetPositionPlus(pos);
	} 
	//CGameObject::SetPosition(pos);
}

CPlate::CPlate(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{ 
	CMesh* pMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 3000, 240, 4000);
	m_ExistingSector = SECTOR_POSITION::SECTOR_3;
	CGameObject* pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader);
	pObject->SetPosition({ 1250.0f, 120.f, 900.0f });
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 3000, 240, 4000, XMFLOAT3{ 0,0,0 });
	pObject->SetTextureIndex(0x04);
	pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 3000 * 0.5f, 240 * 0.5f, 4000 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->SetSize({ 1, 240 * 0.5f, 1 });
	m_Objects.push_back(std::move(pObject));

	pMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 700, 80, 600);
	pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader);
	pObject->Rotate(XMFLOAT3(1, 0, 0), -20.0f);
	pObject->SetPosition(XMFLOAT3(1250.0f, 100.0f, -1370));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 700, 80, 600, XMFLOAT3{ 0,0,0 });
	pObject->SetTextureIndex(0x04);
	pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 600 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->SetSize({ 1, 80 * 0.5f, 1 });
	m_Objects.push_back(std::move(pObject));

	pMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 700, 80, 600);
	pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader);
	pObject->Rotate(XMFLOAT3(1, 0, 0), 20.0f);
	pObject->SetPosition(XMFLOAT3(1250.0f, 100.0f, 3170));
	pObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, 700, 80, 600, XMFLOAT3{ 0,0,0 });
	pObject->SetTextureIndex(0x04);
	pObject->AddColider(new ColliderBox(XMFLOAT3{ 0,0,0 }, XMFLOAT3{ 700 * 0.5f, 80 * 0.5f, 600 * 0.5f }));
	pObject->SetCollisionHandleType(COLLISION_HANDLE_TYPE::On);
	pObject->SetSize({ 1, 80 * 0.5f, 1 });
	m_Objects.push_back(std::move(pObject));
	
}

CPlate::~CPlate()
{
}

void CPlate::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto pObj : m_Objects)
	{
		pObj->Draw(pd3dCommandList, pCamera);
	}
}

void CPlate::RotateAll(const XMFLOAT3& axis, float angle)
{
	for (auto pObj : m_Objects)
	{
		pObj->Rotate(axis, angle);
	}
}

void CPlate::UpdateColliders()
{
	for (auto pObj : m_Objects) {
		pObj->UpdateColliders();
	}
}

bool CPlate::CollisionCheck(Collider* pAABB)
{
	bool res = false;
	for (auto pObj : m_Objects) {
		if (pObj->CollisionCheck(pAABB)) {
			m_CollideObject->FixCollision(pObj);
			//return true;
			res = true;
		}
	}
	return res;
}

bool CPlate::CollisionCheck(CGameObject* other)
{
	bool res = false;
	auto otherAABB = other->GetAABB();
	m_CollideObject = other;
	for (int i = 0; i < otherAABB.size(); ++i) {
		bool result = CollisionCheck(otherAABB[i]);
		if (result) {
			//other->FixCollision(m_CollideObject);
			//return true;
			res = true;
		}
	}

	return res;
}

void CPlate::SetPosition(XMFLOAT3 pos)
{
	for (auto pObj : m_Objects)
	{
		pObj->SetPositionPlus(pos);
	}
	//CGameObject::SetPosition(pos);
}
