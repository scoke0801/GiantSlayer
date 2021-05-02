#pragma once

// ��ü ���� �ǰ� ���� ��� ����� ��ƼŬ �ý����� �����մϴ�.

enum class PARTICLE_TYPE : UINT {
	HitParticle,	// �ǰݽ� ����� ��ƼŬ
	ArrowParticle,	// ȭ�� �ڿ� ����� ��ƼŬ
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
	// GPU���� ��ƼŬ�� �����ϱ� ���Ͽ� 
	// ��ӹ��� ����� �뵵�� �ٸ��� ����մϴ�.
	// 1. �ӵ�, 2. ����, 3.���� ������, 4.��ġ 
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

// ��������� ���� ���.. 
// �ʿ��ϴٸ� �������� 
// �ƴϸ� ��������
class CParticleHandler {

};

