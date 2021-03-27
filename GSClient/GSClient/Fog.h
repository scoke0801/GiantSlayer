#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct VS_CB_FOG_INFO
{
	XMFLOAT4		m_FogColor = { 0.7f,0.7f,0.7f,1.0f };
	float			m_FogStart = 5.0f;
	float			m_FogRange = 150.0f;
};

class CFog
{
private:
	ID3D12Resource				* m_pd3dcbFog = NULL;
	VS_CB_FOG_INFO				* m_pcbFog = NULL;

public:
	CFog();
	~CFog();

	// for Update Loop
	void Update(float elapsedTime);
	void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	//void UpdateLights(float elapsedTime);

public:
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex = 1);


};