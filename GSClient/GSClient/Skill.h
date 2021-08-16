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

	void UpdateOnServer(float fTimeElapsed) override;

	void SetSkill(CGameObject* owner);
	void TrackingTarget(CGameObject target);
	void SetTargetPosition(const XMFLOAT3& targetPos);

	bool IsCanUse() const { return m_isDrawable; }
	void SetUseable(bool data) { m_isDrawable = data; }
	void SetDrawable(bool drawable);

	void SetExplosionBoundingBox(const XMFLOAT3& size);
	BoundingBox GetExplosionBB() { return m_ExplosionAABB; }

protected:
	CGameObject* m_Owner;
	CGameObject* target;

	BoundingBox		m_ExplosionBox;
	BoundingBox		m_ExplosionAABB;

	float m_ElapsedTime = 0;
	float damage;

	XMFLOAT3 m_xmf3TargetPosition;

	ParticleObject* m_ConnectedParticle = nullptr;
	
};