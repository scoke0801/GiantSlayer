#include "stdafx.h"
#include "Terrain.h"
CTerrain::CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	int nWidth, int nLength,
	int nBlockWidth, int nBlockLength, CShader* pShader)
{
#pragma region For Hide
	m_nWidth = nWidth;			// 257
	m_nLength = nLength;		// 257

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;	// 32
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;	// 32

	m_nObjects = cxBlocks * czBlocks;
	m_Objects.reserve(m_nObjects);

	CTerrainMesh* pTerrainPlaneMesh = new CTerrainMesh(pd3dDevice, pd3dCommandList,
		0, 0, nBlockWidth, nBlockLength, cxBlocks, czBlocks, MapMeshHeightType::Plane);
	CTerrainMesh* pTerrainUpRidgeMesh = new CTerrainMesh(pd3dDevice, pd3dCommandList,
		0, 0, nBlockWidth, nBlockLength, cxBlocks, czBlocks, MapMeshHeightType::UpRidge);
	CTerrainMesh* pTerrainDownRidgeMesh = new CTerrainMesh(pd3dDevice, pd3dCommandList,
		0, 0, nBlockWidth, nBlockLength, cxBlocks, czBlocks, MapMeshHeightType::DownRidge);

	MapData Map[10][10];

	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			// 첫번째 지형
			if (i == 0 || i == 1)
			{
				if (j == 0 || j == 1)
				{
					Map[i][j].Name = Map_Configuration_Name::First_Ground;
					Map[i][j].Height = 0;
				}
				if (j == 2)
				{
					Map[i][j].Name = Map_Configuration_Name::Up_Pattern_Ground_Rotate;
					Map[i][j].Height = 0;
				}
				if (j == 3)
				{
					Map[i][j].Name = Map_Configuration_Name::Down_Pattern_Ground_Rotate;
					Map[i][j].Height = 0;
				}
				if (j == 4)
				{
					Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
					Map[i][j].Height = 0;
				}
				if (j == 5)
				{
					Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
					Map[i][j].Height = 0;
				}
				if (j == 6)
				{
					Map[i][j].Name = Map_Configuration_Name::Up_Pattern_Ground_Rotate;
					Map[i][j].Height = 0;
				}
				if (j == 7)
				{
					Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
					Map[i][j].Height = 0;
				}
			}
			else if (i == 2 || i == 3)
			{
				Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
				Map[i][j].Height = 1000;
			}


			// 두번째 지형
			if (j == 9 || j == 8)
			{
				if (i == 0 || i == 1)
				{
					Map[i][j].Name = Map_Configuration_Name::Second_Ground;
					Map[i][j].Height = -1000;
				}
				if (i == 2)
				{
					Map[i][j].Name = Map_Configuration_Name::Up_Pattern_Ground;
					Map[i][j].Height = -1000;
				}
				if (i == 3)
				{
					Map[i][j].Name = Map_Configuration_Name::Cliff_Pattern_Ground_Rotate;
					Map[i][j].Height = -1000;
				}
			}

			// 세번째 지형
			if (i == 4 || i == 5)
			{
				if (j == 8 || j == 9)
				{
					Map[i][j].Name = Map_Configuration_Name::Third_Ground;
					Map[i][j].Height = -2000;
				}
				if (j == 7)
				{
					Map[i][j].Name = Map_Configuration_Name::Down_Pattern_Ground_Rotate;
					Map[i][j].Height = -2000;
				}
				if (j == 6)
				{
					Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
					Map[i][j].Height = -2000;
				}
				if (j == 5)
				{
					Map[i][j].Name = Map_Configuration_Name::Up_Pattern_Ground_Rotate;
					Map[i][j].Height = -2000;
				}
				if (j == 4)
				{
					Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
					Map[i][j].Height = -2000;
				}
				if (j == 3)
				{
					Map[i][j].Name = Map_Configuration_Name::Down_Pattern_Ground_Rotate;
					Map[i][j].Height = -2000;
				}
				if (j == 2)
				{
					Map[i][j].Name = Map_Configuration_Name::Cliff_Pattern_Ground_Vertical;
					Map[i][j].Height = -2000;
				}
			}
			else if (i == 6)
			{
				Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
				Map[i][j].Height = -1000;
			}
			// 네번째 지형
			if (i >= 4 && i <= 9)
			{
				if (j == 0 || j == 1)
				{
					Map[i][j].Name = Map_Configuration_Name::Fourth_Ground;
					Map[i][j].Height = -3000;
				}
			}
			// 보스 지형
			if (i >= 7 && i <= 9)
			{
				if (j >= 7 && j <= 9)
				{
					Map[i][j].Name = Map_Configuration_Name::Boss_Ground;
					Map[i][j].Height = -6000;
				}
				if (j == 2)
				{
					Map[i][j].Name = Map_Configuration_Name::Cliff_Pattern_Ground;
					Map[i][j].Height = -3000;
				}
				if (j == 3)
				{
					Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
					Map[i][j].Height = -4000;
				}
				if (j == 4)
				{
					Map[i][j].Name = Map_Configuration_Name::Cliff_Pattern_Ground;
					Map[i][j].Height = -4000;
				}
				if (j == 5)
				{
					Map[i][j].Name = Map_Configuration_Name::Plane_Pattern_Ground;
					Map[i][j].Height = -5000;
				}
				if (j == 6)
				{
					Map[i][j].Name = Map_Configuration_Name::Cliff_Pattern_Ground;
					Map[i][j].Height = -5000;
				}
			}

		}

	}
	// 첫 섬 지형
	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			CGameObject* pObject = new CGameObject();
			pObject->SetTextureIndex(0x01);
			pObject->SetShader(pShader);
			// 평면
			if (Map[i][j].Name == Map_Configuration_Name::First_Ground)
			{
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}

			if (Map[i][j].Height == -1000)
			{
				pObject->SetTextureIndex(0x02);
			}
			if (Map[i][j].Height == -1000 && Map[i][j].Name == Map_Configuration_Name::Plane_Pattern_Ground)
			{
				pObject->SetTextureIndex(0x04);
			}

			if (Map[i][j].Height == -2000)
			{
				pObject->SetTextureIndex(0x04);
			}
			if (Map[i][j].Height == -3000 || Map[i][j].Height == -4000)
			{
				pObject->SetTextureIndex(0x08);
			}
			if (Map[i][j].Height == -5000 || Map[i][j].Height == -6000)
			{
				pObject->SetTextureIndex(0x010);
			}

			if (Map[i][j].Name == Map_Configuration_Name::Second_Ground)
			{
				pObject->SetMesh(pTerrainPlaneMesh);

				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Third_Ground)
			{
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Fourth_Ground)
			{
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Boss_Ground)
			{
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Up_Pattern_Ground)
			{
				pObject->SetMesh(pTerrainUpRidgeMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Down_Pattern_Ground)
			{
				pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
				pObject->SetMesh(pTerrainUpRidgeMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f + 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Cliff_Pattern_Ground)
			{
				pObject->Rotate(XMFLOAT3(1, 0, 0), 25);
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 600.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}

			if (Map[i][j].Name == Map_Configuration_Name::Up_Pattern_Ground_Rotate)
			{
				pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
				pObject->SetMesh(pTerrainUpRidgeMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f + 2000.0f));
			}

			if (Map[i][j].Name == Map_Configuration_Name::Down_Pattern_Ground_Rotate)
			{
				pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
				pObject->SetMesh(pTerrainDownRidgeMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f + 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Cliff_Pattern_Ground_Rotate)
			{
				pObject->Rotate(XMFLOAT3(0, 1, 0), 90);
				pObject->Rotate(XMFLOAT3(1, 0, 0), 25);
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 600.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f + 2000.0f));
			}
			if (Map[i][j].Name == Map_Configuration_Name::Cliff_Pattern_Ground_Vertical)
			{

				pObject->Rotate(XMFLOAT3(0, 1, 0), 180);
				pObject->Rotate(XMFLOAT3(1, 0, 0), 25);
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 600.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f + 2000.0f, Map[i][j].Height, j * 2000.0f + 2000.0f));
			}

			if (Map[i][j].Name == Map_Configuration_Name::Plane_Pattern_Ground)
			{
				pObject->SetMesh(pTerrainPlaneMesh);
				pObject->Scale(500.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3(i * 2000.0f, Map[i][j].Height, j * 2000.0f));
			}
			m_Objects.push_back(std::move(pObject));
		}
	}

	// 세로 벽
	for (int z = 0; z < 8; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x01);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(400.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(4000.0f, -600.0f, 0.0f + (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}

	for (int z = 0; z < 8; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x01);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(400.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(4000.0f, -2200.0f, 0.0f + (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}

	// 
	for (int z = 0; z < 8; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x04);
		if (z < 2)
			pObject->SetTextureIndex(0x08);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(8000.0f, -1000.0f, 0.0f + (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}
	for (int z = 0; z < 8; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x04);
		if (z < 2)
			pObject->SetTextureIndex(0x08);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(8000.0f, -3000.0f, 0.0f + (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}

	for (int z = 2; z < 10; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x04);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(12000.0f, -3000.0f, 0.0f + (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}

	for (int z = 2; z < 10; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x08);
		if (z > 4)
			pObject->SetTextureIndex(0x010);

		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(600.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(14000.0f, -3400.0f, 0.0f + (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}


	for (int z = 2; z < 10; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x08);
		if (z > 4)
			pObject->SetTextureIndex(0x010);

		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(650.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(14000.0f, -6000.0f, 0.0f + (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}

	// 가로 벽 
	// 첫번째~두번째 사이 지형 위쪽 작은가로벽 
	for (int z = 1; z < 3; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x02);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(2000.0f + (2000.0f * z), -1000.0f, 16000.0f));
		m_Objects.push_back(std::move(pObject));
	}
	for (int z = 1; z < 3; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x02);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3(2000.0f + (2000.0f * z), 1000.0f, 16000.0f));
		m_Objects.push_back(std::move(pObject));
	}
	// 가로벽
	// 3번째 ~ 4번째(메마른지형) 사이 지형 작은가로 벽
	for (int z = 5; z < 6; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x08);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 1000.0f);

		pObject->SetPosition(XMFLOAT3(2000.0f + (2000.0f * z), -1000.0f, 4000.0f));
		m_Objects.push_back(std::move(pObject));
	}

	////////////////////////////////////////////////////////////////////////////////////////

	// 위쪽 벽
	// 두번째 지형 ~ 세번째 지형 가로로 된 벽

	for (int z = 0; z < 10; z++)
	{
		if (z < 3)
		{
			CGameObject* pObject = new CGameObject();
			pObject->SetTextureIndex(0x02);
			pObject->SetShader(pShader);
			pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
			pObject->SetMesh(pTerrainPlaneMesh);
			pObject->Scale(500.0f, 1.0f, 500.0f);

			pObject->SetPosition(XMFLOAT3((0 + (2000.0f * z)), 1000.0f, 20000.0f));
			m_Objects.push_back(std::move(pObject));
		}

		if (z >= 3 && z < 7)
		{
			CGameObject* pObject = new CGameObject();
			pObject->SetTextureIndex(0x04);
			if (z == 3)
				pObject->SetTextureIndex(0x02);
			pObject->SetShader(pShader);
			pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
			pObject->SetMesh(pTerrainPlaneMesh);
			pObject->Scale(500.0f, 1.0f, 500.0f);

			pObject->SetPosition(XMFLOAT3((0 + (2000.0f * z)), 0000.0f, 20000.0f));
			m_Objects.push_back(std::move(pObject));
		}
		if (z >= 7)
		{
			CGameObject* pObject = new CGameObject();
			pObject->SetTextureIndex(0x010);
			pObject->SetShader(pShader);
			pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
			pObject->SetMesh(pTerrainPlaneMesh);
			pObject->Scale(500.0f, 1.0f, 500.0f);

			pObject->SetPosition(XMFLOAT3((0 + (2000.0f * z)), -4000.0f, 20000.0f));
			m_Objects.push_back(std::move(pObject));
		}
	}


	// 아래쪽 벽
	// 세번째 지형 ~ 네번째 지형  가로로 된 벽

	for (int z = 0; z < 10; z++)
	{
		if (z < 4)
		{
			CGameObject* pObject = new CGameObject();
			pObject->SetTextureIndex(0x01);
			pObject->SetShader(pShader);
			pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
			pObject->SetMesh(pTerrainPlaneMesh);
			pObject->Scale(500.0f, 1.0f, 500.0f);

			pObject->SetPosition(XMFLOAT3((0 + (2000.0f * z)), 1000.0f, 0000.0f));
			m_Objects.push_back(std::move(pObject));
		}
		if (z >= 4)
		{
			CGameObject* pObject = new CGameObject();
			pObject->SetTextureIndex(0x08);
			pObject->SetShader(pShader);
			pObject->Rotate(XMFLOAT3(1, 0, 0), 90);
			pObject->SetMesh(pTerrainPlaneMesh);
			pObject->Scale(500.0f, 1.0f, 500.0f);

			pObject->SetPosition(XMFLOAT3((0 + (2000.0f * z)), -1000.0f, 0000.0f));
			m_Objects.push_back(std::move(pObject));
		}
	}

	// 왼쪽 벽
	// 첫번째 지형 ~ 두번째 지형 세로로 된 벽

	for (int z = 0; z < 10; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x01);
		if (z > 7)
			pObject->SetTextureIndex(0x02);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 500.0f);

		pObject->SetPosition(XMFLOAT3((0), -1000.0f, (2000.0f * z)));
		m_Objects.push_back(std::move(pObject));
	}

	// 오른쪽 벽
	// 4번째 지형 ~ 보스 지형 세로로 된 벽

	for (int z = 0; z < 10; z++)
	{
		CGameObject* pObject = new CGameObject();
		pObject->SetTextureIndex(0x010);
		pObject->SetShader(pShader);
		pObject->Rotate(XMFLOAT3(0, 0, 1), 90);
		pObject->SetMesh(pTerrainPlaneMesh);
		pObject->Scale(500.0f, 1.0f, 500.0f);
		if (z < 2)
		{
			pObject->SetTextureIndex(0x08);
			pObject->SetPosition(XMFLOAT3((20000.0f), -3000.0f, (2000.0f * z)));
		}
		else if (z == 2)
		{
			pObject->SetTextureIndex(0x08);
			pObject->SetPosition(XMFLOAT3((20000.0f), -4000.0f, (2000.0f * z)));
		}
		else if (z == 3)
		{
			pObject->SetTextureIndex(0x08);
			pObject->SetPosition(XMFLOAT3((20000.0f), -5000.0f, (2000.0f * z)));
		}
		else if (z <= 4)
		{
			pObject->SetTextureIndex(0x08);
			pObject->SetPosition(XMFLOAT3((20000.0f), -5000.0f, (2000.0f * z)));
		}
		else if (z <= 5)
		{
			pObject->SetPosition(XMFLOAT3((20000.0f), -6000.0f, (2000.0f * z)));
		}
		else if (z > 5)
		{
			pObject->SetPosition(XMFLOAT3((20000.0f), -6000.0f, (2000.0f * z)));
		}
		m_Objects.push_back(std::move(pObject));

	}

	// 20000 확인용 평지
	CGameObject* pObject = new CGameObject();
	pObject->Scale(5000.0f, 1.0f, 5000.0f);
	pObject->SetTextureIndex(0x01);
	pObject->SetMesh(pTerrainPlaneMesh);
	pObject->SetShader(pShader);

	pObject->SetPosition(XMFLOAT3(0.0f, -10000.0f, 0.0f));
	m_Objects.push_back(std::move(pObject));
#pragma endregion


}

CTerrain::CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	CShader* pShader)
{
	InitHeightDatas();

	CGameObject* pObject = new CGameObject();
	pObject->SetTextureIndex(0x01);
	pObject->SetShader(pShader);
	   
	for (int i = 0; i < 25; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			pObject = new CGameObject();
			pObject->SetTextureIndex(0x01);
			pObject->SetShader(pShader);
			pObject->SetMesh(new CTerrainMesh(pd3dDevice, pd3dCommandList,
				4 * j, 4 * i,
				m_Heights));
	
			pObject->Scale(200.0f, 1.0f, 200.0f);
			pObject->SetPosition({ 800.0f * j, 0, 800.0f * i });
			m_Objects.push_back(std::move(pObject));
		} 
	} 
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

void CTerrain::InitHeightDatas()
{
	int count = 0;
	for (int Sero = 0; Sero <= TERRAIN_HEIGHT_MAP_HEIGHT; ++Sero)
	{
		for (int j = 0; j <= TERRAIN_HEIGHT_MAP_WIDTH; ++j)
		{
			m_Heights[Sero][j] = rand() % 300 - 150;
			
			if (j <= 50 && Sero <= 50)
			{
				m_Heights[Sero][j] = rand() % 300 - 350;
			}
			if (Sero > 80 && j < 50)
			{
				m_Heights[Sero][j] = rand() % 300 - 1150;
			}
			if (Sero > 20 && (j > 50 && j < 70))
			{
				m_Heights[Sero][j] = rand() % 300 - 2150;
			}
			if (Sero <= 20 && (j > 50 && j < 70))
			{
				m_Heights[Sero][j] = rand() % 300 - 3150;
			}
			if (Sero <= 20 && j >= 70)
			{
				m_Heights[Sero][j] = rand() % 300 - 3150;
			}
			if (Sero > 20 && j >= 70)
			{
				m_Heights[Sero][j] = rand() % 300 - 4150;
			}

			

		}
	}
}