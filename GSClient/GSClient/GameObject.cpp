#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"
#include "Terrain.h"

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
	return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

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
void CGameObject::FixPositionByTerrain(CTerrain* pTerrain)
{
	m_xmf3Position.y = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z) + m_YPositionCorrection;
	SetPosition(m_xmf3Position);
}
void CGameObject::BuildBoundigBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth,
	const XMFLOAT3& shift)
{
	CMesh* pMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, false, fWidth, fHeight, fDepth, shift);
	m_BoundingObjectMeshes.push_back(std::move(pMesh));
}
void CGameObject::BuildBoundigSphereMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	PulledModel pulledModel,
	float radius, UINT32 sliceCount, UINT32 stackCount,
	const XMFLOAT3& shift)
{
	CMesh* pMesh = new CSphereMeshDiffused(pd3dDevice, pd3dCommandList, pulledModel,  radius, sliceCount, stackCount, shift);
	m_BoundingObjectMeshes.push_back(std::move(pMesh));
}
void CGameObject::BuildBoundigBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	PulledModel pulledModel, 
	float fWidth, float fHeight, float fDepth,
	const XMFLOAT3& shift)
{
	//CMesh* pMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, pulledModel, fWidth, fHeight, fDepth);
	CMesh* pMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, pulledModel, fWidth, fHeight, fDepth,shift);
	m_BoundingObjectMeshes.push_back(std::move(pMesh));
}
void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, 
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObjInfo);
}
void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{ 
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World))); 
	::memcpy(&m_pcbMappedGameObjInfo->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4)); 
	::memcpy(&m_pcbMappedGameObjInfo->m_Material, m_Material, sizeof(MATERIAL));
	::memcpy(&m_pcbMappedGameObjInfo->m_nTextureIndex, &m_nTextureIndex, sizeof(UINT)); 
} 
void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{

}

void CGameObject::Update(float fTimeElapsed)
{
	if (!m_isDrawbale) return;
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	static float Friction = 50.0f;	

	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ =  MaxVelocityXZ * fTimeElapsed;
	if (fLength > MaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY =  MaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > MaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity); 

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = ( Friction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true)); 

	Animate(fTimeElapsed);
}

void CGameObject::OnPrepareRender()
{

}

void CGameObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_isDrawbale) return;
	OnPrepareRender();

	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera); 
	}
	if (m_pMesh) m_pMesh->Render(pd3dCommandList); 
	
	if (gbBoundaryOn)
	{ 
		m_pShader->RenderBoundary(pd3dCommandList, pCamera);
		for (auto pBoundingMesh : m_BoundingObjectMeshes)
		{
			pBoundingMesh->Render(pd3dCommandList);
		}
	}
}

void CGameObject::DrawForBoundingObj(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (gbBoundaryOn)
	{
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0); 
		m_pShader->RenderBoundary(pd3dCommandList, pCamera);
		for (auto pBoundingMesh : m_BoundingObjectMeshes)
		{
			pBoundingMesh->Render(pd3dCommandList);
		}
	}
}

void CGameObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3PrevPosition = m_xmf3Position;
	m_xmf3Position = pos;

	m_xmf4x4World._41 = pos.x;
	m_xmf4x4World._42 = pos.y;
	m_xmf4x4World._43 = pos.z;
}

void CGameObject::SetPositionPlus(XMFLOAT3 pos)
{
	pos = Vector3::Add(m_xmf3Position, pos);
	m_xmf3PrevPosition = m_xmf3Position;
	m_xmf3Position = pos;

	m_xmf4x4World._41 = pos.x;
	m_xmf4x4World._42 = pos.y;
	m_xmf4x4World._43 = pos.z;
}

void CGameObject::SetVelocity(XMFLOAT3 vel)
{
	m_xmf3Velocity = vel;
}

