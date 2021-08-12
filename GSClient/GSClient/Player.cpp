#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "Terrain.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	:CGameObjectVer2()
{
	m_Type = OBJ_TYPE::Player;

	// Status
	m_HP = 100;
	m_SP = 100;
	m_ATK = 30;
	m_DEF = 0;

	FindWeapon("sword1", this);
	//swordCollider = m_pWeapon->GetColliders();
}

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	m_Type = OBJ_TYPE::Player;

	// Status
	m_HP = 100;
	m_SP = 100;
	m_ATK = 100;
	m_DEF = 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update(float fTimeElapsed)
{
	if (m_HP <= 0 && m_AttackedDelay <= 0.1f) {
		return;
	}

	if (m_IsCanAttack == false) {
		if (!m_AnimationPaused)
			m_AttackWaitingTime -= fTimeElapsed;

		switch (m_WeaponType)
		{
		case PlayerWeaponType::Sword: {
		}break;

		case PlayerWeaponType::Bow: {
			if (pullString) {
				if (m_AttackWaitingTime < 1.2f)
					SetDrawableRecursively("bow_arrow_RightHandMiddle1", true);

				if (m_AttackWaitingTime < m_AttackAnimPauseTime)
					m_AnimationPaused = true;

				m_StringPullTime += fTimeElapsed;
				m_SP -= fTimeElapsed;
			}
		}break;

		case PlayerWeaponType::Staff: {

		}break;
		}

		if (m_AttackWaitingTime < 0.0f) {
			ResetAttack();
			m_ComboTimer = 0.5f;
			if (m_AttackKeyDown) {
				SetSwordAttackKeyDown(false);
				Attack(0);
			}
			else
				m_LastAttackAnim = 0;
		}
	}
	else if (m_AttackedDelay > 0.0f) {
		m_AttackedDelay = max(m_AttackedDelay - fTimeElapsed, 0.0f);
		/*if (m_AttackedDelay == 0.0f) {
			SetAnimationSet(AnimationType::IDLE);
		}*/
	}
	else {
		if (m_xmf3Velocity.x == 0 && m_xmf3Velocity.z == 0) {
			if (m_ComboTimer > 0)
				m_ComboTimer -= fTimeElapsed;
			SetAnimationSet(IDLE);
		}
		else if (m_PullBox == TRUE)
		{
			SetAnimationSet(PUSH);
		}
		else
			SetAnimationSet(RUN);
	}

	if (m_Alive) {
		float Friction = (m_MovingType == PlayerMoveType::Run) ? PLAYER_RUN_SPEED : PLAYER_WALK_SPEED;

		XMFLOAT3 vel = Vector3::Multifly(m_xmf3Velocity, fTimeElapsed);

		Move(vel);

		if (false == m_isOnGround) {
			float y;
			if (m_JumpTime > 0.5f) {
				y = -PLAYER_JUMP_HEIGHT * fTimeElapsed;
			}
			else {
				y = PLAYER_JUMP_HEIGHT * fTimeElapsed;
			}
			Move({ 0,y,0 });
			m_JumpTime += fTimeElapsed;
			if (m_JumpTime > TO_JUMP_TIME) {
				m_JumpTime = 0.0f;
				m_isOnGround = true;
			}
		}
		UpdateCamera();

		float fLength = Vector3::Length(m_xmf3Velocity);
		float fDeceleration = (Friction * fTimeElapsed);
		if (fDeceleration > fLength) fDeceleration = fLength;

		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
		m_xmf3Velocity.x = m_xmf3Velocity.y = m_xmf3Velocity.z = 0.0f;
	}

	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CPlayer::UpdateOnServer(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CPlayer::UpdateCamera()
{
	if (m_Camera != nullptr) {
		if (pullString)
		{
			m_Camera->UpdateAimMode(m_xmf3Position);
			auto lookVec = GetLook();
			m_Camera->LookAt(m_Camera->GetPosition3f(), Vector3::Multifly(lookVec, 15000.0f), GetUp());
			m_Camera->Strafe(-20);
		}
		else
		{
			m_Camera->Update(m_xmf3Position);
			m_Camera->LookAt(m_Camera->GetPosition3f(), m_xmf3Position, GetUp());
		}

		m_Camera->UpdateViewMatrix();
	}
}

void CPlayer::FixCameraByTerrain(CTerrain* pTerrain)
{
	if (m_Camera == nullptr) {
		return;
	}
	XMFLOAT3 xmf3CameraPosition = m_Camera->GetPosition3f();

	/*
	높이 맵에서 카메라의 현재 위치 (x, z)에 대한 지형의 높이(y 값)를 구한다.
	이 값이 카메라의 위치 벡터의 y-값 보다 크면 카메라가 지형의 아래에 있게 된다.
	이렇게 되면 다음 그림의 왼쪽과 같이
	지형이 그려지지 않는 경우가 발생한다(카메라가 지형 안에 있으므로 삼각형의 와인딩 순서가 바뀐다).
	이러한 경우가 발생하지 않도록 카메라의 위치 벡터의 y-값의 최소값은 (지형의 높이 + 5)로 설정한다.
	카메라의 위치 벡터의 y-값의 최소값은 지형의 모든 위치에서
	카메라가 지형 아래에 위치하지 않도록 설정해야 한다.*/

	float offsetHeight = m_Camera->GetOffset().y;
	//float fHeight = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z) + 5.0f;	
	float fHeight = pTerrain->GetDetailHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;

	if (fHeight - 1500.0f > m_xmf3Position.y) {
		fHeight = m_xmf3Position.y + 1500.0f;
	}
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_Camera->SetPosition(xmf3CameraPosition);
		m_Camera->LookAt(m_xmf3Position, GetUp());
	}
}

void CPlayer::FixPositionByTerrain(CTerrain* pTerrain)
{
	if (m_isOnGround) {
		m_xmf3Position = GetPosition();
		m_xmf3Position.y = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z) + m_HeightFromTerrain;
		m_xmf4x4ToParent._41 = m_xmf3Position.x;
		m_xmf4x4ToParent._42 = m_xmf3Position.y;
		m_xmf4x4ToParent._43 = m_xmf3Position.z;
		//SetPosition(m_xmf3Position);
		//CGameObjectVer2::SetPosition(m_xmf3Position);
		//m_xmf3Position.y = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z); 
	}
}

