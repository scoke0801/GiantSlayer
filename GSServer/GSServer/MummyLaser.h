#pragma once

#include "GameObject.h"

class ParticleObject;
class CMummy;
class CMummyLaser : public CGameObject
{
public: 
	bool m_Laser[3] = { true,true,true };
	vector<CMummy*> m_Friends;
	XMFLOAT3 Mummy_pos[3];
	 
	UINT				m_LaserIdx = 0;

	CMummyLaser() { m_isDrawable = true; m_ExistingSector = SECTOR_POSITION::SECTOR_4; }
	virtual ~CMummyLaser() {}

public: 
	void Update(float fTimeElapsed) override;

public:
	void SetTargetPosition(const XMFLOAT3& targetPos);
	void SetTargetVector(const XMFLOAT3& playerLookAt);

	bool IsCanUse() const { return m_isDrawable; }
	void SetUseable(bool data) { m_isDrawable = data; }
	void SetDrawable(bool drawable);
	void AddFriends_p(CMummy* mummy);
	void SetDirecionVector(const XMFLOAT3& direction);
	void InverseDirection();

	void ConnectParticle(ParticleObject* particle) { m_ConnectedParticle = particle; }

	Laser_TYPE GetLaserType() const { return m_LaserType; };

	void SetLaserType(Laser_TYPE laser);

	void SetLaser(UINT laser);

	UINT GetLaser() const { return m_LaserIdx; };

	void LookAtDirection(XMFLOAT3 dir, void* pContext);
	void LookAtDirections(XMFLOAT3 dir);
private:
	XMFLOAT3 m_xmf3TargetPosition;

	float m_ElapsedTime = 0.0f;
	float m_LaserElapsedTime[3] = { -10.0,0.0f,0.0f };
	ParticleObject* m_ConnectedParticle = nullptr;

	Laser_TYPE m_LaserType = Laser_TYPE::Laser1;
};



