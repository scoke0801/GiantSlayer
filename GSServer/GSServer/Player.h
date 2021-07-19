#pragma once
#include "GameObject.h"  
#include "Camera.h"

enum class Player_Move_Type
{
	None = 0,
	Walk,
	Run
};
class CPlayer : public CGameObject
{ 
public:
#ifdef _DEBUG
	int		m_moveTime;
#endif
	AnimationType m_StateName;

private:
	Player_Move_Type m_MovingType = Player_Move_Type::Run;

	short	m_Id;
	string	m_Name;
	bool	m_isExist = false;

	CCamera* m_Camera = nullptr;

private:
	float m_AttackWaitingTime = 0.0f;
	bool m_IsCanAttack = true;

	float m_AttackedDelay = 0.0f;
	bool m_IsAlreadyAttack = false;

	BoundingBox m_SpareCollisionBox;
	BoundingBox m_SpareAABB;
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
	 

public:
	void SetCanAttack(bool info) { m_IsCanAttack = info; }
	bool IsCanAttack() const { return m_IsCanAttack; }
	void IncreaseAttackWaitingTime(float time) { m_AttackWaitingTime = time; }

public:
	bool Attacked(CGameObject* pObject);
	void Attack();

	AnimationType GetStateName() const { return m_StateName; }
	  
	bool IsAleradyAttack() const { return m_IsAlreadyAttack; }
	void SetAleradyAttack(bool info) { m_IsAlreadyAttack = info; }
};