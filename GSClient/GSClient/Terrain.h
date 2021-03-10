#pragma once
class CTerrain
{
private:
	int						m_nWidth;
	int						m_nLength;

	int						m_nObjects;
	vector<CGameObject*>    m_Objects;

	long cxBlocks;
	long czBlocks;

public:
	CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nLength, int nBlockWidth, int nBlockLength, CShader* pShader);

	CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	virtual ~CTerrain();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};

