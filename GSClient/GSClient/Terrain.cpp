#include "stdafx.h"
#include "Terrain.h"

CTerrain::CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	CShader* pShader)
{
	m_nWidth = 100, m_nLength = 100;
	InitHeightDatas();
	InitNormals();
	 
	int vertexMeshCount = 25;
	int meshVertexCount = 833 * vertexMeshCount;//20825
	int loosedWallCount[2] = { 110, 5 };

	m_BindTerrainMesh = new CBindingTerrainMesh(pd3dDevice, pd3dCommandList, meshVertexCount);
	m_BindTerrainMeshForLoosedWall[0] = new CBindingTerrainMesh(pd3dDevice, pd3dCommandList, vertexMeshCount * loosedWallCount[0]);
	m_BindTerrainMeshForLoosedWall[1] = new CBindingTerrainMesh(pd3dDevice, pd3dCommandList, vertexMeshCount * loosedWallCount[1]);

	for (int i = 0; i < 25; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			// 늘어진 텍스쳐 발생 부분 제외 후 생성
			if (j == 17 && (i >= 17 && i <= 24) ||
				(j == 19) && (i >= 10 && i < 17) ||
				(j == 22) && (i >= 10 && i < 17))
				continue;
			if ((j == 24 || j == 23 || j == 18 || j == 17) && i == 17)
				continue;
			if ((j == 19 || j == 22)&& i == 17) 
				continue;
			BYTE textureInfo = 0x00;
			if (i < 19 && j < 12)
			{
				textureInfo = 0x01;
			}
			else if (i >= 19 && j < 12)
			{
				textureInfo = 0x02;
			}
			else if (i >= 5 && j >= 12 && j < 17)
			{
				textureInfo = 0x04;
			}
			else if (i < 5 && j >= 12 && j < 25)
			{
				textureInfo = 0x08;
			}
			else if (i > 0 && i <= 10 && j >= 17 && j < 25)
			{
				textureInfo = 0x08;
			}
			else
			{
				textureInfo = 0x10;
			}
			
			m_BindTerrainMesh->CreateGridMesh(pd3dDevice, pd3dCommandList, 
				{ 4.0f * j, 0, 4.0f * i },
				textureInfo,
				4 * j, 4 * i,
				m_Heights,
				m_Normals);   
		}
	} 
	
	BuildFrontWalls(pd3dDevice, pd3dCommandList, pShader);
	BuildLeftWalls(pd3dDevice, pd3dCommandList, pShader);
	BuildRightWalls(pd3dDevice, pd3dCommandList, pShader);
	BuildBackWalls(pd3dDevice, pd3dCommandList, pShader);

	ReviseLoosedTextureWall(pd3dDevice, pd3dCommandList, pShader);

	m_BindTerrainMesh->CreateVertexBuffer(pd3dDevice, pd3dCommandList);
	m_BindTerrainMeshForLoosedWall[0]->CreateVertexBuffer(pd3dDevice, pd3dCommandList);
	m_BindTerrainMeshForLoosedWall[1]->CreateVertexBuffer(pd3dDevice, pd3dCommandList);

	CGameObject* pObject = new CGameObject();
	pObject->SetShader(pShader); 
	pObject->SetTextureIndex(0x01); 
	pObject->SetMesh(m_BindTerrainMesh); 
	pObject->Scale(200.0f, 1.0f, 200.0f);
	m_Objects.push_back(std::move(pObject)); 

	pObject = new CGameObject();
	pObject->SetShader(pShader);
	pObject->SetTextureIndex(0x01);
	pObject->SetMesh(m_BindTerrainMeshForLoosedWall[0]);
	pObject->Scale(40.0f, 1.0f, 200.0f);
	m_Objects.push_back(std::move(pObject));

	pObject = new CGameObject();
	pObject->SetShader(pShader);
	pObject->SetTextureIndex(0x01);
	pObject->SetMesh(m_BindTerrainMeshForLoosedWall[1]);
	pObject->Scale(200.0f, 1.0f, 40.0f);
	m_Objects.push_back(std::move(pObject));
}

CTerrain::~CTerrain()
{
}

void CTerrain::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto pObj : m_Objects)
	{
		pObj->Draw(pd3dCommandList, pCamera);
	}
}

float CTerrain::GetHeight(int xPosition, int zPosition)
{
	int x = xPosition / 200.0f;
	int z = zPosition / 200.0f;

	return m_Heights[z][x];
}

float CTerrain::GetDetailHeight(float xPosition, float zPosition)
{
	// 1. center
	// 2. left end
	// 3. right end

	float fx = xPosition / 200.0f;
	float fz = zPosition / 200.0f;

	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다.
	높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면
	지형의 높이는0이다.*/
	if ((fx < 0.0f)
		|| (fz < 0.0f)
		|| (fx > 100)
		|| (fz > 100))
		return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	int BottomLeft = m_Heights[z][x];
	int BottomRight = m_Heights[z][x + 1];
	int TopLeft = m_Heights[z+1][x];
	int TopRight = m_Heights[z+1][x + 1];
	 
	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = TopLeft * (1 - fxPercent) + TopRight * fxPercent;
	float fBottomHeight = BottomLeft * (1 - fxPercent) + BottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}

