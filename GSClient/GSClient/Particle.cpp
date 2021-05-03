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
}

void ParticleObject::SetSpeedVector(const XMFLOAT3& speed)
{
	m_xmf4x4World._11 = speed.x;
	m_xmf4x4World._12 = speed.y;
	m_xmf4x4World._13 = speed.z;
}

void ParticleObject::SetDirecionVector(const XMFLOAT3& direction)
{
	m_xmf4x4World._21 = direction.x;
	m_xmf4x4World._22 = direction.y;
	m_xmf4x4World._23 = direction.z;
}

void ParticleObject::SetParticleParameter(int idx, float parameter)
{
	if (idx == 0) {      // randomParameter
		m_xmf4x4World._31 = parameter;
	}
	else if (idx == 1) { // emitTime
		m_xmf4x4World._32 = parameter;
	}
	else if (idx == 2) { // lifeTime
		m_xmf4x4World._33 = parameter;
	}
	else if (idx == 3) { // 
		m_xmf4x4World._14 = parameter;
	}
	else if (idx == 4) {
		m_xmf4x4World._24 = parameter;
	}
	else if (idx == 5) {
		m_xmf4x4World._34 = parameter;
	} 
} 

CParticle::CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count)
{
	CParticleMesh* pMesh = new CParticleMesh(pd3dDevice, pd3dCommandList, 1000);

	ParticleObject* pObject = new ParticleObject();
	pObject->SetShader(CShaderHandler::GetInstance().GetData("Particle"));
	//pObject->SetTextureIndex(0x01);
	pObject->SetMesh(pMesh);
	pObject->SetParticleParameter(0, GetRandomValue(1, 0));
	m_ParticleObjs.push_back(std::move(pObject));
}

CParticle::~CParticle()
{
}

void CParticle::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto particle : m_ParticleObjs) {
		particle->Draw(pd3dCommandList, pCamera);
	}
}

void CParticle::Update(float elapsedTime)
{
	for (auto particle : m_ParticleObjs) {
		particle->Update(elapsedTime);
	}
}
