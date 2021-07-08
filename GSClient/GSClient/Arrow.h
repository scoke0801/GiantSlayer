#pragma once
#include "GameObject.h"

class ParticleObject;
class CArrow :  public CGameObject
{
public:
	CArrow() { m_isDrawbale = true; }
	virtual ~CArrow() {}

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	void Update(float fTimeElapsed) override;

public:
	void SetTargetPosition(const XMFLOAT3& targetPos);
	void SetTargetVector(const XMFLOAT3& playerLookAt); 

	bool IsCanUse() const { return m_isDrawbale; }
	void SetUseable(bool data) { m_isDrawbale = data;  }
	void SetDrawable(bool drawable);

	void ConnectParticle(ParticleObject* particle) { m_ConnectedParticle = particle; }
private:
	XMFLOAT3 m_xmf3TargetPosition;

	float m_ElapsedTime = 0.0f; 
	ParticleObject* m_ConnectedParticle = nullptr;

public:
	XMFLOAT3 m_startPos;
	bool falldown = false;
};

