#include "stdafx.h"
#include "Effect.h"
#include "Shader.h"
#include "Player.h"
#include "Terrain.h"

CEffect::CEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	CPlayer* targetPlayer, const XMFLOAT3& size)
{
	CTexturedRectMesh* pMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, size.x, size.y, size.z);
	SetMesh(pMesh);
	SetShader(CShaderHandler::GetInstance().GetData("Effect"));
	m_isDrawable = false;
	m_xmf3Size = { 1,1,1 };
	m_Targetplayer = targetPlayer;
	m_ElapsedTime = 0.0f;
	m_LifeTime = 0.0f; 
	m_EffectType = EffectTypes::None;
	
	m_HeightFromTerrain = 3.0f + size.y * 0.5f;
}

CEffect::~CEffect()
{
}

void CEffect::Update(float elapsedTime)
{
	if (false == m_isDrawable) {
		return;
	}
	XMFLOAT3 playerLook = m_Targetplayer->GetLook();
	XMFLOAT3 playerPos = m_Targetplayer->GetPosition();
	playerPos.y = m_xmf3Position.y;

	XMFLOAT3 targetVec = Vector3::Subtract(m_xmf3Position, playerPos);
	targetVec = Vector3::Normalize(targetVec);

	XMFLOAT3 toMovePosition = Vector3::Subtract(m_Targetplayer->GetPosition(),
		Vector3::Multifly(targetVec, 3000.0f));

	toMovePosition.y = m_xmf3Position.y;

	Vector3::Subtract(toMovePosition, m_xmf3Position);

	LookAt(m_xmf3Position, toMovePosition, { 0,1,0 });

	m_ElapsedTime += elapsedTime;
	if (m_ElapsedTime > m_LifeTime) {
		m_ElapsedTime = 0.0f;
		//m_isDrawable = false;
	}
}

void CEffect::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_isDrawable) return;
	OnPrepareRender();

	if (m_pShader)
	{ 
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) {
		m_pMesh->Render(pd3dCommandList);
	}
}

void CEffect::FixPositionByTerrain(CTerrain* pTerrain)
{
	m_xmf3Position.y = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z) + m_HeightFromTerrain;
	SetPosition(m_xmf3Position);
}

void CEffect::SetEffectType(EffectTypes effectType)
{
	switch (effectType)
	{
	case EffectTypes::None:
		break;
	case EffectTypes::BossAttacked:
		SetTextureIndex(0x01);
		m_LifeTime = 1.5f;
		break;
	case EffectTypes::Thunder:
		SetTextureIndex(0x02);
		m_LifeTime = 1.2f;
		break;
	case EffectTypes::WarnningCircle:
		SetTextureIndex(0x04);
		m_LifeTime = 1.2f;
		break;
	default:
		break;
	}
	m_EffectType = effectType;
}

void CEffectHandler::Update(float elapsedTime)
{
	for (auto& pEffect : m_Effects) {
		pEffect->Update(elapsedTime);
	}
}

void CEffectHandler::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto& pEffect : m_Effects) {
		pEffect->Draw(pd3dCommandList, pCamera);
	}
}

void CEffectHandler::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* targetPlayer)
{
	for (int i = 0; i < 10; ++i) {
		CEffect* pTempEffect = new CEffect(pd3dDevice, pd3dCommandList, targetPlayer, { 192.0f, 1500.0f, 0.0f });
		pTempEffect->SetEffectType(EffectTypes::Thunder);
		m_Effects.emplace_back(std::move(pTempEffect));
	}

	for (int i = 0; i < 10; ++i) {
		CEffect* pTempEffect = new CEffect(pd3dDevice, pd3dCommandList, targetPlayer, { 192.0f, 0.0f, 192.0f });
		pTempEffect->SetEffectType(EffectTypes::BossAttacked);
		m_Effects.emplace_back(std::move(pTempEffect));
	}

	for (int i = 0; i < 10; ++i) {
		CEffect* pTempEffect = new CEffect(pd3dDevice, pd3dCommandList, targetPlayer, { 300.0f, 0.0f, 300.0f });
		pTempEffect->SetEffectType(EffectTypes::WarnningCircle);
		m_Effects.emplace_back(std::move(pTempEffect));
	}
}

CEffect* CEffectHandler::RecycleEffect(EffectTypes effectType)
{
	for (auto& pEffect : m_Effects) {
		if (pEffect->IsDrawable() == false &&
			pEffect->GetEffectType() == effectType) {
			return pEffect;
		}
	}
	return nullptr;
}
