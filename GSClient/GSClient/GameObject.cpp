#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

string ConvertToObjectName(const OBJ_NAME& name)
{
	switch (name)
	{
	case OBJ_NAME::None:
		return "None";
	case OBJ_NAME::Box:
		return "Box";
	case OBJ_NAME::Terrain:
		return "Terrain";
	default:
		assert(!"UnDefinedObjectName");
	}
}

CGameObject::CGameObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}
CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}
void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();

	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();

}
void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();

	m_pMesh = pMesh;

	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}
void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	//if (m_Material)
	//{
	//	m_Material->UpdateShaderVariables(pd3dCommandList, m_MaterialParameterIndex);
	//}
	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) m_pMesh->Render(pd3dCommandList); 
}

void CGameObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3Position = pos;

	m_xmf4x4World._41 = pos.x;
	m_xmf4x4World._42 = pos.y;
	m_xmf4x4World._43 = pos.z;
}

void CGameObject::SetVelocity(XMFLOAT3 pos)
{
	m_xmf3Velocity = pos;
}

void CGameObject::SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents)
{	
	
}

void CGameObject::Move(XMFLOAT3 pos)
{
	SetPosition(Vector3::Add(GetPosition(), pos));
}

void CGameObject::Move()
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity);
}


void CGameObject::Rotate(XMFLOAT3 pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

CRotatingObject::CRotatingObject()
{
	m_Name = OBJ_NAME::None;
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}
CRotatingObject::~CRotatingObject()
{
}
void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}
  
CBox::CBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	m_Name = OBJ_NAME::Box;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);

	SetMesh(pCubeMeshTex);
}

CBox::~CBox()
{
}


CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	m_nObjects = 6;
	m_ppObjects = new CGameObject * [m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CGameObject* pObject = new CGameObject();

		m_ppObjects[i] = pObject;
	}	
	
	CTexturedRectMesh* pSkyBoxMesh_Front = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Back = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Left = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Right = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, 10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Top = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f, +10.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Bottom = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f, -10.0f, +0.0f);

	// 스카이박스
	m_ppObjects[0]->SetMesh(pSkyBoxMesh_Front);
	m_ppObjects[0]->SetTextureIndex(0x02);
	m_ppObjects[0]->SetShader(pShader);

	m_ppObjects[1]->SetMesh(pSkyBoxMesh_Back);
	m_ppObjects[1]->SetTextureIndex(0x04);
	m_ppObjects[1]->SetShader(pShader);

	m_ppObjects[2]->SetMesh(pSkyBoxMesh_Left);
	m_ppObjects[2]->SetTextureIndex(0x08);
	m_ppObjects[2]->SetShader(pShader);

	m_ppObjects[3]->SetMesh(pSkyBoxMesh_Right);
	m_ppObjects[3]->SetTextureIndex(0x10);
	m_ppObjects[3]->SetShader(pShader);

	m_ppObjects[4]->SetMesh(pSkyBoxMesh_Top);
	m_ppObjects[4]->SetTextureIndex(0x20);
	m_ppObjects[4]->SetShader(pShader);

	m_ppObjects[5]->SetMesh(pSkyBoxMesh_Bottom);
	m_ppObjects[5]->SetTextureIndex(0x40);
	m_ppObjects[5]->SetShader(pShader); 
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition3f();

	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->SetPosition(xmf3CameraPos);

	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Draw(pd3dCommandList, pCamera);
}

CTerrain::CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nLength, int nBlockWidth, int nBlockLength,CShader* pShader)
{
	m_nWidth = nWidth;			// 257
	m_nLength = nLength;		// 257

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;	// 32
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;	// 32

	m_nObjects = cxBlocks * czBlocks;
	m_ppObjects = new CGameObject * [m_nObjects];

	for (int i = 0; i < m_nObjects; i++) 
	{
		CGameObject* pObject = new CGameObject();

		m_ppObjects[i] = pObject;
	}

	CTerrainMesh* pTerrainMesh = NULL;

	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			pTerrainMesh = new CTerrainMesh(pd3dDevice, pd3dCommandList, 0, 0, nBlockWidth, nBlockLength, cxBlocks, czBlocks);
		}
	}

	/*CTerrainMesh** pTerrainMesh = new CTerrainMesh * [cxBlocks * czBlocks];

	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			pTerrainMesh[x+(cxBlocks*z)] = new CTerrainMesh(pd3dDevice, pd3dCommandList, x, z, nBlockWidth, nBlockLength,cxBlocks,czBlocks);
		}
	}*/

	for (int i = 0; i < 10; i++)
	{
		m_ppObjects[i]->SetTextureIndex(0x01);
		m_ppObjects[i]->SetMesh(pTerrainMesh);
		m_ppObjects[i]->SetShader(pShader);
		m_ppObjects[i]->SetPosition(XMFLOAT3(i, 200.0f,i*200.0f));

	}
}

CTerrain::~CTerrain()
{
}

void CTerrain::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Draw(pd3dCommandList, pCamera);
}