void CTerrain::BuildBackWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	BYTE textureInfo = 0x00;
	int heightWallIndex = 99;
	for (int j = 0; j < 68; j += 4)
	{
		(j < 48) ? textureInfo = (0x02) : textureInfo = 0x04;

		int heightsTemp[25] = {
			m_Heights[heightWallIndex][j] + 200.0f, m_Heights[heightWallIndex][j + 1] + 200.0f, m_Heights[heightWallIndex][j + 2] + 200.0f, m_Heights[heightWallIndex][j + 3] + 200.0f, m_Heights[heightWallIndex][j + 4] + 200.0f,
			m_Heights[heightWallIndex][j] + 500.0f, m_Heights[heightWallIndex][j + 1] + 500.0f, m_Heights[heightWallIndex][j + 2] + 500.0f, m_Heights[heightWallIndex][j + 3] + 500.0f, m_Heights[heightWallIndex][j + 4] + 500.0f,
			m_Heights[heightWallIndex][j] + 700.0f, m_Heights[heightWallIndex][j + 1] + 700.0f, m_Heights[heightWallIndex][j + 2] + 700.0f, m_Heights[heightWallIndex][j + 3] + 700.0f, m_Heights[heightWallIndex][j + 4] + 700.0f,
			m_Heights[heightWallIndex][j] + 700.0f, m_Heights[heightWallIndex][j + 1] + 700.0f, m_Heights[heightWallIndex][j + 2] + 700.0f, m_Heights[heightWallIndex][j + 3] + 700.0f, m_Heights[heightWallIndex][j + 4] + 700.0f,
			m_Heights[heightWallIndex][j],          m_Heights[heightWallIndex][j + 1],			m_Heights[heightWallIndex][j + 2],		    m_Heights[heightWallIndex][j + 3],			m_Heights[heightWallIndex][j + 4] };
		 
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ 1.0f * j, 0, 100.0f }, 
			textureInfo,
			heightsTemp,
			m_Normals,
			j, 96);  
	}

	for (int j = 72; j < 100; j += 4)
	{
		textureInfo = 0x20;

		int heightsTemp[25] = {
			m_Heights[heightWallIndex][j] + 200.0f,  m_Heights[heightWallIndex][j + 1] + 200.0f,  m_Heights[heightWallIndex][j + 2] + 200.0f,  m_Heights[heightWallIndex][j + 3] + 200.0f,  m_Heights[heightWallIndex][j + 4] + 200.0f,
			m_Heights[heightWallIndex][j] + 500.0f,  m_Heights[heightWallIndex][j + 1] + 500.0f,  m_Heights[heightWallIndex][j + 2] + 500.0f,  m_Heights[heightWallIndex][j + 3] + 500.0f,  m_Heights[heightWallIndex][j + 4] + 500.0f,
			m_Heights[heightWallIndex][j] + 1500.0f, m_Heights[heightWallIndex][j + 1] + 1500.0f, m_Heights[heightWallIndex][j + 2] + 1500.0f, m_Heights[heightWallIndex][j + 3] + 1500.0f, m_Heights[heightWallIndex][j + 4] + 1500.0f,
			m_Heights[heightWallIndex][j] + 700.0f,  m_Heights[heightWallIndex][j + 1] + 700.0f,  m_Heights[heightWallIndex][j + 2] + 700.0f,  m_Heights[heightWallIndex][j + 3] + 700.0f,  m_Heights[heightWallIndex][j + 4] + 700.0f,
			m_Heights[heightWallIndex][j],           m_Heights[heightWallIndex][j + 1],			  m_Heights[heightWallIndex][j + 2],		   m_Heights[heightWallIndex][j + 3],			m_Heights[heightWallIndex][j + 4] };
		 
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ 1.0f * j, 0, 100.0f },
			textureInfo,
			heightsTemp,
			m_Normals,
			j, 96); 
	}
	{
		int j = 0;
		textureInfo = (0x02);

		int heightsTemp[25] = {
		m_Heights[heightWallIndex][j + 4] + 200.0f, m_Heights[heightWallIndex][j + 3] + 200.0f, m_Heights[heightWallIndex][j + 2] + 200.0f, m_Heights[heightWallIndex][j + 1] + 200.0f, m_Heights[heightWallIndex][j] + 200.0f,
		m_Heights[heightWallIndex][j + 4] + 500.0f, m_Heights[heightWallIndex][j + 3] + 500.0f, m_Heights[heightWallIndex][j + 2] + 500.0f, m_Heights[heightWallIndex][j + 1] + 500.0f, m_Heights[heightWallIndex][j] + 500.0f,
		m_Heights[heightWallIndex][j + 4] + 700.0f, m_Heights[heightWallIndex][j + 3] + 700.0f, m_Heights[heightWallIndex][j + 2] + 700.0f, m_Heights[heightWallIndex][j + 1] + 700.0f, m_Heights[heightWallIndex][j] + 700.0f,
		m_Heights[heightWallIndex][j + 4] + 700.0f, m_Heights[heightWallIndex][j + 3] + 700.0f, m_Heights[heightWallIndex][j + 2] + 700.0f, m_Heights[heightWallIndex][j + 1] + 700.0f, m_Heights[heightWallIndex][j] + 700.0f,
		m_Heights[heightWallIndex][j + 4],          m_Heights[heightWallIndex][j + 3],			m_Heights[heightWallIndex][j + 2],		    m_Heights[heightWallIndex][j + 1],			m_Heights[heightWallIndex][j] };
		 
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ -4.0f, 0, 100.0f },
			textureInfo,
			heightsTemp,
			m_Normals,
			j, 96);  
	}
	{
		int j = 96; 
		textureInfo = (0x20);

		int heightsTemp[25] = {
				m_Heights[heightWallIndex][j + 4] + 200.0f, m_Heights[heightWallIndex][j + 3] + 200.0f, m_Heights[heightWallIndex][j + 2] + 200.0f, m_Heights[heightWallIndex][j + 1] + 200.0f, m_Heights[heightWallIndex][j] + 200.0f,
				m_Heights[heightWallIndex][j + 4] + 500.0f, m_Heights[heightWallIndex][j + 3] + 500.0f, m_Heights[heightWallIndex][j + 2] + 500.0f, m_Heights[heightWallIndex][j + 1] + 500.0f, m_Heights[heightWallIndex][j] + 500.0f,
				m_Heights[heightWallIndex][j + 4] + 1500.0f, m_Heights[heightWallIndex][j + 3] + 1500.0f, m_Heights[heightWallIndex][j + 2] + 1500.0f, m_Heights[heightWallIndex][j + 1] + 1500.0f, m_Heights[heightWallIndex][j] + 1500.0f,
				m_Heights[heightWallIndex][j + 4] + 700.0f, m_Heights[heightWallIndex][j + 3] + 700.0f, m_Heights[heightWallIndex][j + 2] + 700.0f, m_Heights[heightWallIndex][j + 1] + 700.0f, m_Heights[heightWallIndex][j] + 700.0f,
				m_Heights[heightWallIndex][j + 4],          m_Heights[heightWallIndex][j + 3],			m_Heights[heightWallIndex][j + 2],		    m_Heights[heightWallIndex][j + 1],			m_Heights[heightWallIndex][j] };
		 
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ 100.0f, 0, 100.0f },
			textureInfo,
			heightsTemp,
			m_Normals,
			j, 96);
	}
	{
		int j = 72; 
		textureInfo = (0x20);

		int heightsTemp[25] = {
				m_Heights[heightWallIndex][j + 4] + 200.0f, m_Heights[heightWallIndex][j + 3] + 200.0f, m_Heights[heightWallIndex][j + 2] + 200.0f, m_Heights[heightWallIndex][j + 1] + 200.0f, m_Heights[heightWallIndex][j] + 200.0f,
				m_Heights[heightWallIndex][j + 4] + 500.0f, m_Heights[heightWallIndex][j + 3] + 500.0f, m_Heights[heightWallIndex][j + 2] + 500.0f, m_Heights[heightWallIndex][j + 1] + 500.0f, m_Heights[heightWallIndex][j] + 500.0f,
				m_Heights[heightWallIndex][j + 4] + 1500.0f, m_Heights[heightWallIndex][j + 3] + 1500.0f, m_Heights[heightWallIndex][j + 2] + 1500.0f, m_Heights[heightWallIndex][j + 1] + 1500.0f, m_Heights[heightWallIndex][j] + 1500.0f,
				m_Heights[heightWallIndex][j + 4] + 700.0f, m_Heights[heightWallIndex][j + 3] + 700.0f, m_Heights[heightWallIndex][j + 2] + 700.0f, m_Heights[heightWallIndex][j + 1] + 700.0f, m_Heights[heightWallIndex][j] + 700.0f,
				m_Heights[heightWallIndex][j + 4],          m_Heights[heightWallIndex][j + 3],			m_Heights[heightWallIndex][j + 2],		    m_Heights[heightWallIndex][j + 1],			m_Heights[heightWallIndex][j] };
		 
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList, 
			{ 1.0f * j - 4.0f, 0, 100.0f }, 
			textureInfo,
			heightsTemp,
			m_Normals,
			j, 96);  
	}
}

