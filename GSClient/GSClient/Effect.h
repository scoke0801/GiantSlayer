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
	// 이펙트가 보여질 대상 플레이어
	CPlayer* m_Targetplayer;
	
	// 이펙트가 그려질 시간
	float m_LifeTime;
	float m_ElapsedTime; // 생성 이후 경과 시간

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