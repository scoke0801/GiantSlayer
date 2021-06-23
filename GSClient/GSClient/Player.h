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
	None = 0x00,
	Sword = 0x01,
	Bow = 0x02
};

class CTerrain; 

class CPlayer : public CGameObjectVer2
{    
private:
	bool m_IsAlreadyAttack = false;
private:
	PlayerMoveType m_MovingType = PlayerMoveType::Run;
	PlayerWeaponType m_WeaponType = PlayerWeaponType::Sword;

	AnimationType IDLE = AnimationType::SWORD_IDLE;
	AnimationType RUN = AnimationType::SWORD_RUN;
	AnimationType ATK = AnimationType::SWORD_ATK;
	AnimationType DEATH = AnimationType::SWORD_DEATH;
	AnimationType CHANGEWEAPON = AnimationType::BOW_GET;

	bool m_isOnGround = true;
	float m_JumpTime = 0.0f;

private:
	float m_AttackAnimLength = 0.0f;
	float m_AttackWaitingTime = 0.0f;
	float m_AttackAnimPauseTime = 0.0f;
	bool m_IsCanAttack = true;

	float m_AttackedDelay = 0.0f;

	CMesh* m_SpareBoundingBox;
	Collider* m_SpareCollisionBox;
	Collider* m_SpareAABB;

	bool keydown = false;

public:
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

	virtual void SetVelocity(XMFLOAT3 dir) override;

	bool IsMoving() const { return Vector3::Length(m_xmf3Velocity) > 0.01f; }

	void SetWeapon(PlayerWeaponType weaponID) { m_WeaponType = weaponID; }
	PlayerWeaponType GetWeapon() { return m_WeaponType; }
	UINT GetSelectedWeapon() const { return (UINT)m_WeaponType; } 

	void SetCanAttack(bool info) { m_IsCanAttack = info; }
	bool IsCanAttack() const { return m_IsCanAttack; }
	void IncreaseAttackWaitingTime(float time) { m_AttackWaitingTime = time; }

	float GetAttackWaitTime() const { return m_AttackWaitingTime; }

	void Jump();

	bool Attacked(CGameObject* pObject);
	void Attack();

	bool IsAleradyAttack() const { return m_IsAlreadyAttack; }
	void SetAleradyAttack(bool info) { m_IsAlreadyAttack = info; }

	void AnimationChange(PlayerWeaponType weapon);
	void DisableSword();
	void DisableBow();

	void PullBowString(bool pull) {
		keydown = pull;
	}
};