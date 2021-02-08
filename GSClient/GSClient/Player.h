#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "stdafx.h"

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
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CPlayer();	
	
public:
	virtual void Update(double fTimeElapsed);

public:
	virtual void SetVelocity(OBJ_DIRECTION direction) override;
};