void CTerrain::BuildFrontWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	BYTE textureInfo = 0x00;
	for (int j = 0; j < 48; j += 4) {
		textureInfo = 0x01;

		int heightsTemp[25] = {
				m_Heights[0][j],		  m_Heights[0][j + 1],		    m_Heights[0][j + 2],		  m_Heights[0][j + 3],		    m_Heights[0][j + 4],
				m_Heights[0][j] + 700.0f, m_Heights[0][j + 1] + 700.0f, m_Heights[0][j + 2] + 700.0f, m_Heights[0][j + 3] + 700.0f, m_Heights[0][j + 4] + 700.0f,
				m_Heights[0][j] + 700.0f, m_Heights[0][j + 1] + 700.0f, m_Heights[0][j + 2] + 700.0f, m_Heights[0][j + 3] + 700.0f, m_Heights[0][j + 4] + 700.0f,
				m_Heights[0][j] + 500.0f, m_Heights[0][j + 1] + 500.0f, m_Heights[0][j + 2] + 500.0f, m_Heights[0][j + 3] + 500.0f, m_Heights[0][j + 4] + 500.0f,
				m_Heights[0][j] + 200.0f, m_Heights[0][j + 1] + 200.0f, m_Heights[0][j + 2] + 200.0f, m_Heights[0][j + 3] + 200.0f, m_Heights[0][j + 4] + 200.0f };
		
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ 1.0f * j , 0, -4.0f },
			textureInfo,
			heightsTemp,
			m_Normals,
			j, 0); 
	}
	
	for (int j = 52; j < 100; j += 4) {
		textureInfo = 0x08;
		int heightsTemp[25] = {
		m_Heights[0][j],		  m_Heights[0][j + 1],		    m_Heights[0][j + 2],		  m_Heights[0][j + 3],		    m_Heights[0][j + 4],
		m_Heights[0][j] + 700.0f, m_Heights[0][j + 1] + 700.0f, m_Heights[0][j + 2] + 700.0f, m_Heights[0][j + 3] + 700.0f, m_Heights[0][j + 4] + 700.0f,
		m_Heights[0][j] + 700.0f, m_Heights[0][j + 1] + 700.0f, m_Heights[0][j + 2] + 700.0f, m_Heights[0][j + 3] + 700.0f, m_Heights[0][j + 4] + 700.0f,
		m_Heights[0][j] + 500.0f, m_Heights[0][j + 1] + 500.0f, m_Heights[0][j + 2] + 500.0f, m_Heights[0][j + 3] + 500.0f, m_Heights[0][j + 4] + 500.0f,
		m_Heights[0][j] + 200.0f, m_Heights[0][j + 1] + 200.0f, m_Heights[0][j + 2] + 200.0f, m_Heights[0][j + 3] + 200.0f, m_Heights[0][j + 4] + 200.0f };

		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList, 
			{ 1.0f * j , 0.0f, -4.0f }, 
			textureInfo, 
			heightsTemp,
			m_Normals,
			j, 0);   
	} 

	{
		textureInfo = 0x01;

		int heightsTemp[25] = {
				m_Heights[0][0],		  m_Heights[0][1],		    m_Heights[0][2],		  m_Heights[0][3],		    m_Heights[0][4],
				m_Heights[0][0] + 700.0f, m_Heights[0][1] + 700.0f, m_Heights[0][2] + 700.0f, m_Heights[0][3] + 700.0f, m_Heights[0][4] + 700.0f,
				m_Heights[0][0] + 700.0f, m_Heights[0][1] + 700.0f, m_Heights[0][2] + 700.0f, m_Heights[0][3] + 700.0f, m_Heights[0][4] + 700.0f,
				m_Heights[0][0] + 500.0f, m_Heights[0][1] + 500.0f, m_Heights[0][2] + 500.0f, m_Heights[0][3] + 500.0f, m_Heights[0][4] + 500.0f,
				m_Heights[0][0] + 200.0f, m_Heights[0][1] + 200.0f, m_Heights[0][2] + 200.0f, m_Heights[0][3] + 200.0f, m_Heights[0][4] + 200.0f };
		
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList, 
			{ -4.0f , 0, -4.0f }, 
			textureInfo,
			heightsTemp,
			m_Normals,
			0, 0); 
	}

	{
		int j = 52;
		textureInfo = 0x08;
		int heightsTemp[25] = {
		m_Heights[0][j + 4] - 100.0f, m_Heights[0][j + 3] - 100.0f,	m_Heights[0][j + 2] - 100.0f, m_Heights[0][j + 1] - 100.0f, m_Heights[0][j] - 100.0f,
		m_Heights[0][j + 4] + 700.0f, m_Heights[0][j + 3] + 700.0f, m_Heights[0][j + 2] + 700.0f, m_Heights[0][j + 1] + 700.0f, m_Heights[0][j] + 700.0f,
		m_Heights[0][j + 4] + 700.0f, m_Heights[0][j + 3] + 700.0f, m_Heights[0][j + 2] + 700.0f, m_Heights[0][j + 1] + 700.0f, m_Heights[0][j] + 700.0f,
		m_Heights[0][j + 4] + 500.0f, m_Heights[0][j + 3] + 500.0f, m_Heights[0][j + 2] + 500.0f, m_Heights[0][j + 1] + 500.0f, m_Heights[0][j] + 500.0f,
		m_Heights[0][j + 4] + 200.0f, m_Heights[0][j + 3] + 200.0f, m_Heights[0][j + 2] + 200.0f, m_Heights[0][j + 1] + 200.0f, m_Heights[0][j] + 200.0f };

		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ 1.0f * j - 4.0f, 0.0f, -4.0f }, 
			textureInfo,
			heightsTemp,
			m_Normals,
			j, 0); 
	}
}

