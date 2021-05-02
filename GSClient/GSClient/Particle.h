#pragma once

// 객체 공격 피격 시점 등에서 사용할 파티클 시스템을 정의합니다.

enum class PARTICLE_TYPE : UINT {
	HitParticle,	// 피격시 사용할 파티클
	ArrowParticle,	// 화살 뒤에 사용할 파티클
};

class ParticleObject : public CGameObject
{
public:
	ParticleObject();
	virtual ~ParticleObject();

public:
	virtual void Animate(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed);

public:
	// GPU에서 파티클을 조작하기 위하여 
	// 상속받은 행렬의 용도를 다르게 사용합니다.
	// 1. 속도, 2. 방향, 3.계산용 변수들, 4.위치 
	void SetSpeedVector(const XMFLOAT3& speed);
	void SetDirecionVector(const XMFLOAT3& direction);
	void SetParticleParameter(int idx, float parameter);
};

class CParticle
{
private:
	vector<ParticleObject*> m_ParticleObjs;

public:
	CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count);
	~CParticle();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Update(float elapsedTime);
};

// 사용할지는 아직 고민.. 
// 필요하다면 정의하자 
// 아니면 삭제하자
class CParticleHandler {

};

