#pragma once

class CPlayer;

enum class EffectTypes {
	None,
	BossAttacked,
	Thunder
};
class CEffect : public CGameObject
{
public:
	CEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CPlayer* targetPlayer, const XMFLOAT3& size);
	~CEffect();

public:
	void Update(float elapsedTime) override;
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	 
public:
	void SetEffectType(EffectTypes effectType);
	EffectTypes GetEffectType() const { return m_EffectType; }

private:
	// ����Ʈ�� ������ ��� �÷��̾�
	CPlayer* m_Targetplayer;
	
	// ����Ʈ�� �׷��� �ð�
	float m_LifeTime;
	float m_ElapsedTime; // ���� ���� ��� �ð�

	EffectTypes m_EffectType;
};

class CEffectHandler
{
public:
	void Update(float elapsedTime);
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* targetPlayer);
	
	CEffect* RecycleEffect(EffectTypes effectType);

private:
	vector<CEffect*> m_Effects;
	CPlayer* m_TargetPlayer;
};