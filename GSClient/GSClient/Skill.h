#pragma once
#include "GameObject.h"

class ParticleObject;

class CFireBall : public CGameObject
{
public:
	CFireBall();
	virtual ~CFireBall();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void Update(float fTimeElapsed) override;

	void SetSkill(CGameObject* owner);
	void TrackingTarget(CGameObject target);
	void SetTargetPosition(const XMFLOAT3& targetPos);

	bool IsCanUse() const { return m_isDrawable; }
	void SetUseable(bool data) { m_isDrawable = data; }
	void SetDrawable(bool drawable);

protected:
	CGameObject* m_Owner;
	CGameObject* target;

	float m_ElapsedTime = 0;
	float damage;

	XMFLOAT3 m_xmf3TargetPosition;

	ParticleObject* m_ConnectedParticle = nullptr;
};