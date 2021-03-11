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
	// 이전에 사용하여 지형을 생성한 방식
	CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nLength, int nBlockWidth, int nBlockLength, CShader* pShader);

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

private:
	void InitHeightDatas();
};

