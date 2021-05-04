#pragma once

// 객체 공격 피격 시점 등에서 사용할 파티클 시스템을 정의합니다.

enum class PARTICLE_TYPE : UINT {
	HitParticle,	// 피격시 사용할 파티클
	ArrowParticle,	// 화살 뒤에 사용할 파티클
};
constexpr float ARROW_PARTICLE_LIFE_TIME = 10.0f;

class ParticleObject : public CGameObject
{
private:
	bool m_IsCanUse = false;
	PARTICLE_TYPE m_Type;

	float m_elapsedTime = 0.0f;

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

	bool IsCanUse() const { return m_IsCanUse; }
	void SetUseable(bool data) { m_IsCanUse = data; }

	void SetType(PARTICLE_TYPE type) { m_Type = type; }
	PARTICLE_TYPE GetParticleType() const { return m_Type; }
};

class CParticle
{
private:
	vector<ParticleObject*> m_ParticleObjs;
public:
	CParticle( );
	~CParticle();

public: 
	void AddParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		int count,
		PARTICLE_TYPE type);
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Update(float elapsedTime);

	int GetCanUseableParticle(PARTICLE_TYPE type);

	void SetPosition(int idx, const XMFLOAT3& pos);
	void SetDirection(int idx, const XMFLOAT3& dir);

	void UseParticle(int idx, const XMFLOAT3& pos, const XMFLOAT3& dir);
	void UseParticle(int idx);
};

// 사용할지는 아직 고민.. 
// 필요하다면 정의하자 
// 아니면 삭제하자
class CParticleHandler {

};

