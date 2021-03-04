#include "stdafx.h"
#include "Puzzle.h"

CPuzzle::CPuzzle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, PuzzleType type, CShader* pShader)
{
	CMesh* pMesh = nullptr; 
	switch (type)
	{
	case PuzzleType::Holding:
		
		break;
	case PuzzleType::Mirror:
		break;
	}
	if (pMesh != nullptr) SetMesh(pMesh);

	pMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 150, 50, 50);

	CGameObject* pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader);
	pObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 100.0f));
	m_Objects.push_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader);	
	pObject->SetPosition(XMFLOAT3(0.0f, 0.0f, -100.0f));
	m_Objects.push_back(std::move(pObject));

	pMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 250, 50, 50);
	pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader); 
	pObject->SetPosition(XMFLOAT3(100.0f, 0.0f, 0.0f));
	pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
	m_Objects.push_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader);
	pObject->SetPosition(XMFLOAT3(-100.0f, 0.0f, 0.0f));
	pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
	m_Objects.push_back(std::move(pObject));

	pMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 150, 100, 150);
	pObject = new CGameObject();
	pObject->SetMesh(pMesh);
	pObject->SetShader(pShader); 
	pObject->SetTextureIndex(0x02);
	m_Objects.push_back(std::move(pObject));
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

void CPuzzle::SetPosition(XMFLOAT3 pos)
{
	for (auto pObj : m_Objects)
	{
		pObj->SetPositionPlus(pos);
	}
}

