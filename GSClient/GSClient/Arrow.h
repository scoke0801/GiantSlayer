#pragma once
#include "GameObject.h"

class ParticleObject;
class CArrow :  public CGameObject
{
public:
	CArrow() {}
	virtual ~CArrow() {}

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	void Update(float fTimeElapsed) override;

public:
	void SetTargetPosition(const XMFLOAT3& targetPos);

	bool IsCanUse() const { return m_IsCanUse; }
	void SetUseable(bool data) { m_IsCanUse = data; }

	void ConnectParticle(ParticleObject* particle) { m_ConnectedParticle = particle; }
private:
	XMFLOAT3 m_xmf3TargetPosition;

	float m_ElapsedTime = 0.0f;
	bool m_IsCanUse = true; 
	ParticleObject* m_ConnectedParticle = nullptr;
};

