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

	void SetIsPlayerArrow(bool info) { m_IsPlayerArrow = info; }
	bool IsPlayerArrow(bool info) { return m_IsPlayerArrow; }

private:
	XMFLOAT3 m_xmf3TargetPosition;

	float m_ElapsedTime = 0.0f;

	float stringPower = 0.0f;

	CGameObject* m_pOwner;

	bool			m_IsPlayerArrow;
public:
	XMFLOAT3 m_startPos;
	bool falldown = false;
};

