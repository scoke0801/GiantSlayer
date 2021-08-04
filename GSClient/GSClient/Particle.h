#pragma once

class ParticleObject : public CGameObject
{
private:
	bool m_IsCanUse = false;
	PARTICLE_TYPE m_Type;
	int m_idx = 0;

	float m_elapsedTime = 0.0f;

public:
	ParticleObject();
	virtual ~ParticleObject();

public:
	virtual void Animate(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed);

public:
	// GPU���� ��ƼŬ�� �����ϱ� ���Ͽ� 
	// ��ӹ��� ����� �뵵�� �ٸ��� ����մϴ�.
	// 1. �ӵ�, 2. ����, 3.���� ������, 4.��ġ 
	void SetSpeedVector(const XMFLOAT3& speed);
	void SetDirecionVector(const XMFLOAT3& direction);
	void SetParticleParameter(int idx, float parameter);

	bool IsCanUse() const { return m_IsCanUse; }
	void SetUseable(bool data) { m_IsCanUse = data; }
	void Setidx(int idx) { m_idx = idx; }

	void SetType(PARTICLE_TYPE type) { m_Type = type; }
	PARTICLE_TYPE GetParticleType() const { return m_Type; }
	int Getidx() const { return m_idx; }
};

class CParticle
{
private:
	vector<ParticleObject*> m_ParticleObjs;
	ParticleObject* m_Particle;

public:
	CParticle( );
	~CParticle();

public: 
	void AddParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		int count,
		PARTICLE_TYPE type);
	void AddParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		int count,
		PARTICLE_TYPE type,
		CMesh* pMesh);
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Update(float elapsedTime);

	int GetCanUseableParticle(PARTICLE_TYPE type);
	int GetCanUseableParticle(PARTICLE_TYPE type,int idx);

	void SetPosition(int idx, const XMFLOAT3& pos);
	void SetDirection(int idx, const XMFLOAT3& dir);

	void UseParticle(int idx, const XMFLOAT3& pos, const XMFLOAT3& dir,int LaserCount);
	void UseParticle(int idx, const XMFLOAT3& pos, const XMFLOAT3& dir);
	void UseParticle(int idx);

	ParticleObject* GetParticleObj(int idx) { return m_ParticleObjs[idx]; }
}; 