int CPlayer::GetPlayerExistingSector() const
{
	if (m_xmf3Position.x < 14379 && m_xmf3Position.z < 22824) {
		return 0;
	}
	if (m_xmf3Position.x < 14379 && m_xmf3Position.z >= 22824) {
		return 1;
	}
	if (m_xmf3Position.x >= 14379 && m_xmf3Position.x < 20422 &&
		(m_xmf3Position.z > 10764)) {
		return 2;
	}
	if ((m_xmf3Position.x >= 14379 && m_xmf3Position.x < 20422) &&
		m_xmf3Position.z <= 10764) {
		return 3;
	}

	if (m_xmf3Position.x >= 20422 && m_xmf3Position.z < 12077) {
		return 3;
	}
	if (m_xmf3Position.x >= 20422 && m_xmf3Position.z >= 12077) {
		return 4;
	}
	return -1;
}

void CPlayer::SetVelocity(XMFLOAT3 dir)
{
	if (false == IsCanAttack()) {
		return;
	}
	dir.y = 0;
	XMFLOAT3 normalizedDir = Vector3::Normalize(dir);
	XMFLOAT3 targetPosition = Vector3::Multifly(normalizedDir, 150000.0f);
	LookAt(m_xmf3Position, targetPosition, XMFLOAT3{ 0,1,0 });
	//Rotate({ 0,1,0 }, 180.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, m_xmf3Velocity.y, 0.0f);
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity,
		Vector3::Multifly(normalizedDir, PLAYER_RUN_SPEED));

	float speed = m_MovingType == (PlayerMoveType::Run) ? PLAYER_RUN_SPEED : PLAYER_WALK_SPEED;

	if (m_xmf3Velocity.x > speed) m_xmf3Velocity.x = speed;
	if (m_xmf3Velocity.y > speed) m_xmf3Velocity.y = speed;
	if (m_xmf3Velocity.z > speed) m_xmf3Velocity.z = speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
}

void CPlayer::SetWeapon(PlayerWeaponType weaponType)
{
	m_WeaponType = weaponType;

	if (weaponType == PlayerWeaponType::Sword) {
		IDLE = AnimationType::SWORD_IDLE;
		RUN = AnimationType::SWORD_RUN;
		ATK = AnimationType::SWORD_ATK;
		ATK2 = AnimationType::SWORD_ATK2;
		ATK3 = AnimationType::SWORD_ATK3;
		SKILL = AnimationType::SWORD_SKILL;
		DEATH = AnimationType::SWORD_DEATH;

		m_AttackAnimLength = 2.0f;
		m_SwordAnim2Length = 1.133333f;
		m_SwordAnim3Length = 1.466667f;
		m_SkillAnimLength = 4.8f;
		m_DeathAnimLength = 3.7f;

		SetDrawSword();
	}
	else if (weaponType == PlayerWeaponType::Bow) {
		IDLE = AnimationType::BOW_IDLE;
		RUN = AnimationType::BOW_RUN;
		ATK = AnimationType::BOW_ATK;
		SKILL = AnimationType::BOW_SKILL;
		DEATH = AnimationType::BOW_DEATH;

		m_AttackAnimLength = 1.533333f;
		m_SkillAnimLength = 5.0f;
		m_AttackAnimPauseTime = 0.6f;
		m_DeathAnimLength = 3.866667f;

		SetDrawBow();
	}
	else if (weaponType == PlayerWeaponType::Staff) {
		IDLE = AnimationType::STAFF_IDLE;
		RUN = AnimationType::STAFF_RUN;
		ATK = AnimationType::STAFF_ATK;
		SKILL = AnimationType::STAFF_SKILL;
		DEATH = AnimationType::STAFF_DEATH;

		m_AttackAnimLength = 1.433333f;
		m_SkillAnimLength = 1.133333f;
		m_DeathAnimLength = 3.7f;

		SetDrawStaff();
	}
}

