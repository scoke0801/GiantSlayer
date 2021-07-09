#include "stdafx.h"
#include "Boss.h"
#include "State.h"
#include "Effect.h"

#include "SceneJH.h"

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
	m_EnemyType = EnemyType::Boss;
	m_AttackRange = 1000.0f;
	m_Speed = 165.0f * 10.0f;
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

void CBoss::Attack(float elapsedTime)
{
	if (m_AttackDelayTime <= 0.0f) {
		// 실제 공격!  
		if (m_AttackType == EnemyAttackType::BossSkill_1) {
			m_AttackDelayTime = BOSS_ATTACK_1_ANIMATION_LENGTH + 0.3f;
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_2) {
			m_AttackDelayTime = BOSS_ATTACK_2_ANIMATION_LENGTH + 0.3f;
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_3) {
			m_AttackDelayTime = BOSS_ATTACK_3_ANIMATION_LENGTH + 0.5f;
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_4) { 
			m_AttackDelayTime = BOSS_ATTACK_4_ANIMATION_LENGTH + 0.7f;
			// 두 번 방방뛰는 애니메이션 -> 2번 사이클 돌면 좋을듯

			XMFLOAT3 targetPosition;
			//z : 21800.0 14533
			//z : 29700.0 19800
			//x : 29700.0 19800
			//x : 21800.0 14533
			int thunderCount = rand() % 5 + 5;
			auto workScene = MAIN_GAME_SCENE;
			float thunderRange = 3700.0f;
			for (int i = 0; i < thunderCount; ++i) {
				targetPosition.x = (((float)rand() / (RAND_MAX)) * (thunderRange * 2)) + m_xmf3ActivityScopeCenter.x - thunderRange;
				targetPosition.y = m_xmf3Position.y;
				targetPosition.z = (((float)rand() / (RAND_MAX)) * (thunderRange * 2)) + m_xmf3ActivityScopeCenter.z - thunderRange;

				workScene->UseEffects((int)EffectTypes::WarnningCircle, targetPosition);
				workScene->UseEffects((int)EffectTypes::Thunder, targetPosition, 1.5f);
			}
			cout << "공격 이펙트 생성 테스트\n";
		}
	}
}

void CBoss::CalcNextAttackType()
{
	m_AttackType = (EnemyAttackType)(rand() % 4 + (int)EnemyAttackType::BossSkill_1);
	if (m_AttackType == EnemyAttackType::BossSkill_1) {
		m_AttackRange = 1000.0f;
		m_AttackDelayTime = BOSS_ATTACK_1_ANIMATION_LENGTH + 1.0f;
	}
	else if (m_AttackType == EnemyAttackType::BossSkill_2) {
		m_AttackRange = 1000.0f;
		m_AttackDelayTime = BOSS_ATTACK_2_ANIMATION_LENGTH + 1.0f;
	}
	else if (m_AttackType == EnemyAttackType::BossSkill_3) {
		m_AttackRange = 3000.0f;
		m_AttackDelayTime = BOSS_ATTACK_3_ANIMATION_LENGTH + 1.0f;
	}
	else if (m_AttackType == EnemyAttackType::BossSkill_4) {
		m_AttackRange = 16000.0f;
		m_AttackDelayTime = BOSS_ATTACK_3_ANIMATION_LENGTH + 1.0f;
	}
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
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Idle:
		SetAnimationSet((int)BOSS_ANIMATION::Idle);
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Patrol:
		SetAnimationSet((int)BOSS_ANIMATION::Run);
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Trace:
		SetAnimationSet((int)BOSS_ANIMATION::Run);
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Attack: 
	{
		if (m_AttackType == EnemyAttackType::BossSkill_1) {
			SetAnimationSet((int)BOSS_ANIMATION::Skill_1);
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_2) {
			SetAnimationSet((int)BOSS_ANIMATION::Skill_2);
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_3) {
			SetAnimationSet((int)BOSS_ANIMATION::Skill_3);
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_4) {
			SetAnimationSet((int)BOSS_ANIMATION::Born_2);
		}
	}
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::Attacked:
		break;
	case ObjectState::Die: 
		SetAnimationSet((int)BOSS_ANIMATION::Dead);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::RunAway:
		break;
	case ObjectState::BossSkill_1: 
		SetAnimationSet((int)BOSS_ANIMATION::Skill_1);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_2:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_2);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_3:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_3);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_4:
		SetAnimationSet((int)BOSS_ANIMATION::Born_2);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_5:
		break;
	case ObjectState::BossBorn: 
		SetAnimationSet((int)BOSS_ANIMATION::Born_1);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	default:
		break;
	}
}
