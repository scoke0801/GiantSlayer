#pragma once 
 
class CTerrain
{
private:
	CBindingTerrainMesh* m_BindTerrainMesh;
	CBindingTerrainMesh* m_BindTerrainMeshForLoosedWall[2];
private:
	int m_Heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1];
	XMFLOAT3 m_Normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1];

private:
	int						m_nWidth;
	int						m_nLength;
	int						F_L;

	int						m_nObjects;
	vector<CGameObject*>    m_Objects;
	vector<CGameObject*>	m_BlockingObjects;

	long					cxBlocks;
	long					czBlocks;

public: 
	CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	virtual ~CTerrain();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	  
public:
	// x,z��ġ�� ���� ���̸� ��ȯ�ϴ� �Լ��̴�.
	// ������Ʈ�� ���� ���� ��ġ�ϵ��� �ϱ� ���� �ʿ��� �Լ�.  
	float GetHeight(int xPosition, int zPosition);
	float GetDetailHeight(float xPosition, float zPosition);
	float GetHeight(const XMFLOAT3& xmf3Position) { return 0.0f; }

private: // ��輱 ������ �߰������� �������ֱ� ���� �Լ�.
	void BuildBackWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	void BuildFrontWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	void BuildLeftWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	void BuildRightWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);

	void ReviseLoosedTextureWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		CShader* pShader);
	 
private:
	void InitHeightDatas();
	void FileSave();
	void FileRead();

	void InitNormals();	
	XMFLOAT3 GetHeightMapNormal(int x, int z);
};


class CTerrainWater : public CGameObject
{
public:
	CTerrainWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		ID3D12RootSignature* pd3dGraphicsRootSignature, float fWidth, float fLength);
	virtual ~CTerrainWater();

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseShaderVariables();

	//void Animate(float fTimeElapsed) override;

	//void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL) override;

public:
	XMFLOAT4X4					m_xmf4x4Texture;

private:
	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

	XMFLOAT4X4					m_xmf4x4WaterAnimation;
};