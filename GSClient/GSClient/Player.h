#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "stdafx.h"

#include "GameObjectVer2.h"

enum class PlayerMoveType
{
	None = 0,
	Walk,
	Run
};

enum class PlayerWeaponType
{ 
	None	= 0x00,
	Sword	= 0x01,
	Bow		= 0x02,
	Staff	= 0x04 
};

class CTerrain;

class CPlayer : public CGameObjectVer2
{
public:
	AnimationType IDLE = AnimationType::SWORD_IDLE;
	AnimationType RUN = AnimationType::SWORD_RUN;
	AnimationType ATK = AnimationType::SWORD_ATK;
	AnimationType ATK2 = AnimationType::SWORD_ATK2;
	AnimationType ATK3 = AnimationType::SWORD_ATK3;
	AnimationType SKILL = AnimationType::SWORD_SKILL;
	AnimationType DEATH = AnimationType::SWORD_DEATH;
	AnimationType CHANGEWEAPON = AnimationType::BOW_GET;
	AnimationType PUSH = AnimationType::BOX_PUSH;

private:
	bool m_IsAlreadyAttack = false;

private:
	PlayerMoveType m_MovingType = PlayerMoveType::Run;
	PlayerWeaponType m_WeaponType = PlayerWeaponType::Sword;

	bool m_isOnGround = true;
	float m_JumpTime = 0.0f;

private:
	float m_AttackAnimLength = 0.0f;
	float m_SwordAnim2Length = 0.0f;
	float m_SwordAnim3Length = 0.0f;
	float m_SkillAnimLength = 0.0f;
	float m_DeathAnimLength = 0.0f;
	float m_AttackWaitingTime = 0.0f;

	float m_AttackAnimPauseTime = 0.0f;

	int m_LastAttackAnim = 1;

	bool m_PullBox = false;

	bool m_IsCanAttack = true;
	bool m_AttackKeyDown = false;

	float m_AttackedDelay = 0.0f;
	float m_SkillCoolTime = 0.0f;
	float m_ComboTimer = 0.5f;

	CGameObject* m_sword = nullptr;

public:
	bool m_Alive = true;

	bool pullString = false;
	float m_StringPullTime = 0.0f;
	vector<Collider*> swordCollider;

	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName);

	~CPlayer();

public:
	void Update(float fTimeElapsed) override;
	void UpdateOnServer(float fTimeElapsed) override;

	void UpdateCamera();
	void FixCameraByTerrain(CTerrain* pTerrain);

	void FixPositionByTerrain(CTerrain* pTerrain) override;

public:
	int GetPlayerExistingSector() const;
	virtual void SetVelocity(XMFLOAT3 dir) override;

	bool IsMoving() const { return Vector3::Length(m_xmf3Velocity) > 0.01f; }

	void SetWeapon(PlayerWeaponType weaponType);
	void SetDrawSword();
	void SetDrawBow();
	void SetDrawStaff();
	void SetWeaponPointer();
	PlayerWeaponType GetWeapon() { return m_WeaponType; }
	UINT GetSelectedWeapon() const { return (UINT)m_WeaponType; }

	void SetCanAttack(bool info) { m_IsCanAttack = info; }
	void SetPullBox(bool info) { m_PullBox = info; }
	bool IsCanAttack() const { return m_IsCanAttack; }

	bool IsBox() const { return m_IsCanAttack; }
	void SetSwordAttackKeyDown(bool info) { m_AttackKeyDown = info; }

	void IncreaseAttackWaitingTime(float time) { m_AttackWaitingTime = time; }
	//void IncreaseBoxPickWaitingTime(float time) { m_BoxPickWaitingTime = time; }
	//void IncreaseBoxDownWaitingTime(float time) { m_BoxDownWaitingTime = time; }

	float GetAttackWaitTime() const { return m_AttackWaitingTime; }
	float GetStringPullTime() const { return m_StringPullTime; }

	void AnimationChange(PlayerWeaponType weapon);

	void Jump();

	bool Attacked(CGameObject* pObject);
	void Attack(int type);
	void ResetAttack();
	void ResetBow();

	void Box_Pull(bool Pull_State);

	bool IsAleradyAttack() const { return m_IsAlreadyAttack; }
	void SetAleradyAttack(bool info) { m_IsAlreadyAttack = info; }

	void DisableSword();
	void DisableBow();
	bool ShotAble();
};