void CGameObject::SetVelocity(OBJ_DIRECTION direction)
{
	XMFLOAT3 look = GetLook();
	XMFLOAT3 right = GetRight(); 
	
	switch (direction)
	{
	case OBJ_DIRECTION::Front: 
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(look, PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Back:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(look, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Left:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(right, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Right:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(right, PLAYER_RUN_VELOCITY));
		break;
	default:
		assert("잘못된 방향으로 이동할 수 없어요~");
		break;
	} 
	
}

void CGameObject::SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents)
{	
	
}
  
XMFLOAT3 CGameObject::GetRight()const
{
	return XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
}


XMFLOAT3 CGameObject::GetUp()const
{
	return XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
}

XMFLOAT3 CGameObject::GetLook()const
{
	return XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
}

void CGameObject::AddAABB(Collider* pCollider)
{
	auto type = pCollider->GetType();
	if (type == ColliderType::Box) {
		m_AABB.push_back(new ColliderBox(pCollider->GetBox().Center, pCollider->GetBox().Extents));
	}
	else {
		assert(!" CGameObject::AddAABB(Collider* pCollider)");
	}
}

void CGameObject::Move(XMFLOAT3 shift)
{
	SetPosition(Vector3::Add(m_xmf3Position, shift)); 
}

void CGameObject::Move()
{
	SetPosition(Vector3::Add(m_xmf3Position, m_xmf3Velocity));
	//m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity);
}

void CGameObject::Rotate(XMFLOAT3 pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
void CGameObject::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{  
	XMFLOAT3 L = Vector3::Normalize(Vector3::Subtract(target, pos));
	XMFLOAT3 R = Vector3::Normalize(Vector3::CrossProduct(up, L));
	XMFLOAT3 U = Vector3::CrossProduct(L, R);

	float x = -(Vector3::DotProduct(pos, R));
	float y = -(Vector3::DotProduct(pos, U));
	float z = -(Vector3::DotProduct(pos, L));

	m_xmf4x4World(0, 0) = R.x;
	m_xmf4x4World(0, 1) = R.y;
	m_xmf4x4World(0, 2) = R.z;  

	m_xmf4x4World(1, 0) = U.x;
	m_xmf4x4World(1, 1) = U.y;
	m_xmf4x4World(1, 2) = U.z; 

	m_xmf4x4World(2, 0) = L.x;
	m_xmf4x4World(2, 1) = L.y;
	m_xmf4x4World(2, 2) = L.z;
	Scale(m_xmf3Size.x, m_xmf3Size.y, m_xmf3Size.z, false); 
}

void CGameObject::Scale(float x, float y, float z, bool setSize)
{
	if (setSize)
	{
		m_xmf3Size = { x,y,z };
	}
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World); 
}

bool CGameObject::CollisionCheck(Collider* pAABB)
{
	for (int i = 0; i < m_AABB.size(); ++i) {
		auto thisBox = m_AABB[i]->GetBox(); 
		bool result = thisBox.Intersects(pAABB->GetBox());
		if (result) return true;  
	}

	return false;
}

bool CGameObject::CollisionCheck(CGameObject* other)
{
	auto otherAABB = other->GetAABB();
	for (int i = 0; i < otherAABB.size(); ++i) {
		bool result = CollisionCheck(otherAABB[i]);
		if (result) return true; 
	}

	return false;
}

void CGameObject::FixCollision()
{
	SetPosition(m_xmf3PrevPosition);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void CGameObject::UpdateColliders()
{
	for (int i = 0; i < m_Colliders.size(); ++i) {
		m_Colliders[i]->GetBox().Transform(m_AABB[i]->GetBox(), XMLoadFloat4x4(&m_xmf4x4World));
	}
	/*for (auto collider : m_Colliders) {
		collider->Update(m_xmf4x4World);
	}*/
}
 
//void CGameObject::Rotate(float x, float y, float z)
//{
//	static float pitch, yaw, roll;
//	if (x != 0.0f)
//	{
//		pitch += x;
//		if (pitch > +89.0f) { x -= (pitch - 89.0f); pitch = +89.0f; }
//		if (pitch < -89.0f) { x -= (pitch + 89.0f); pitch = -89.0f; }
//	}
//	if (y != 0.0f)
//	{
//		yaw += y;
//		if (yaw > 360.0f) yaw -= 360.0f;
//		if (yaw < 0.0f) yaw += 360.0f;
//	}
//	if (z != 0.0f)
//	{
//		roll += z;
//		if (roll > +20.0f) { z -= (roll - 20.0f); roll = +20.0f; }
//		if (roll < -20.0f) { z -= (roll + 20.0f); roll = -20.0f; }
//	}
//	//m_pCamera->Rotate(x, y, z);
//	if (y != 0.0f)
//	{
//		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
//		XMFLOAT3 xmf3Look = Vector3::TransformNormal(GetLook(), xmmtxRotate);
//		XMFLOAT3 xmf3Right = Vector3::TransformNormal(GetRight(), xmmtxRotate);
//	}
//	XMFLOAT3 xmf3Look = GetLook();
//	XMFLOAT3 xmf3Right = GetRight();
//	XMFLOAT3 xmf3Up = GetUp();
//	m_xmf4x4World._11 = xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = xmf3Right.z;
//	m_xmf4x4World._21 = xmf3Up.x;    m_xmf4x4World._22 = xmf3Up.y;	  m_xmf4x4World._23 = xmf3Up.z;
//	m_xmf4x4World._31 = xmf3Look.x;  m_xmf4x4World._32 = xmf3Look.y;  m_xmf4x4World._33 = xmf3Look.z;
//}
 
CBox::CBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	m_Name = OBJ_NAME::Box;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);
	SetMesh(pCubeMeshTex);

	m_Type = OBJ_TYPE::Obstacle;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CBox::~CBox()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////

CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	m_nObjects = 6;
	m_ppObjects = new CGameObject * [m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CGameObject* pObject = new CGameObject();

		m_ppObjects[i] = pObject;
	}	
	
	CTexturedRectMesh* pSkyBoxMesh_Front = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,  10.0f, 10.0f,  0.0f,   0.0f,   0.0f,   5.0f);
	CTexturedRectMesh* pSkyBoxMesh_Back = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,   10.0f, 10.0f,  0.0f,   0.0f,   0.0f,  -5.0f);
	CTexturedRectMesh* pSkyBoxMesh_Left = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,    0.0f, 10.0f, 10.0f,  -5.0f,   0.0f,   0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Right = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,   0.0f, 10.0f, 10.0f,   5.0f,   0.0f,   0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Top = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,    10.0f, 00.0f, 10.0f,   0.0f,   5.0f,   0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Bottom = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 10.0f, 00.0f, 10.0f,   0.0f,  -5.0f,   0.0f);

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

