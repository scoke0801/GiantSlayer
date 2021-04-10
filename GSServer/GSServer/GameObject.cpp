#include "stdafx.h"
#include "GameObject.h" 

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
}
  

void CGameObject::Update(float fTimeElapsed)
{
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
}
  
void CGameObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3PrevPosition = m_xmf3Position;
	m_xmf3Position = pos;

	m_xmf4x4World._41 = pos.x;
	m_xmf4x4World._42 = pos.y;
	m_xmf4x4World._43 = pos.z;
}

void CGameObject::SetVelocity(const XMFLOAT3& vel)
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

bool CGameObject::CollisionCheck(const BoundingBox& pAABB)
{
	for (int i = 0; i < m_AABB.size(); ++i) { 
		bool result = m_AABB[i].Intersects(pAABB);
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
	cout << "prev : ";
	DisplayVector3(m_xmf3PrevPosition, false);
	SetPosition(m_xmf3PrevPosition);
	cout << " cur : ";
	DisplayVector3(m_xmf3Position);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void CGameObject::UpdateColliders()
{
	for (int i = 0; i < m_BoundingBox.size(); ++i) {
		m_BoundingBox[i].Transform(m_AABB[i], XMLoadFloat4x4(&m_xmf4x4World)); 
	}
}

void CGameObject::AddAABB(const BoundingBox& boundingBox)
{ 
	m_AABB.push_back(boundingBox);
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

void CGameObject::Move(XMFLOAT3 shift)
{
	SetPosition(Vector3::Add(m_xmf3Position, shift));
}

void CGameObject::Move()
{
	SetPosition(Vector3::Add(m_xmf3Position, m_xmf3Velocity)); 
}

void CGameObject::Rotate(const XMFLOAT3& pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
 

void CGameObject::Scale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World);
}
 
 