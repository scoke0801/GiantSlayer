#pragma once
#include "AnimationObject.h"
#include "Camera.h"

enum class PlayerMoveType
{
	None = 0,
	Walk,
	Run
};
 

class CPlayer : public CAnimationObject
{ 
public:
#ifdef _DEBUG
	int		m_moveTime;
#endif
	AnimationType m_StateName;

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
	PlayerMoveType m_MovingType = PlayerMoveType::Run;
	PlayerWeaponType m_WeaponType = PlayerWeaponType::Sword;

	short	m_Id;
	string	m_Name;
	bool	m_isExist = false;

	CCamera* m_Camera = nullptr;

private:  
	bool m_IsCanAttack = true;

	bool m_IsAlreadyAttack = false;
	 
	float m_ComboTimer = 0.2f;

	bool m_AttackKeyDown = false;

private:
	float m_SkillCoolTime = 0.0f;
	float m_AttackedDelay = 0.0f;
	float m_AttackAnimLength = 0.0f;
	float m_SwordAnim1Length = 0.0f;
	float m_SwordAnim2Length = 0.0f;
	float m_SwordAnim3Length = 0.0f;
	float m_SkillAnimLength = 0.0f;
	float m_DeathAnimLength = 0.0f;
	float m_AttackWaitingTime = 0.0f;

	float m_AttackAnimPauseTime = 0.0f;

	int m_LastAttackAnim = 0;

	bool m_PullBox = false; 
public:
	bool pullString = false;
	float m_StringPullTime = 0.0f;
	 
	bool m_AnimationPaused = false;

	bool m_Alive = true;
	 
public:
	CPlayer();
	~CPlayer();	
	
public:
	void Update(float fTimeElapsed) override;
	void UpdateCamera();

	void FixCameraByTerrain(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1]);
	void FixPositionByTerrain(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1]) override;

	int GetPlayerExistingSector() const;

public:
	virtual void SetVelocity(const XMFLOAT3& dir) override;

	bool IsMoving() const { return Vector3::Length(m_xmf3Velocity) > 0.01f; }

	void SetName(const string& name) { m_Name = name; }
	string GetName() const { return m_Name; }

	void SetId(short id) { m_Id = id; }
	short GetId() const { return m_Id; }

	void SetExistence(bool existence) { m_isExist = existence; };
	bool IsExist() const { return m_isExist; }
	 
	void SetCamera(CCamera* camera) { m_Camera = camera; }
	CCamera* GetCamera() const { return m_Camera; }
	  
	PlayerWeaponType GetWeaponType() const { return m_WeaponType; }
	void SetWeaponType(int weaponType) { m_WeaponType = (PlayerWeaponType)weaponType; }

	void SetSwordAttackKeyDown(bool info) { m_AttackKeyDown = info; }
	void SetWeaponPointer(); 

	float GetAttackWaitTime() const { return m_AttackWaitingTime; }
	float GetStringPullTime() const { return m_StringPullTime; }

public:
	void SetCanAttack(bool info) { m_IsCanAttack = info; }
	bool IsCanAttack() const { return m_IsCanAttack; }
	void IncreaseAttackWaitingTime(float time) { m_AttackWaitingTime = time; }

	bool IsAleradyAttack() const { return m_IsAlreadyAttack; }
	void SetAleradyAttack(bool info) { m_IsAlreadyAttack = info; }

	void AnimationChange(PlayerWeaponType weapon); 

	bool ShotAble();

public:
	bool Attacked(CGameObject* pObject);
	void Attack(int type);
	void ResetAttack();

	AnimationType GetStateName() const { return m_StateName; } 
};