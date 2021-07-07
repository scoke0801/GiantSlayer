#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "Terrain.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	:CGameObjectVer2()
{
	m_Type = OBJ_TYPE::Player;

	m_HP = 100;
	m_SP = 100;

	m_SpareBoundingBox = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, PulledModel::Center, 0.4, 1.2, 1.4, XMFLOAT3{ 0,0.6, 0.2f });
	m_SpareCollisionBox = new ColliderBox(ColliderBox(XMFLOAT3(0, 0.6, 0.2f), XMFLOAT3(0.2, 0.6, 1.4)));
	m_SpareAABB = new ColliderBox(ColliderBox(XMFLOAT3(0, 0.6, 0.2f), XMFLOAT3(0.2, 0.6, 1.4))); 
}

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	m_Type = OBJ_TYPE::Player;

	m_HP = 100;
	m_SP = 100;
}

CPlayer::~CPlayer()
{

}

void CPlayer::Update(float fTimeElapsed)
{
	//cout << m_pChild->m_pAnimationController->m_pAnimationTracks[m_pChild->m_pAnimationController->m_nAnimationTrack].m_pAnimationSet->m_fPosition << endl;

	if (false == m_IsCanAttack) {
		if (GetWeapon() == PlayerWeaponType::Bow) {
			if (pullString && m_AttackWaitingTime < 0.6f) {
				pause = true;
			}
		}

		if (!pause) {
			m_AttackWaitingTime -= fTimeElapsed;
		}

		//m_pAnimationController->repeat = false;

		SetAnimationSet(ATK);

		if (m_AttackWaitingTime < 0.0f){
			m_IsAlreadyAttack = false;
			m_AttackWaitingTime = 0.0f;
			m_IsCanAttack = true;

			auto temp = m_SpareCollisionBox;
			m_SpareCollisionBox = m_Colliders[0];
			m_Colliders[0] = temp;
			
			temp = m_SpareAABB;
			m_SpareAABB = m_AABB[0];
			m_AABB[0] = temp;
			
			auto tempMesh = m_SpareBoundingBox;
			m_SpareBoundingBox = m_BoundingObjectMeshes[0];
			m_BoundingObjectMeshes[0] = tempMesh;

			UpdateColliders(); 

			if (GetWeapon() == PlayerWeaponType::Bow) {
				pause = false;
				pausedTime = 0;
				//m_pChild->m_pAnimationController->m_pAnimationTracks[m_pChild->m_pAnimationController->m_nAnimationTrack].m_pAnimationSet->m_fPosition = 0;
				//m_pAnimationController->repeat = true;
			}
		}
	}
	// 피격
	else if (m_AttackedDelay > 0.0f) {
		m_AttackedDelay = max(m_AttackedDelay - fTimeElapsed, 0.0f);
		/*if (m_AttackedDelay == 0.0f) {
			SetAnimationSet(AnimationType::IDLE);
		}*/
	}
	else {
		//m_pAnimationController->repeat = true;

		if (m_xmf3Velocity.x == 0 && m_xmf3Velocity.z == 0)
			SetAnimationSet(IDLE);
		else
			SetAnimationSet(RUN);
	}

	
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
			//m_Camera->UpdateAimMode(m_xmf3Position);
			//m_Camera->LookAt(m_Camera->GetPosition3f(), Vector3::Add(m_xmf3Position, {0, 0, 3}), GetUp());
			m_Camera->UpdateViewMatrix();
		}
		else
		{
			m_Camera->Update(m_xmf3Position);
			m_Camera->LookAt(m_Camera->GetPosition3f(), m_xmf3Position, GetUp());
			m_Camera->UpdateViewMatrix();
		}
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
	m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	m_AttackedDelay += 0.6666667f;
	m_HP -= 5;
	SetAnimationSet(AnimationType::DAMAGED);
	if (m_HP <= 5) {
		m_HP = 0;
	}
	return true;
}

void CPlayer::Attack()
{
	SetCanAttack(false);

	IncreaseAttackWaitingTime(m_AttackAnimLength);

	SetVelocityToZero();

	auto temp = m_Colliders[0];
	m_Colliders[0] = m_SpareCollisionBox;
	m_SpareCollisionBox = temp;

	temp = m_AABB[0];
	m_AABB[0] = m_SpareAABB;
	m_SpareAABB = temp;

	auto tempMesh = m_BoundingObjectMeshes[0];
	m_BoundingObjectMeshes[0] = m_SpareBoundingBox;
	m_SpareBoundingBox = tempMesh;
	UpdateColliders();
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
		m_AttackAnimPauseTime = 1.0f;
	}
}
 
void CPlayer::DisableSword()
{
	SetDrawableRecursively("sword1", false);
	SetDrawableRecursively("bow_LeftHand", true);
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", true);
}

void CPlayer::DisableBow()
{
	SetDrawableRecursively("sword1", true);
	SetDrawableRecursively("bow_LeftHand", false);
	SetDrawableRecursively("bow_arrow_RightHandMiddle1", false);
}

void CPlayer::PullString()
{
	/*
	atk disable
	set stop time

	*/
	SetCanAttack(false);

	IncreaseAttackWaitingTime(m_AttackAnimLength);

	SetVelocityToZero();
}

void CPlayer::ReleaseString()
{
	//SetCanAttack(true);
	//
	//
}