void CPlayer::SetDrawSword()
{
	SetDrawableRecursively("sword1", true);
	SetDrawableRecursively("bow_LeftHand", false);
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", false);
	SetDrawableRecursively("twoHandedStaff", false);
}

void CPlayer::SetDrawBow()
{
	SetDrawableRecursively("sword1", false);
	SetDrawableRecursively("bow_LeftHand", true);
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", false);
	SetDrawableRecursively("twoHandedStaff", false);
}

void CPlayer::SetDrawStaff()
{
	SetDrawableRecursively("sword1", false);
	SetDrawableRecursively("bow_LeftHand", false);
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", false);
	SetDrawableRecursively("twoHandedStaff", true);
}

void CPlayer::Jump()
{
	if (false == m_isOnGround) {
		return;
	}
	// 일반 사람의 점프 높이는 대략 30 ~ 40cm 
	m_isOnGround = false;
	m_JumpTime = 0.0f;
}

bool CPlayer::Attacked(CGameObject* pObject)
{
	if (m_AttackedDelay > 0.0f) {
		return false;
	}
	m_IsCanAttack = false;
	m_xmf3Velocity = XMFLOAT3(0, 0, 0);

	if (pObject == nullptr)
		TakeDamage(1000);
	else
		TakeDamage(pObject->GetATK());

	if (m_HP > 0) {
		m_AttackedDelay += 0.6666667f;
		SetAnimationSet(AnimationType::DAMAGED);
	}
	else {
		m_Alive = false;
		m_HP = 0;
		m_SP = 0;
		m_AttackedDelay += m_DeathAnimLength;
		SetAnimationType(ANIMATION_TYPE_ONCE);
		SetAnimationSet(DEATH);
	}

	return true;
}

void CPlayer::Attack(int type)
{
	SetCanAttack(false);

	SetVelocityToZero();

	if (type == 0) {
		if (m_WeaponType == PlayerWeaponType::Sword && m_ComboTimer > 0.0f) {
			if (m_LastAttackAnim == 0) {
				IncreaseAttackWaitingTime(m_SwordAnim2Length);
				SetAnimationSet(ATK2);
				m_LastAttackAnim = 1;
			}
			else if (m_LastAttackAnim == 1) {
				IncreaseAttackWaitingTime(m_SwordAnim3Length);
				SetAnimationSet(ATK3);
				m_LastAttackAnim = 2;
			}
			else if (m_LastAttackAnim == 2) {
				IncreaseAttackWaitingTime(m_AttackAnimLength);
				SetAnimationSet(ATK);
				m_LastAttackAnim = 0;
			}
		}
		else {
			IncreaseAttackWaitingTime(m_AttackAnimLength);
			SetAnimationSet(ATK);
			m_LastAttackAnim = 0;
		}
	}
	else if (type == 1) {
		IncreaseAttackWaitingTime(m_SkillAnimLength);
		SetAnimationSet(SKILL);
	}
}

void CPlayer::ResetAttack()
{
	m_IsAlreadyAttack = false;
	m_AttackWaitingTime = 0.0f;
	m_IsCanAttack = true;
}

void CPlayer::ResetBow()
{
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", false);
	pullString = false;
	m_AnimationPaused = false;
	m_StringPullTime = 0;
}

void CPlayer::Box_Pull(bool Pull_State)
{
	SetPullBox(Pull_State);
}

void CPlayer::AnimationChange(PlayerWeaponType weapon)
{
	if (weapon == PlayerWeaponType::Sword) {
		IDLE = AnimationType::SWORD_IDLE;
		RUN = AnimationType::SWORD_RUN;
		ATK = AnimationType::SWORD_ATK;
		DEATH = AnimationType::SWORD_DEATH;

		m_AttackAnimLength = 1.033333f;
	}
	else if (weapon == PlayerWeaponType::Bow) {
		IDLE = AnimationType::BOW_IDLE;
		RUN = AnimationType::BOW_RUN;
		ATK = AnimationType::BOW_ATK;
		DEATH = AnimationType::BOW_DEATH;

		m_AttackAnimLength = 1.533333f;
		m_AttackAnimPauseTime = 0.6f;
	}
}

void CPlayer::DisableSword()
{
	SetDrawableRecursively("sword1", false);
	SetDrawableRecursively("bow_LeftHand", true);
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", false);
}

void CPlayer::DisableBow()
{
	SetDrawableRecursively("sword1", true);
	SetDrawableRecursively("bow_LeftHand", false);
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", false);
}

bool CPlayer::ShotAble()
{
	if (pullString && m_AttackWaitingTime < m_AttackAnimPauseTime)
		return true;

	return false;
}