void CTerrain::BuildLeftWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	BYTE textureInfo = 0x00;
	for (int j = 0; j < 100; j += 4)
	{
		(j < 76) ? textureInfo = (0x01) : textureInfo = (0x02);

		int heightsTemp[25] = {
				m_Heights[j + 4][0] + 200.0f, m_Heights[j + 4][0] + 500.0f, m_Heights[j + 4][0] + 700.0f, m_Heights[j + 4][0] + 700.0f, m_Heights[j + 4][0],
				m_Heights[j + 3][0] + 200.0f, m_Heights[j + 3][0] + 500.0f, m_Heights[j + 3][0] + 700.0f, m_Heights[j + 3][0] + 700.0f, m_Heights[j + 3][0],
				m_Heights[j + 2][0] + 200.0f, m_Heights[j + 2][0] + 500.0f, m_Heights[j + 2][0] + 700.0f, m_Heights[j + 2][0] + 700.0f, m_Heights[j + 2][0],
				m_Heights[j + 1][0] + 200.0f, m_Heights[j + 1][0] + 500.0f, m_Heights[j + 1][0] + 700.0f, m_Heights[j + 1][0] + 700.0f, m_Heights[j + 1][0],
				m_Heights[j + 0][0] + 200.0f, m_Heights[j + 0][0] + 500.0f, m_Heights[j + 0][0] + 700.0f, m_Heights[j + 0][0] + 700.0f, m_Heights[j + 0][0] };

		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList, 
			{ -4.0f, 0, 1.0f * j },
			textureInfo,
			heightsTemp,
			m_Normals,
			0, j); 
	}
	{
		textureInfo = 0x01;

		int heightsTemp[25] = {
				m_Heights[4][0] + 200.0f, m_Heights[4][0] + 500.0f, m_Heights[4][0] + 700.0f, m_Heights[4][0] + 700.0f, m_Heights[4][0],
				m_Heights[3][0] + 200.0f, m_Heights[3][0] + 500.0f, m_Heights[3][0] + 700.0f, m_Heights[3][0] + 700.0f, m_Heights[3][0],
				m_Heights[2][0] + 200.0f, m_Heights[2][0] + 500.0f, m_Heights[2][0] + 700.0f, m_Heights[2][0] + 700.0f, m_Heights[2][0],
				m_Heights[1][0] + 200.0f, m_Heights[1][0] + 500.0f, m_Heights[1][0] + 700.0f, m_Heights[1][0] + 700.0f, m_Heights[1][0],
				m_Heights[0][0] + 200.0f, m_Heights[0][0] + 500.0f, m_Heights[0][0] + 700.0f, m_Heights[0][0] + 700.0f, m_Heights[0][0] };

		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList, 
			{ -4.0f, 0, -4.0f },
			textureInfo,
			heightsTemp,
			m_Normals,
			0, 0); 
	}

	{
		textureInfo = 0x02;

		int heightsTemp[25] = {
				m_Heights[96][0] + 200.0f,    m_Heights[96][0] + 500.0f, m_Heights[96][0] + 700.0f,  m_Heights[96][0] + 700.0f,  m_Heights[96][0] ,
				m_Heights[97][0] + 200.0f,   m_Heights[97][0] + 500.0f,  m_Heights[97][0] + 700.0f,  m_Heights[97][0] + 700.0f,  m_Heights[97][0] ,
				m_Heights[98][0] + 200.0f,   m_Heights[98][0] + 500.0f,  m_Heights[98][0] + 700.0f,  m_Heights[98][0] + 700.0f,  m_Heights[98][0] ,
				m_Heights[99][0] + 200.0f,   m_Heights[99][0] + 500.0f,  m_Heights[99][0] + 700.0f,  m_Heights[99][0] + 700.0f,  m_Heights[99][0] ,
				m_Heights[100][0] + 200.0f,  m_Heights[100][0] + 500.0f, m_Heights[100][0] + 700.0f, m_Heights[100][0] + 700.0f, m_Heights[100][0] };

		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ -4.0f, 0, 100.0f }, 
			textureInfo,
			heightsTemp,
			m_Normals,
			0, 96); 
	}
}

