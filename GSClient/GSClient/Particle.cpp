#include "stdafx.h"
#include "Particle.h"

ParticleObject::ParticleObject() :CGameObject()
{
}

ParticleObject::~ParticleObject()
{
}

void ParticleObject::Update(float fTimeElapsed)
{
	m_elapsedTime += fTimeElapsed;
	if (m_Type == PARTICLE_TYPE::ArrowParticle) {
		if (m_elapsedTime > ARROW_PARTICLE_LIFE_TIME) {
			m_IsCanUse = false; 
			m_elapsedTime = 0.0f;
		}
	}
	else if (m_Type == PARTICLE_TYPE::HitParticleTex) {
		if (m_elapsedTime > HIT_PARTICLE_LIFE_TIME) {
			m_IsCanUse = false;
			m_elapsedTime = 0.0f;
		}
	}
}

void ParticleObject::SetSpeedVector(const XMFLOAT3& speed)
{
	m_xmf4x4World._11 = speed.x;
	m_xmf4x4World._12 = speed.y;
	m_xmf4x4World._13 = speed.z;
}

void ParticleObject::SetDirecionVector(const XMFLOAT3& direction)
{
	XMFLOAT3 normalVec = Vector3::Normalize(direction);
	m_xmf4x4World._21 = normalVec.x;
	m_xmf4x4World._22 = normalVec.y;
	m_xmf4x4World._23 = normalVec.z;
}

void ParticleObject::SetParticleParameter(int idx, float parameter)
{
	if (idx == 3) { // 
		m_xmf4x4World._14 = parameter;
	}
	else if (idx == 4) {
		m_xmf4x4World._24 = parameter;
	}
	else if (idx == 5) {
		m_xmf4x4World._34 = parameter;
	} 
} 

CParticle::CParticle()
{
	
}

CParticle::~CParticle()
{ 
}

void CParticle::AddParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	int count, PARTICLE_TYPE type)
{
	if (type == PARTICLE_TYPE::ArrowParticle) {
		CArrowParticleMesh* pMesh = new CArrowParticleMesh(pd3dDevice, pd3dCommandList, count);

		ParticleObject* pObject = new ParticleObject();
		pObject->SetShader(CShaderHandler::GetInstance().GetData("ArrowParticle"));
		pObject->SetMesh(pMesh);
		pObject->SetType(type);
		m_ParticleObjs.push_back(std::move(pObject));
	}
	else if (type == PARTICLE_TYPE::HitParticleTex) {
		CTexParticleMesh* pMesh = new CTexParticleMesh(pd3dDevice, pd3dCommandList, count);
		
		ParticleObject* pObject = new ParticleObject();
		pObject->SetShader(CShaderHandler::GetInstance().GetData("TexParticle"));
		pObject->SetMesh(pMesh);
		pObject->SetType(type);
		m_ParticleObjs.push_back(std::move(pObject));
	}
}

void CParticle::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto particle : m_ParticleObjs) {
		if (particle->IsCanUse()) {
			particle->Draw(pd3dCommandList, pCamera);
		}
	}
}

void CParticle::Update(float elapsedTime)
{
	for (auto particle : m_ParticleObjs) {
		particle->Update(elapsedTime);
	}
}

int CParticle::GetCanUseableParticle(PARTICLE_TYPE type)
{
	for (int i = 0; i < m_ParticleObjs.size(); ++i) {
		if (true == m_ParticleObjs[i]->IsCanUse()) {
			continue;
		}
		if (type == m_ParticleObjs[i]->GetParticleType()) {
			return i;
		}
	}
	return -1;
}

void CParticle::SetPosition(int idx, const XMFLOAT3& pos)
{
	m_ParticleObjs[idx]->SetPosition(pos);
}

void CParticle::SetDirection(int idx, const XMFLOAT3& dir)
{
	m_ParticleObjs[idx]->SetDirecionVector(dir);
}

void CParticle::UseParticle(int idx, const XMFLOAT3& pos, const XMFLOAT3& dir)
{
	m_ParticleObjs[idx]->SetPosition(pos); 
	m_ParticleObjs[idx]->SetDirecionVector(dir); 
	m_ParticleObjs[idx]->SetUseable(true);
}

void CParticle::UseParticle(int idx)
{
	m_ParticleObjs[idx]->SetUseable(true);
}