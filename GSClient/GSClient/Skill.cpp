#include "stdafx.h"
#include "Skill.h"
#include "Shader.h"
#include "Particle.h"
#include "Effect.h"

#include "GameScene.h"

CFireBall::CFireBall()
{
	m_isDrawable = true;
	m_ATK = 200;
}

CFireBall::~CFireBall()
{

}

void CFireBall::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (true == m_isDrawable) return;
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

void CFireBall::Update(float fTimeElapsed)
{
	m_ExplosionBox.Transform(m_ExplosionAABB, XMLoadFloat4x4(&m_xmf4x4ToParent));

	if (false == m_isDrawable) {
		m_ElapsedTime += fTimeElapsed;

		if (m_ElapsedTime > FIREBALL_LIFE_TIME) {
			m_ElapsedTime = 0.0f;
			m_isDrawable = true;

			auto curScene = MAIN_GAME_SCENE;
			curScene->UseEffects((int)EffectTypes::FireBallExplosion, m_xmf3Position);
		}

		SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, FIREBALL_SPEED * fTimeElapsed)));
		Rotate(XMFLOAT3(0.3f, 1.0f, 0.0f), 360.0f * fTimeElapsed);
	}
}

void CFireBall::UpdateOnServer(float fTimeElapsed)
{
	Rotate(XMFLOAT3(0.3f, 1.0f, 0.0f), 360.0f * fTimeElapsed);
}

void CFireBall::SetSkill(CGameObject* owner)
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

void CFireBall::SetTargetPosition(const XMFLOAT3& targetPos)
{
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Subtract(targetPos, m_xmf3Position));

	m_xmf3TargetPosition = targetPos;
	m_xmf3Velocity = dirVector;
}

void CFireBall::TrackingTarget(CGameObject target)
{

}

void CFireBall::SetDrawable(bool drawable)
{
	m_isDrawable = drawable;
	if (drawable) {
		if (m_ConnectedParticle != nullptr) {
			m_ConnectedParticle->SetDrawable(false);
		}
	}
}

void CFireBall::SetExplosionBoundingBox(const XMFLOAT3& size)
{
	m_ExplosionBox = BoundingBox({ 0,0,0 }, size);

	m_ExplosionAABB = BoundingBox({ 0,0,0 }, size);
}