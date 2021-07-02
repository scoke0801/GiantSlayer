#include "stdafx.h"
#include "Boss.h"
#include "State.h"
CBoss::CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature)
{
	CGameObjectVer2* pBossParent = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		pd3dRootSignature, "resources/FbxExported/Boss.bin", NULL, true);

	SetChild(pBossParent, true);
	SetAnimationSet((int)BOSS_ANIMATION::Idle);
	SetShadertoAll();

	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 11, 10, 7, XMFLOAT3{ 0,0,0 });
	AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5.5, 5, 3.5)));
	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 4.5f, 5, 6, XMFLOAT3{ 2.5, 3, 7 });
	AddColider(new ColliderBox(XMFLOAT3(2.5, 5.5, 7), XMFLOAT3(2.25, 2.5, 3)));
	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 4.5f, 5, 6, XMFLOAT3{ -2.5, 3, 7 });
	AddColider(new ColliderBox(XMFLOAT3(-2.5, 5.5, 7), XMFLOAT3(2.25, 2.5, 3)));
	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 3, 3, 5, XMFLOAT3{ 0,3,-7 });
	AddColider(new ColliderBox(XMFLOAT3(0, 4.5, -7), XMFLOAT3(1.5, 1.5, 2.5)));

	m_State = new WaitState(this);
}

CBoss::~CBoss()
{
}

void CBoss::Update(float elapsedTime)
{
	m_State->Execute(this, elapsedTime);

	m_SightBox.Transform(m_SightAABB, XMLoadFloat4x4(&m_xmf4x4ToParent));

	if (m_AttackDelayTime > 0.0f) {
		m_AttackDelayTime = max(m_AttackDelayTime - elapsedTime, 0.0f);
	}

	CGameObjectVer2::Animate(elapsedTime);
	UpdateTransform(NULL);
}

void CBoss::UpdateOnServer(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}
 
void CBoss::PlayerEnter(CPlayer* target)
{
	if (m_isOnAwaken == false) {
		SetAnimationSet((int)BOSS_ANIMATION::Born_1);
		m_isOnAwaken = true;
	}
}

void CBoss::ChangeAnimation(ObjectState stateInfo)
{
	switch (stateInfo)
	{
	case ObjectState::Wait:
		SetAnimationSet((int)BOSS_ANIMATION::Idle);
		break;
	case ObjectState::Idle:
		SetAnimationSet((int)BOSS_ANIMATION::Idle);
		break;
	case ObjectState::Patrol:
		SetAnimationSet((int)BOSS_ANIMATION::Run);
		break;
	case ObjectState::Trace:
		SetAnimationSet((int)BOSS_ANIMATION::Run);
		break;
	case ObjectState::Attack: 
		SetAnimationSet((int)BOSS_ANIMATION::Skill_1);
		break;
	case ObjectState::Attacked:
		break;
	case ObjectState::Die: 
		SetAnimationSet((int)BOSS_ANIMATION::Dead);
		break;
	case ObjectState::RunAway:
		break;
	case ObjectState::BossSkill_1: 
		SetAnimationSet((int)BOSS_ANIMATION::Skill_1);
		break;
	case ObjectState::BossSkill_2:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_2);
		break;
	case ObjectState::BossSkill_3:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_3);
		break;
	case ObjectState::BossSkill_4:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_4);
		break;
	case ObjectState::BossSkill_5:
		break;
	case ObjectState::BossBorn: 
		SetAnimationSet((int)BOSS_ANIMATION::Born_1);
		break;
	default:
		break;
	}
}
