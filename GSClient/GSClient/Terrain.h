#pragma once 
 
#include "Camera.h"

class CTerrain
{
public:
	

private:
	CBindingTerrainMesh* m_BindTerrainMesh;
	CBindingTerrainMesh* m_BindTerrainMeshForLoosedWall[2];
public:
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
	// x,z위치의 지형 높이를 반환하는 함수이다.
	// 오브젝트가 지형 위에 위치하도록 하기 위해 필요한 함수.  
	float GetHeight(int xPosition, int zPosition);
	float GetDetailHeight(float xPosition, float zPosition);
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

public:
	void BernsteinCoeffcient5x5(float t,float fBernstein[5]);
	float CubicBezierSum5x5_C(float uB[5], float vB[5], int xIndex, int zIndex);

	void InitNormals();	
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	float GetHeighty(float nx,float nz, int xIndex, int zIndex);
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