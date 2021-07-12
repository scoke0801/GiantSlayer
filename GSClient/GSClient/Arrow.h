#pragma once
#include "GameObject.h"

class ParticleObject;
class CArrow :  public CGameObject
{
public:
	CArrow() { m_isDrawable = true; }
	virtual ~CArrow() {}

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	void Update(float fTimeElapsed) override;

public:
	void SetTargetPosition(const XMFLOAT3& targetPos);
	void SetTargetVector(const XMFLOAT3& playerLookAt); 

	bool IsCanUse() const { return m_isDrawable; }
	void SetUseable(bool data) { m_isDrawable = data;  }
	void SetDrawable(bool drawable);

	void ConnectParticle(ParticleObject* particle) { m_ConnectedParticle = particle; }

	void SetStringPower(float power) { stringPower = power; }
private:
	XMFLOAT3 m_xmf3TargetPosition;

	float m_ElapsedTime = 0.0f; 
	ParticleObject* m_ConnectedParticle = nullptr;

	float stringPower = 0.0f;

public:
	XMFLOAT3 m_startPos;
	bool falldown = false;
};

