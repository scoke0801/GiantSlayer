#include "stdafx.h"
#include "Wall.h"

CWall::CWall(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	m_Name = OBJ_NAME::Wall;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth, true);
	m_Type = OBJ_TYPE::Obstacle;
	SetMesh(pCubeMeshTex);
}

CWall::~CWall()
{
}

CDoor::CDoor(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float width, float height, float depth
	, bool isLeft)
	:m_IsLeft(isLeft)
{
	const float MAX_HEIGHT = 2500.0f;
	if (height >= MAX_HEIGHT) height = MAX_HEIGHT;
	m_Height = height;
	m_Name = OBJ_NAME::Door;
	CDoorMeshTest* pCubeMeshTex = new CDoorMeshTest(pd3dDevice, pd3dCommandList,
		width, height, depth, isLeft);
	m_Type = OBJ_TYPE::Obstacle;
	SetMesh(pCubeMeshTex);
	if (!m_IsLeft) Rotate(XMFLOAT3(0, 1, 0), 180.0);
}

CDoor::~CDoor()
{
}

void CDoor::Update(float fTimeElapsed)
{ 
	if (m_IsLeft)
	{
		float rotateAngle = 50;
		if (!m_IsOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 89.0f)
		{
			m_IsOpening = false;
		}
		else if (m_fAngle < -89.0f)
		{
			m_IsOpening = true;
		}
	}
	else
	{
		float rotateAngle = -50;
		if (!m_IsOpening) rotateAngle *= -1;
		Rotate(XMFLOAT3(0, 1, 0), rotateAngle * fTimeElapsed);

		m_fAngle += rotateAngle * fTimeElapsed;
		if (m_fAngle > 89.0f)
		{
			m_IsOpening = true;
		}
		else if (m_fAngle < -89.0f)
		{
			m_IsOpening = false;
		}
	}
}
 
CDoorWall::CDoorWall(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth,
	CShader* pShader)
{
	float fWidthRatio = width * 0.1f;

	CWall* pWall = new CWall(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth);
	pWall->SetShader(pShader);
	pWall->SetObjectName(OBJ_NAME::Wall);
	pWall->SetTextureIndex(0x01);
	pWall->SetPosition({ fWidthRatio * 2,  height * 0.5f, depth * 0.5f }); 
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), 
		XMFLOAT3{ fWidthRatio * 4 *0.5f, height *0.5f, depth *0.5f}));
	m_Walls.push_back(pWall);
	  
	pWall = new CWall(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth);
	pWall->SetShader(pShader);
	pWall->SetObjectName(OBJ_NAME::Wall);
	pWall->SetTextureIndex(0x01);
	pWall->SetPosition({ fWidthRatio * 6 + fWidthRatio * 2,  height * 0.5f, depth * 0.5f });
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), 
		XMFLOAT3{ fWidthRatio * 4 * 0.5f, height * 0.5f, depth * 0.5f }));
	m_Walls.push_back(pWall);  

	float createdHeight;
	m_LeftDoor = new CDoor(pd3dDevice, pd3dCommandList, fWidthRatio, height, depth * 0.2f, true);
	m_LeftDoor->SetShader(pShader);
	m_LeftDoor->SetObjectName(OBJ_NAME::Wall);
	m_LeftDoor->SetTextureIndex(0x10);
	createdHeight = m_LeftDoor->GetHeight(); 
	m_LeftDoor->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Left, fWidthRatio, createdHeight, depth * 0.2f, XMFLOAT3{ 0,0,0 });
	m_LeftDoor->AddColider(new ColliderBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f), 
		XMFLOAT3{ fWidthRatio * 0.5f, createdHeight * 0.5f, depth * 0.2f * 0.5f }));
	if (height != createdHeight)
		m_LeftDoor->SetPosition({ fWidthRatio * 4,  height * 0.5f - createdHeight * 0.5f, depth * 0.5f });
	else
		m_LeftDoor->SetPosition({ fWidthRatio * 4,  height * 0.5f, depth * 0.5f });

	//3000, 1500
	m_RightDoor = new CDoor(pd3dDevice, pd3dCommandList, fWidthRatio, height, depth * 0.2f, false);
	m_RightDoor->SetShader(pShader);
	m_RightDoor->SetObjectName(OBJ_NAME::Wall);
	m_RightDoor->SetTextureIndex(0x10);  
	createdHeight = m_RightDoor->GetHeight();
	m_RightDoor->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Left, fWidthRatio, createdHeight, depth * 0.2f, XMFLOAT3{ 0,0,0 });
	m_RightDoor->AddColider(new ColliderBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 0.5f, createdHeight * 0.5f, depth * 0.2f * 0.5f }));
	if (height != createdHeight)
		m_RightDoor->SetPosition({ fWidthRatio * 6,  height * 0.5f - createdHeight * 0.5f, depth * 0.5f });
	else
		m_RightDoor->SetPosition({ fWidthRatio * 6,  height * 0.5f, depth * 0.5f });
}

