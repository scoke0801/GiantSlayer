#include "stdafx.h"
#include "MummyLaser.h"
#include "Shader.h"
#include "Particle.h"
#include "Enemy.h"

void CMummyLaser::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (true == m_isDrawable) return;
	OnPrepareRender();

	
	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
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

void CMummyLaser::Update(float fTimeElapsed)
{
	if (false == m_isDrawable) {
		m_ConnectedParticle->SetDrawable(true);
		SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, ARROW_SPEED * fTimeElapsed)));
		Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), 15.0f+m_ElapsedTime);
		if (m_ConnectedParticle != nullptr) {
			m_ConnectedParticle->SetPosition(m_xmf3Position);
		}
		m_ElapsedTime += fTimeElapsed;
	}
}
void CMummyLaser::SetTargetPosition(const XMFLOAT3& targetPos)
{
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Subtract(targetPos, m_xmf3Position));

	m_xmf3TargetPosition = targetPos;
	m_xmf3Velocity = dirVector;
}

void CMummyLaser::SetTargetVector(const XMFLOAT3& MummyLookAt)
{
	XMFLOAT3 dirVector = Vector3::Normalize(MummyLookAt);
	XMFLOAT3 targetPos = Vector3::Multifly(dirVector, 150000);
	m_xmf3Velocity = dirVector;
	LookAt(m_xmf3Position, targetPos, XMFLOAT3(0, 1, 0));
}

void CMummyLaser::SetDrawable(bool drawable)
{
	m_isDrawable = drawable;
	if (drawable) {
		if (m_ConnectedParticle != nullptr) {
			m_ConnectedParticle->SetDrawable(false);
		}
	}
}

void CMummyLaser::AddFriends_p(CMummy* mummy)
{
	m_Friends.push_back(mummy);
}

void CMummyLaser::SetDirecionVector(const XMFLOAT3& direction)
{
	


	XMFLOAT3 normalVec = Vector3::Normalize(direction);
	m_xmf4x4World._21 = normalVec.x;
	m_xmf4x4World._22 = normalVec.y;
	m_xmf4x4World._23 = normalVec.z;
	
}