void CTerrain::BuildRightWalls(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	BYTE textureInfo = 0x00;
	int heightWallIndex = 99;
	for (int j = 0; j < 68; j += 4)
	{
		(j < 42) ? textureInfo = (0x08) : textureInfo = (0x20);

		int heightsTemp[25] = {
				m_Heights[j + 4][heightWallIndex], m_Heights[j + 4][heightWallIndex] + 700.0f, m_Heights[j + 4][heightWallIndex] + 700.0f, m_Heights[j + 4][heightWallIndex] + 500.0f, m_Heights[j + 4][heightWallIndex] + 200.0f,
				m_Heights[j + 3][heightWallIndex], m_Heights[j + 3][heightWallIndex] + 700.0f, m_Heights[j + 3][heightWallIndex] + 700.0f, m_Heights[j + 3][heightWallIndex] + 500.0f, m_Heights[j + 3][heightWallIndex] + 200.0f,
				m_Heights[j + 2][heightWallIndex], m_Heights[j + 2][heightWallIndex] + 700.0f, m_Heights[j + 2][heightWallIndex] + 700.0f, m_Heights[j + 2][heightWallIndex] + 500.0f, m_Heights[j + 2][heightWallIndex] + 200.0f,
				m_Heights[j + 1][heightWallIndex], m_Heights[j + 1][heightWallIndex] + 700.0f, m_Heights[j + 1][heightWallIndex] + 700.0f, m_Heights[j + 1][heightWallIndex] + 500.0f, m_Heights[j + 1][heightWallIndex] + 200.0f,
				m_Heights[j + 0][heightWallIndex], m_Heights[j + 0][heightWallIndex] + 700.0f, m_Heights[j + 0][heightWallIndex] + 700.0f, m_Heights[j + 0][heightWallIndex] + 500.0f, m_Heights[j + 0][heightWallIndex] + 200.0f };
		 
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList, 
			{ 100.0f, 0, 1.0f * j },
			textureInfo,
			heightsTemp,
			m_Normals,
			100, j);  
	}
	for (int j = 72; j < 100; j += 4)
	{ 
		textureInfo = (0x20);

		int heightsTemp[25] = {
				m_Heights[j + 4][heightWallIndex], m_Heights[j + 4][heightWallIndex] + 700.0f, m_Heights[j + 4][heightWallIndex] + 1500.0f, m_Heights[j + 4][heightWallIndex] + 500.0f, m_Heights[j + 4][heightWallIndex] + 200.0f,
				m_Heights[j + 3][heightWallIndex], m_Heights[j + 3][heightWallIndex] + 700.0f, m_Heights[j + 3][heightWallIndex] + 1500.0f, m_Heights[j + 3][heightWallIndex] + 500.0f, m_Heights[j + 3][heightWallIndex] + 200.0f,
				m_Heights[j + 2][heightWallIndex], m_Heights[j + 2][heightWallIndex] + 700.0f, m_Heights[j + 2][heightWallIndex] + 1500.0f, m_Heights[j + 2][heightWallIndex] + 500.0f, m_Heights[j + 2][heightWallIndex] + 200.0f,
				m_Heights[j + 1][heightWallIndex], m_Heights[j + 1][heightWallIndex] + 700.0f, m_Heights[j + 1][heightWallIndex] + 1500.0f, m_Heights[j + 1][heightWallIndex] + 500.0f, m_Heights[j + 1][heightWallIndex] + 200.0f,
				m_Heights[j + 0][heightWallIndex], m_Heights[j + 0][heightWallIndex] + 700.0f, m_Heights[j + 0][heightWallIndex] + 1500.0f, m_Heights[j + 0][heightWallIndex] + 500.0f, m_Heights[j + 0][heightWallIndex] + 200.0f };
		  
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList, 
			{ 100.0f, 0, 1.0f * j },
			textureInfo,
			heightsTemp,
			m_Normals,
			100, j);   
	}
	{
		int j = 72; 
		textureInfo = (0x20);

		int heightsTemp[25] = {
				m_Heights[j + 0][heightWallIndex], m_Heights[j + 0][heightWallIndex] + 700.0f, m_Heights[j + 0][heightWallIndex] + 1500.0f, m_Heights[j + 0][heightWallIndex] + 500.0f, m_Heights[j + 0][heightWallIndex] + 200.0f,
				m_Heights[j + 1][heightWallIndex], m_Heights[j + 1][heightWallIndex] + 700.0f, m_Heights[j + 1][heightWallIndex] + 1500.0f, m_Heights[j + 1][heightWallIndex] + 500.0f, m_Heights[j + 1][heightWallIndex] + 200.0f,
				m_Heights[j + 2][heightWallIndex], m_Heights[j + 2][heightWallIndex] + 700.0f, m_Heights[j + 2][heightWallIndex] + 1500.0f, m_Heights[j + 2][heightWallIndex] + 500.0f, m_Heights[j + 2][heightWallIndex] + 200.0f,
				m_Heights[j + 3][heightWallIndex], m_Heights[j + 3][heightWallIndex] + 700.0f, m_Heights[j + 3][heightWallIndex] + 1500.0f, m_Heights[j + 3][heightWallIndex] + 500.0f, m_Heights[j + 3][heightWallIndex] + 200.0f,
				m_Heights[j + 4][heightWallIndex], m_Heights[j + 4][heightWallIndex] + 700.0f, m_Heights[j + 4][heightWallIndex] + 1500.0f, m_Heights[j + 4][heightWallIndex] + 500.0f, m_Heights[j + 4][heightWallIndex] + 200.0f };
		 
		m_BindTerrainMesh->CreateWallMesh(pd3dDevice, pd3dCommandList,
			{ 100.0f, 0, 1.0f * j - 4.0f }, 
			textureInfo,
			heightsTemp,
			m_Normals,
			100, j); 
	}
}