CDoorWall::CDoorWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth,
	bool isRotated, CShader* pShader)
{
	float fWidthRatio = width * 0.1f;

	CWall* pWall = new CWall(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth);
	pWall->SetShader(pShader);
	pWall->SetObjectName(OBJ_NAME::Wall);
	pWall->SetTextureIndex(0x01);
	pWall->Rotate({ 0,1,0 }, 90);
	pWall->SetPosition({ depth * 0.5f,  height * 0.5f, fWidthRatio * 2 });	
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fWidthRatio * 4, height, depth)));
	m_Walls.push_back(pWall);

	pWall = new CWall(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth);
	pWall->SetShader(pShader);
	pWall->SetObjectName(OBJ_NAME::Wall);
	pWall->SetTextureIndex(0x01);
	pWall->Rotate({ 0,1,0 }, 90);
	pWall->SetPosition({ depth * 0.5f,  height * 0.5f, fWidthRatio * 6 + fWidthRatio * 2 });	
	pWall->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, fWidthRatio * 4, height, depth, XMFLOAT3{ 0,0,0 });
	pWall->AddColider(new ColliderBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fWidthRatio * 4, height, depth)));
	m_Walls.push_back(pWall);

	m_LeftDoor = new CDoor(pd3dDevice, pd3dCommandList, fWidthRatio, height, depth * 0.2f, true);
	m_LeftDoor->SetShader(pShader);
	m_LeftDoor->SetObjectName(OBJ_NAME::Wall);
	m_LeftDoor->SetTextureIndex(0x10);
	m_LeftDoor->Rotate({ 0,1,0 }, 90);
	m_LeftDoor->Rotate({ 0,1,0 }, 180);
	m_LeftDoor->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Left, fWidthRatio, height, depth * 0.2f, XMFLOAT3{ 0,0,0 });
	m_LeftDoor->AddColider(new ColliderBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 0.5f, height * 0.5f, depth * 0.2f * 0.5f }));
	m_LeftDoor->SetPosition({ depth * 0.5f,  height * 0.5f,  fWidthRatio * 4 });

	m_RightDoor = new CDoor(pd3dDevice, pd3dCommandList, fWidthRatio, height, depth * 0.2f, false);
	m_RightDoor->SetShader(pShader);
	m_RightDoor->SetObjectName(OBJ_NAME::Wall);
	m_RightDoor->SetTextureIndex(0x10);
	m_RightDoor->Rotate({ 0,1,0 }, 90); 
	m_RightDoor->Rotate({ 0,1,0 }, 180); 
	m_RightDoor->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Right, fWidthRatio, height, depth * 0.2f, XMFLOAT3{ 0,0,0 });
	m_RightDoor->AddColider(new ColliderBox(XMFLOAT3(fWidthRatio * 0.5f, 0.0f, 0.0f),
		XMFLOAT3{ fWidthRatio * 0.5f, height * 0.5f, depth * 0.2f * 0.5f }));

	m_RightDoor->SetPosition({ depth * 0.5f,  height * 0.5f, fWidthRatio * 6 });
}

CDoorWall::~CDoorWall()
{
}

void CDoorWall::Update(float fTimeElapsed)
{
	m_LeftDoor->Update(fTimeElapsed);
	m_RightDoor->Update(fTimeElapsed);
	for (CWall* pWall : m_Walls)
	{
		pWall->Update(fTimeElapsed);
	}
}

void CDoorWall::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_LeftDoor->Draw(pd3dCommandList, pCamera);
	m_RightDoor->Draw(pd3dCommandList, pCamera);
	for (CWall* pWall : m_Walls)
	{
		pWall->Draw(pd3dCommandList, pCamera);
	}
}

void CDoorWall::UpdateColliders()
{
	m_LeftDoor->UpdateColliders();
	m_RightDoor->UpdateColliders();
	for (CWall* pWall : m_Walls)
	{
		pWall->UpdateColliders();

		//DisplayVector3(pWall->GetColliders()[0]->GetBox().Center);
	}
}

bool CDoorWall::CollisionCheck(Collider* pAABB)
{
	//cout << "Called\n";
	if(m_LeftDoor->CollisionCheck(pAABB)) return true;
	if (m_RightDoor->CollisionCheck(pAABB)) return true; 

	for (CWall* pWall : m_Walls) {
		if (pWall->CollisionCheck(pAABB)) return true;  
	}
	return false;
}
 
void CDoorWall::SetPosition(XMFLOAT3 xmf3Position)
{
	m_LeftDoor->SetPositionPlus(xmf3Position);
	m_RightDoor->SetPositionPlus(xmf3Position);
	for (CWall* pWall : m_Walls)
	{
		pWall->SetPositionPlus(xmf3Position);
	}
}

void CDoorWall::SetTextureIndexes(int idx)
{
	for (CWall* pWall : m_Walls)
	{
		pWall->SetTextureIndex(idx);
	}
}
void CDoorWall::RotateAll(XMFLOAT3 xmf3Axis, float angle)
{
	m_LeftDoor->Rotate(xmf3Axis, angle);
	m_RightDoor->Rotate(xmf3Axis, angle);

	for (CWall* pWall : m_Walls)
	{
		pWall->Rotate(xmf3Axis, angle);
	}
}
