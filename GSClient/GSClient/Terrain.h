#pragma once 
 
class CTerrain
{
private:
	int m_Heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1];
	XMFLOAT3 m_Normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1];

private:
	int						m_nWidth;
	int						m_nLength;
	int						F_L;

	int						m_nObjects;
	vector<CGameObject*>    m_Objects;
	
	long					cxBlocks;
	long					czBlocks;

public:
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