void CTerrain::ReviseLoosedTextureWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	CShader* pShader)
{
	BYTE textureInfo = 0x00;
	int tolerance = 30; 
	for (int i = 0; i < 25; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			// 고저 차에 의헤 늘어진 텍스처 보완 코드.
			if (j == 17 && (i >= 17 && i <= 24) ||
				(j == 19) && (i >= 10 && i < 17) ||
				(j == 22) && (i >= 10 && i < 17))
			{
				tolerance = 30;
				if ((j == 22) && (i >= 10 && i <= 17)) {
					tolerance = -15;
				}

				int heightsGap[5] = {
					m_Heights[i * 4 + 4][(j + 1) * 4] - m_Heights[i * 4 + 4][j * 4] - tolerance,
					m_Heights[i * 4 + 3][(j + 1) * 4] - m_Heights[i * 4 + 3][j * 4] - tolerance,
					m_Heights[i * 4 + 2][(j + 1) * 4] - m_Heights[i * 4 + 2][j * 4] - tolerance,
					m_Heights[i * 4 + 1][(j + 1) * 4] - m_Heights[i * 4 + 1][j * 4] - tolerance,
					m_Heights[i * 4][(j + 1) * 4] - m_Heights[i * 4][j * 4] - tolerance
				};
				if (j == 17 && i == 17)
				{
					heightsGap[0] = m_Heights[i * 4 + 4][j * 4] - 4000;
					heightsGap[1] = m_Heights[i * 4 + 3][j * 4] - 4000;
					heightsGap[2] = m_Heights[i * 4 + 2][j * 4] - 4000;
					heightsGap[3] = m_Heights[i * 4 + 1][j * 4] - 4000;
					heightsGap[4] = m_Heights[i * 4][j * 4] - 4000;
				}
				for (int k = 0; k < 5; ++k)
				{
					int heightsTemp[25] =
					{
						m_Heights[i * 4 + 4][j * 4] + (heightsGap[0] / 20) * (k * 4),
						m_Heights[i * 4 + 4][j * 4] + (heightsGap[0] / 20) * (k * 4 + 1),
						m_Heights[i * 4 + 4][j * 4] + (heightsGap[0] / 20) * (k * 4 + 2),
						m_Heights[i * 4 + 4][j * 4] + (heightsGap[0] / 20) * (k * 4 + 3),
						m_Heights[i * 4 + 4][j * 4] + (heightsGap[0] / 20) * (k * 4 + 4),

						m_Heights[i * 4 + 3][j * 4] + (heightsGap[1] / 20) * (k * 4),
						m_Heights[i * 4 + 3][j * 4] + (heightsGap[1] / 20) * (k * 4 + 1),
						m_Heights[i * 4 + 3][j * 4] + (heightsGap[1] / 20) * (k * 4 + 2),
						m_Heights[i * 4 + 3][j * 4] + (heightsGap[1] / 20) * (k * 4 + 3),
						m_Heights[i * 4 + 3][j * 4] + (heightsGap[1] / 20) * (k * 4 + 4),

						m_Heights[i * 4 + 2][j * 4] + (heightsGap[2] / 20) * (k * 4),
						m_Heights[i * 4 + 2][j * 4] + (heightsGap[2] / 20) * (k * 4 + 1),
						m_Heights[i * 4 + 2][j * 4] + (heightsGap[2] / 20) * (k * 4 + 2),
						m_Heights[i * 4 + 2][j * 4] + (heightsGap[2] / 20) * (k * 4 + 3),
						m_Heights[i * 4 + 2][j * 4] + (heightsGap[2] / 20) * (k * 4 + 4),

						m_Heights[i * 4 + 1][j * 4] + (heightsGap[3] / 20) * (k * 4),
						m_Heights[i * 4 + 1][j * 4] + (heightsGap[3] / 20) * (k * 4 + 1),
						m_Heights[i * 4 + 1][j * 4] + (heightsGap[3] / 20) * (k * 4 + 2),
						m_Heights[i * 4 + 1][j * 4] + (heightsGap[3] / 20) * (k * 4 + 3),
						m_Heights[i * 4 + 1][j * 4] + (heightsGap[3] / 20) * (k * 4 + 4),

						m_Heights[i * 4][j * 4] + (heightsGap[4] / 20) * (k * 4),
						m_Heights[i * 4][j * 4] + (heightsGap[4] / 20) * (k * 4 + 1),
						m_Heights[i * 4][j * 4] + (heightsGap[4] / 20) * (k * 4 + 2),
						m_Heights[i * 4][j * 4] + (heightsGap[4] / 20) * (k * 4 + 3),
						m_Heights[i * 4][j * 4] + (heightsGap[4] / 20) * (k * 4 + 4),
					};
					textureInfo = (0x20);
					m_BindTerrainMeshForLoosedWall[0]->CreateWallMesh(pd3dDevice, pd3dCommandList,
						{ 20.0f * j + 4.0f * k, 0, 4.0f * i }, 
						textureInfo,
						heightsTemp,							
						m_Normals,
						j, i); 
					//pObject->Scale(40.0f, 1.0f, 200.0f				}
					if (j != 17 ) continue;
				}
				if ((j == 24 || j == 23 || j == 18 || j == 17) && i == 17) {
					tolerance = 10;
					int heightsGap[5] = {
						m_Heights[(i + 1) * 4][j * 4] - m_Heights[i * 4][j * 4] - tolerance,
						m_Heights[(i + 1) * 4][j * 4 + 1] - m_Heights[i * 4][j * 4 + 1] - tolerance,
						m_Heights[(i + 1) * 4][j * 4 + 2] - m_Heights[i * 4][j * 4 + 2] - tolerance,
						m_Heights[(i + 1) * 4][j * 4 + 3] - m_Heights[i * 4][j * 4 + 3] - tolerance,
						m_Heights[(i + 1) * 4][j * 4 + 4] - m_Heights[i * 4][j * 4 + 4] - tolerance
					};
					if (j == 17)
					{
						heightsGap[0] = m_Heights[i * 4][j * 4] - 4000;
						heightsGap[1] = m_Heights[i * 4][j * 4 + 1] - 4000;
						heightsGap[2] = m_Heights[i * 4][j * 4 + 2] - 4000;
						heightsGap[3] = m_Heights[i * 4][j * 4 + 3] - 4000;
						heightsGap[4] = m_Heights[i * 4][j * 4 + 4] - 4000;
					}
					for (int k = 0; k < 5; ++k)
					{
						int heightsTemp[25] =
						{
							m_Heights[i * 4][j * 4 + 0] + (heightsGap[0] / 20) * (k * 4 + 4),
							m_Heights[i * 4][j * 4 + 1] + (heightsGap[1] / 20) * (k * 4 + 4),
							m_Heights[i * 4][j * 4 + 2] + (heightsGap[2] / 20) * (k * 4 + 4),
							m_Heights[i * 4][j * 4 + 3] + (heightsGap[3] / 20) * (k * 4 + 4),
							m_Heights[i * 4][j * 4 + 4] + (heightsGap[4] / 20) * (k * 4 + 4),

							m_Heights[i * 4][j * 4 + 0] + (heightsGap[0] / 20) * (k * 4 + 3),
							m_Heights[i * 4][j * 4 + 1] + (heightsGap[1] / 20) * (k * 4 + 3),
							m_Heights[i * 4][j * 4 + 2] + (heightsGap[2] / 20) * (k * 4 + 3),
							m_Heights[i * 4][j * 4 + 3] + (heightsGap[3] / 20) * (k * 4 + 3),
							m_Heights[i * 4][j * 4 + 4] + (heightsGap[4] / 20) * (k * 4 + 3),

							m_Heights[i * 4][j * 4 + 0] + (heightsGap[0] / 20) * (k * 4 + 2),
							m_Heights[i * 4][j * 4 + 1] + (heightsGap[1] / 20) * (k * 4 + 2),
							m_Heights[i * 4][j * 4 + 2] + (heightsGap[2] / 20) * (k * 4 + 2),
							m_Heights[i * 4][j * 4 + 3] + (heightsGap[3] / 20) * (k * 4 + 2),
							m_Heights[i * 4][j * 4 + 4] + (heightsGap[4] / 20) * (k * 4 + 2),

							m_Heights[i * 4][j * 4 + 0] + (heightsGap[0] / 20) * (k * 4 + 1),
							m_Heights[i * 4][j * 4 + 1] + (heightsGap[1] / 20) * (k * 4 + 1),
							m_Heights[i * 4][j * 4 + 2] + (heightsGap[2] / 20) * (k * 4 + 1),
							m_Heights[i * 4][j * 4 + 3] + (heightsGap[3] / 20) * (k * 4 + 1),
							m_Heights[i * 4][j * 4 + 4] + (heightsGap[4] / 20) * (k * 4 + 1),

							m_Heights[i * 4][j * 4 + 0] + (heightsGap[0] / 20) * (k * 4),
							m_Heights[i * 4][j * 4 + 1] + (heightsGap[1] / 20) * (k * 4),
							m_Heights[i * 4][j * 4 + 2] + (heightsGap[2] / 20) * (k * 4),
							m_Heights[i * 4][j * 4 + 3] + (heightsGap[3] / 20) * (k * 4),
							m_Heights[i * 4][j * 4 + 4] + (heightsGap[4] / 20) * (k * 4),
						};	

						textureInfo = (0x20);
						if (i == 10)textureInfo = (0x08);
						m_BindTerrainMeshForLoosedWall[1]->CreateWallMesh(pd3dDevice, pd3dCommandList,
							{ 4.0f * j , 0, 20.0f * i + 4.0f * k }, 
							textureInfo,
							heightsTemp,
							m_Normals,
							j, i); 
					}
				}
			}
		}
	} 
}

