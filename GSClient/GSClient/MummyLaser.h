#pragma once
#include "GameObject.h"
#include "stdafx.h"

class ParticleObject;
class CMummy;
class CMummyLaser : public CGameObject
{
public:
	
	bool m_Laser[3] = { true,true,true };
	vector<CMummy*> m_Friends;
	XMFLOAT3 Mummy_pos[3];


	CMummyLaser() { m_isDrawable = true; m_ExistingSector = SECTOR_POSITION::SECTOR_4; }
	virtual ~CMummyLaser() {}

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	void Update(float fTimeElapsed) override;

public:
	void SetTargetPosition(const XMFLOAT3& targetPos);
	void SetTargetVector(const XMFLOAT3& playerLookAt);

	bool IsCanUse() const { return m_isDrawable; }
	void SetUseable(bool data) { m_isDrawable = data; }
	void SetDrawable(bool drawable);
	void AddFriends_p(CMummy* mummy);
	void SetDirecionVector(const XMFLOAT3& direction);
	 
	void ConnectParticle(ParticleObject* particle) { m_ConnectedParticle = particle; }
private:
	XMFLOAT3 m_xmf3TargetPosition;

	float m_ElapsedTime = 0.0f;
	float m_LaserElapsedTime[3] = { -10.0,0.0f,0.0f };
	ParticleObject* m_ConnectedParticle = nullptr;
};



