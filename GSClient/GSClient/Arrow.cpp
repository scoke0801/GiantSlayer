#include "stdafx.h"
#include "Arrow.h"
#include "Shader.h"
#include "Particle.h"

void CArrow::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (true == m_isDrawbale) return;
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

void CArrow::Update(float fTimeElapsed)
{
	if (false == m_isDrawbale) {
		SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, ARROW_SPEED * fTimeElapsed)));

		Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), 360.0f * fTimeElapsed);
		if (m_ConnectedParticle != nullptr) {
			m_ConnectedParticle->SetPosition(m_xmf3Position);
		}
		m_ElapsedTime += fTimeElapsed;
		if (m_ElapsedTime > ARROW_LIFE_TIME) {
			m_ElapsedTime = 0.0f;
			m_isDrawbale = true;
			//m_ConnectedParticle = nullptr;
		}

		if (m_xmf3Position.y >= m_startPos.y + 50) {
			falldown = true;
		}

		if (!falldown) {
			m_xmf3Position.y += 3.0f;
		}
		else {

		}

		m_xmf3Position = Vector3::Add(m_xmf3Position, { 0.0f, -1.0f, 0.0f });
		m_xmf3Velocity.z += AIR_RESISTANCE;
		//Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -90.0f * fTimeElapsed);
	}
	else return; 
}

void CArrow::SetTargetPosition(const XMFLOAT3& targetPos)
{	
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Subtract(targetPos, m_xmf3Position));

	m_xmf3TargetPosition = targetPos;
	m_xmf3Velocity = dirVector; 
}

void CArrow::SetTargetVector(const XMFLOAT3& playerLookAt)
{
	XMFLOAT3 dirVector = Vector3::Normalize(playerLookAt);
	XMFLOAT3 targetPos = Vector3::Multifly(dirVector, 150000);
	m_xmf3Velocity = dirVector;
	LookAt(m_xmf3Position, targetPos, XMFLOAT3(0, 1, 0));
}
void CArrow::SetDrawable(bool drawable)
{  
	m_isDrawbale = drawable; 
	if (drawable) {
		if (m_ConnectedParticle != nullptr) {
			m_ConnectedParticle->SetDrawable(false);
		}
	}
}
