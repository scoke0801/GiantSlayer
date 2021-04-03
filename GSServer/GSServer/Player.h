#pragma once
#include "GameObject.h"  

enum class Player_Move_Type
{
	None = 0,
	Walk,
	Run
};
class CPlayer : public CGameObject
{ 
private:
	Player_Move_Type m_MovingType = Player_Move_Type::Run;

	short	m_Id;
	string	m_Name;
	bool	m_isExist = false;

public:
	CPlayer();
	~CPlayer();	
	
public:
	virtual void Update(double fTimeElapsed);

public:
	virtual void SetVelocity(OBJ_DIRECTION direction) override;

	bool IsMoving() const { return Vector3::Length(m_xmf3Velocity) > 0.01f; }

	void SetName(const string& name) { m_Name = name; }
	string GetName() const { return m_Name; }

	void SetId(short id) { m_Id = id; }
	short GetId() const { return m_Id; }

	void SetExistence(bool existence) { m_isExist = existence; };
	bool IsExist() const { return m_isExist; }
};