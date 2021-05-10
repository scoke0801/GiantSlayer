#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "stdafx.h"

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

class CPlayer : public CGameObject
{    
private:
	PlayerMoveType m_MovingType = PlayerMoveType::Run;
	PlayerWeaponType m_WeaponType = PlayerWeaponType::Sword;

	bool m_isOnGround = true;

private:
	float m_AttackWaitingTime = 0.0f;
	bool m_IsCanAttack = true;

public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CPlayer();	
	
public:
	void Update(float fTimeElapsed) override;

	void UpdateCamera();
	void FixCameraByTerrain(CTerrain* pTerrain);

	void FixPositionByTerrain(CTerrain* pTerrain);
	 
public:
	virtual void SetVelocity(OBJ_DIRECTION direction) override;
	virtual void SetVelocity(XMFLOAT3 dir) override;

	bool IsMoving() const { return Vector3::Length(m_xmf3Velocity) > 0.01f; }

	void SetWeapon(PlayerWeaponType weaponID) { m_WeaponType = weaponID; }
	UINT GetSelectedWeapon() const { return (UINT)m_WeaponType; } 

	void SetCanAttack(bool info) { m_IsCanAttack = info; }
	bool IsCanAttack() const { return m_IsCanAttack; }
	void IncreaseAttackWaitingTime(float time) { m_AttackWaitingTime = time; }

	void Jump();
};