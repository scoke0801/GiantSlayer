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
	// 앞으로 수정하여 사용할 방식
	CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	virtual ~CTerrain();

public:
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	
public:
	// x,z위치의 지형 높이를 반환하는 함수이다.
	// 오브젝트가 지형 위에 위치하도록 하기 위해 필요한 함수.  
	float GetHeight(int xPosition, int zPosition);
	float GetHeight(const XMFLOAT3& xmf3Position) { return 0.0f; }

private: // 경계선 지형을 추가적으로 생성해주기 위한 함수.
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

