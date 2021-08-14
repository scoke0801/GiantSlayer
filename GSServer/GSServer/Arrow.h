#pragma once
#include "GameObject.h"

class ParticleObject;
class CArrow :  public CGameObject
{
public:
	CArrow() { m_isUsing = true; }
	virtual ~CArrow() {}

public:
	void Update(float fTimeElapsed) override;

public:
	void SetTargetPosition(const XMFLOAT3& targetPos);
	void SetTargetVector(const XMFLOAT3& playerLookAt);  

	void SetStringPower(float power) { stringPower = power; }
private:
	XMFLOAT3 m_xmf3TargetPosition;

	float m_ElapsedTime = 0.0f;

	float stringPower = 0.0f;

	CGameObject* m_pOwner;

public:
	XMFLOAT3 m_startPos;
	bool falldown = false;
};