void CTerrain::InitNormals()
{
	for (int i = 0; i <= TERRAIN_HEIGHT_MAP_HEIGHT; ++i) {
		for (int j = 0; j <= TERRAIN_HEIGHT_MAP_WIDTH; ++j) {
			m_Normals[i][j] = GetHeightMapNormal(j, i);
		}
	} 
}

XMFLOAT3 CTerrain::GetHeightMapNormal(int x, int z)
{
	XMFLOAT3 xmf3Scale = { 200.0f, 1.0f, 200.0f };
	if ((x < 0.0f) || (z < 0.0f) || (x > TERRAIN_HEIGHT_MAP_WIDTH) || (z > TERRAIN_HEIGHT_MAP_HEIGHT))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int xHeightMapAdd = (x <= (TERRAIN_HEIGHT_MAP_WIDTH)) ? 1 : -1;
	int zHeightMapAdd = (z <= (TERRAIN_HEIGHT_MAP_HEIGHT)) ? 1 : -1;

	float y1 = (float)m_Heights[z][x] * xmf3Scale.y;
	float y2 = (float)m_Heights[z][x + xHeightMapAdd] * xmf3Scale.y;
	float y3 = (float)m_Heights[z + zHeightMapAdd][x] * xmf3Scale.y;

	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(xmf3Scale.x, y2 - y1, 0.0f);
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);  
}

