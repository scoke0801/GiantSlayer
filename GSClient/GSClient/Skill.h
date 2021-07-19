#pragma once
#include "GameObject.h"

enum class SKtype {
	SK_Sword,
	SK_Arrow,
	MG_1,
	MG_2,
};

class CSkill : public CGameObject
{
public:
	CSkill(SKtype type);
	virtual ~CSkill();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void Update(float fTimeElapsed) override;

	void SetSkill(CGameObject* owner);
	void TrackingTarget(CGameObject target);

protected:
	CGameObject* m_Owner;
	CGameObject* target;

	float damage;
};