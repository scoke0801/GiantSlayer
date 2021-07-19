#include "stdafx.h"
#include "Skill.h"
#include "Shader.h"

CSkill::CSkill(SKtype type)
{
	switch (type) {
	case SKtype::SK_Sword:
		SetTextureIndex(0x00);
		damage = 10;
		break;
	case SKtype::MG_1:
		SetTextureIndex(0x00);
		damage = 30;
		break;
	}
}

CSkill::~CSkill()
{

}

void CSkill::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_pShader)
	{
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);

	if (gbBoundaryOn)
	{
		m_pShader->RenderBoundary(pd3dCommandList, pCamera);
		for (auto pBoundingMesh : m_BoundingObjectMeshes)
		{
			pBoundingMesh->Render(pd3dCommandList);
		}
	}
}

void CSkill::Update(float fTimeElapsed)
{
	SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, TEST_ARROW_SPEED * fTimeElapsed)));
}

void CSkill::SetSkill(CGameObject* owner)
{
	m_Owner = owner;

	XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ m_Owner->GetPosition() }, { 0,180,0 });
	SetPosition(pos);

	//SetTargetVector(Vector3::Multifly(m_Owner->GetLook(), 1));
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Multifly(m_Owner->GetLook(), 1));
	XMFLOAT3 targetPos = Vector3::Multifly(dirVector, 150000);
	m_xmf3Velocity = dirVector;
	LookAt(m_xmf3Position, targetPos, XMFLOAT3(0, 1, 0));

}

void CSkill::TrackingTarget(CGameObject target)
{

}
