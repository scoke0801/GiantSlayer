#pragma once
#include "GameObject.h"

class ParticleObject;

class CMagic : public CGameObject
{
public:
	CMagic();
	~CMagic();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void Update(float fTimeElapsed) override;

	void SetTarget();
	void TrackingTarget();

protected:
	CGameObject* caster;
	CGameObject* target;

};