void CTerrain::InitHeightDatas()
{
	//for (int Sero = 0; Sero <= TERRAIN_HEIGHT_MAP_HEIGHT; ++Sero)
	//{
	//	for (int j = 0; j <= TERRAIN_HEIGHT_MAP_WIDTH; ++j)
	//	{
	//		m_Heights[Sero][j] = rand() % 300 - 150;
	//	}
	//}

	//// 첫번째 지형및 구조물
	//for (int i = 0; i < 15; i++)
	//{
	//	for (int Garo = 0; Garo < 15; Garo++)
	//	{
	//		m_Heights[i][Garo] = 100;
	//	}
	//}

	//for (int i = 7; i < 15; i++)
	//{
	//	for (int Garo = 5; Garo < 15; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 500 + (Garo * Garo *2);
	//	}
	//}

	//for (int i = 0; i < 5; i++)
	//{
	//	for (int Garo = 15; Garo < 25; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 500 + (Garo * Garo * 2);
	//	}
	//}

	//for (int i = 5; i < 15; i++)
	//{
	//	for (int Garo = 20; Garo < 25; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 500 + (Garo * Garo * 2);
	//	}
	//}


	//for (int i = 15; i < 25; i++)
	//{
	//	for (int Garo = 0; Garo < 5; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 500 - (i * i );
	//	}
	//}
	// 
	//for (int i = 25; i < 35; i++)
	//{
	//	for (int Garo = 15; Garo < 20; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i*20);
	//	}
	//}

	//for (int i = 25; i < 35; i++)
	//{
	//	for (int Garo = 0; Garo < 7; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i * 20);
	//	}
	//}

	//for (int i = 0; i < 35; i++)
	//{
	//	for (int Garo = 30; Garo < 40; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i * 30);
	//	}
	//}

	//for (int i = 40; i < 50; i++)
	//{
	//	for (int Garo = 20; Garo < 30; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i * 30);
	//	}
	//}

	//for (int i = 57; i < 62; i++)
	//{
	//	for (int Garo = 20; Garo < 27; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i * 30)-1300.0f;
	//	}
	//}

	//for (int i = 65; i < 72; i++)
	//{
	//	for (int Garo = 20; Garo < 27; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i * 30) -1300.0f;
	//	}
	//}

	//for (int i = 55; i < 65; i++)
	//{
	//	for (int Garo = 40; Garo < 50; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i * 30)-1700.0f;
	//	}
	//}

	//for (int i = 62; i < 65; i++)
	//{
	//	for (int Garo = 0; Garo < 10; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 + (i * 30) - 2000.0f;
	//	}
	//}

	//for (int i = 25; i < 30; i++)
	//{
	//	for (int Garo = 45; Garo < 50; Garo++)
	//	{
	//		//m_Heights[i][Garo] = rand() % 300 - (i * 30)-500.0f;
	//	}
	//}
	//
	//// 두번째 지형및 구조물
	//for (int i = 80; i < 100; i++)
	//{
	//	for (int Garo = 0; Garo < 50; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 1150;
	//	}
	//}
	//for (int i = 80; i < 100; i++)
	//{
	//	for (int Garo = 40; Garo < 50; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 1750;
	//	}
	//}

	//for (int i = 0; i < 80; i++)
	//{
	//	for (int Garo = 50; Garo < 70; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 2150;
	//	}
	//}
	//for (int i = 0; i < 80; i++)
	//{
	//	for (int Garo = 50; Garo < 70; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 2150;
	//	}
	//}

	//// 구덩이
	//for (int i = 95; i < 100; i++)
	//{
	//	for (int Garo = 10; Garo < 15; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 1650;
	//	}
	//}

	//for (int i = 85; i < 90; i++)
	//{
	//	for (int Garo = 7; Garo < 12; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 1650;
	//	}
	//}

	//for (int i = 80; i < 85; i++)
	//{
	//	for (int Garo = 22; Garo < 27; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 1650;
	//	}
	//}

	//for (int i = 95; i < 100; i++)
	//{
	//	for (int Garo = 27; Garo < 32; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 1650;
	//	}
	//}

	//
	//// 세번째 지형및 구조물
	//for (int i = 80; i < 100; i++)
	//{
	//	for (int Garo = 50; Garo < 70; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 1150;
	//	}
	//}
	//for (int i = 0; i < 20; i++)
	//{
	//	for (int Garo = 50; Garo < 70; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 3650;
	//	}
	//}
	// 
	//// 4번째~보스
	//for (int i = 0; i < 20; i++)
	//{
	//	for (int Garo = 70; Garo < 100; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 3650;
	//	}
	//}
	//for (int i = 20; i < 40; i++)
	//{
	//	for (int Garo = 70; Garo < 100; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 3650;
	//	}
	//}
	//for (int i = 40; i < 70; i++)
	//{
	//	for (int Garo = 70; Garo < 100; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 2550;
	//		if (Garo >= 78 && Garo <= 88)
	//		{
	//			m_Heights[i][Garo] = rand() % 300 - 4650;
	//			for(int i= 45; i<= 70; i++)
	//			{
	//				m_Heights[i][Garo] = -(100*i);
	//			}
	//		}
	//	}
	//}
	//for (int i = 69; i < 100; i++)
	//{
	//	for (int Garo = 70; Garo < 100; Garo++)
	//	{
	//		m_Heights[i][Garo] = rand() % 300 - 7150;
	//	}
	//} 

	//FileSave();
	FileRead();
}
void CTerrain::FileSave()
{
	ofstream fileOut("Heights.txt");
	int indexX, indexY;
	for (int i = 0; i <= TERRAIN_HEIGHT_MAP_HEIGHT; ++i)
	{
		for (int j = 0; j <= TERRAIN_HEIGHT_MAP_WIDTH; ++j)
		{ 
			indexX = j, indexY = i;
			if (j == TERRAIN_HEIGHT_MAP_WIDTH) { 
				indexX = j - 1;
			}
			else if (i == TERRAIN_HEIGHT_MAP_HEIGHT) {
				indexY = i - 1;
			}
			fileOut << m_Heights[indexY][indexX] << "\t";
			
			if (((j + 1) % 5 == 0 && (j != 0 && j != 100)))
				fileOut << "//\t";
		}
		fileOut << endl;
		if (((i + 1) % 5 == 0 && (i != 0 && i != 100)))
		{
			fileOut << "\n\n";
		}

		fileOut << endl;
	}
}

void CTerrain::FileRead()
{
	ifstream fileIn("resources/Heights.txt");
	for (int i = 0; i <= TERRAIN_HEIGHT_MAP_HEIGHT; ++i)
	{
		for (int j = 0; j <= TERRAIN_HEIGHT_MAP_WIDTH; ++j)
		{
			string text;
			fileIn >> text;
			if (text.compare("//") == 0)
			{
				j--;
				continue;
			} 
			m_Heights[i][j] = stoi(text);
		} 
	}
}