void CSkyBox::Rotate(XMFLOAT3 pxmf3Axis, float fAngle)
{
	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Rotate(pxmf3Axis, fAngle);
}

void CSkyBox::Update(float timeElapsed)
{ 
	Rotate(XMFLOAT3(0, 1, 0), 0.3 * timeElapsed);
}

CSkyBoxSphere::CSkyBoxSphere(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	CShader* pShader,
	float radius, UINT32 sliceCount, UINT32 stackCount)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject * [m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CGameObject* pObject = new CGameObject();

		m_ppObjects[i] = pObject;
	} 

	// 스카이박스  
	CSphereMesh* pSphereMesh = new CSphereMesh(pd3dDevice, pd3dCommandList,
		radius, sliceCount, stackCount);
	m_ppObjects[0]->SetMesh(pSphereMesh); 
	m_ppObjects[0]->SetTextureIndex(0x02);
	m_ppObjects[0]->SetShader(pShader);
}

CSkyBoxSphere::~CSkyBoxSphere()
{
}

CBillboard::CBillboard(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nBillboardObjects = 300;

	m_BillboardObjects.reserve(m_nBillboardObjects);
	
	CBillboardVertex* pGeometryBillboardVertices = new CBillboardVertex[m_nBillboardObjects];

	CBillboardMesh* pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, pGeometryBillboardVertices, m_nBillboardObjects);
	
}

CBillboard::~CBillboard()
{
}

void CBillboard::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto pObj : m_BillboardObjects)
	{
		pObj->Draw(pd3dCommandList, pCamera);
	}
}