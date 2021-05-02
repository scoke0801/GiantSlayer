#pragma once

// ��ü ���� �ǰ� ���� ��� ����� ��ƼŬ �ý����� �����մϴ�.

enum class PARTICLE_TYPE : UINT {
	HitParticle,	// �ǰݽ� ����� ��ƼŬ
	ArrowParticle,	// ȭ�� �ڿ� ����� ��ƼŬ
};
class CParticle
{
private:
	vector<CGameObject*> m_ParticleObjs;

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

