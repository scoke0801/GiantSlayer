#pragma once 
 
class CTerrain
{
private:
	int m_Heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1];

private:
	int						m_nWidth;
	int						m_nLength;

	int						m_nObjects;
	vector<CGameObject*>    m_Objects;
	
	long cxBlocks;
	long czBlocks;

public:
	// ������ ����Ͽ� ������ ������ ���
	CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nLength, int nBlockWidth, int nBlockLength, CShader* pShader);

	// ������ �����Ͽ� ����� ���
	CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	virtual ~CTerrain();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	// x,z��ġ�� ���� ���̸� ��ȯ�ϴ� �Լ��̴�.
	// ������Ʈ�� ���� ���� ��ġ�ϵ��� �ϱ� ���� �ʿ��� �Լ�.  
	float GetHeight(int xPosition, int zPosition);
	float GetHeight(const XMFLOAT3& xmf3Position) { return 0.0f; }

private:
	void InitHeightDatas();
};

