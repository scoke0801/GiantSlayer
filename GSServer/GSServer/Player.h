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

public:
	CPlayer();
	~CPlayer();	
	
public:
	virtual void Update(double fTimeElapsed);

public:
	virtual void SetVelocity(OBJ_DIRECTION direction) override;

	bool IsMoving() const { return Vector3::Length(m_xmf3Velocity) > 0.01f; }
};