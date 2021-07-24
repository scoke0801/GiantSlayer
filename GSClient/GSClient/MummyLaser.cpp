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
		//���� ��ü�� ���� ��ȯ ����� ���̴��� ��� ���۷� ����(����)�Ѵ�.
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
		Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), 360.0f * fTimeElapsed);
		if (m_ConnectedParticle != nullptr) {
			m_ConnectedParticle->SetPosition(m_xmf3Position);
		}
		m_ElapsedTime += fTimeElapsed;

		
		if (m_ElapsedTime > 3.0f ) {
			for (auto& mummy : m_Friends)
			{
				if (GetLaserType() == Laser_TYPE::Laser1 && m_ElapsedTime > 3.0f+5.f)
				{
		
					XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ mummy->GetPosition() }, { 0,200,0 });
					m_ConnectedParticle->SetPosition(pos);
					m_ConnectedParticle->SetDrawable(true);
					SetPosition(pos);
					m_ElapsedTime = 0.0f;
					
				}
				if (GetLaserType() == Laser_TYPE::Laser2 && m_ElapsedTime > 3.0f + 3.0f)
				{
					
					XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ mummy->GetPosition() }, { 0,200,0 });
					m_ConnectedParticle->SetPosition(pos);
					m_ConnectedParticle->SetDrawable(true);
					SetPosition(pos);
					m_ElapsedTime = 0.0f;
					
				}
				if (GetLaserType() == Laser_TYPE::Laser3 && m_ElapsedTime > 3.0f + 7.0f)
				{
					
					XMFLOAT3 pos = Vector3::Add(XMFLOAT3{ mummy->GetPosition() }, { 0,200,0 });
					m_ConnectedParticle->SetPosition(pos);
					m_ConnectedParticle->SetDrawable(true);
					SetPosition(pos);
					m_ElapsedTime = 0.0f;
					
				}
			}
			
			m_ConnectedParticle->SetDrawable(false);
			
			
			
			//m_ConnectedParticle = nullptr;
		}
	}
	return;
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
