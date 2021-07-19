#include "stdafx.h"
#include "AnimationObject.h"
#include "AnimationController.h"

void CAnimationObject::Update(float fTimeElapsed)
{
	//CGameObject::Update(fTimeElapsed);
	CAnimationObject::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CAnimationObject::Animate(float fTimeElapsed)
{
	if (m_pAnimationController) m_pAnimationController->AdvanceTime(fTimeElapsed, NULL);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void CAnimationObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CAnimationObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3PrevPosition = m_xmf3Position;
	m_xmf3Position = pos;
	m_xmf4x4ToParent._41 = pos.x;
	m_xmf4x4ToParent._42 = pos.y;
	m_xmf4x4ToParent._43 = pos.z;

	UpdateTransform(NULL);
}

void CAnimationObject::Move(XMFLOAT3 shift)
{
	SetPosition(Vector3::Add(m_xmf3Position, shift));
}

void CAnimationObject::Scale(float x, float y, float z, bool setSize)
{
	if (setSize)
	{
		m_xmf3Size = { x,y,z };
	}
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CAnimationObject::Rotate(const XMFLOAT3& pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CAnimationObject::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMFLOAT3 L = Vector3::Normalize(Vector3::Subtract(target, pos));
	XMFLOAT3 R = Vector3::Normalize(Vector3::CrossProduct(up, L));
	XMFLOAT3 U = Vector3::CrossProduct(L, R);

	float x = -(Vector3::DotProduct(pos, R));
	float y = -(Vector3::DotProduct(pos, U));
	float z = -(Vector3::DotProduct(pos, L));

	m_xmf4x4ToParent(0, 0) = R.x;
	m_xmf4x4ToParent(0, 1) = R.y;
	m_xmf4x4ToParent(0, 2) = R.z;

	m_xmf4x4ToParent(1, 0) = U.x;
	m_xmf4x4ToParent(1, 1) = U.y;
	m_xmf4x4ToParent(1, 2) = U.z;

	m_xmf4x4ToParent(2, 0) = L.x;
	m_xmf4x4ToParent(2, 1) = L.y;
	m_xmf4x4ToParent(2, 2) = L.z;
	Scale(m_xmf3Size.x, m_xmf3Size.y, m_xmf3Size.z, false);
}

void CAnimationObject::SetChild(CAnimationObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) {

			SetAABB(pChild->GetAABB());
			pChild->AddRef();
		}
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CAnimationObject::UpdateColliders()
{
	for (int i = 0; i < m_BoundingBox.size(); ++i) {
		m_BoundingBox[i].Transform(m_AABB[i], XMLoadFloat4x4(&m_xmf4x4World));
	}
	
	if (m_pSibling) m_pSibling->UpdateColliders();
	if (m_pChild) m_pChild->UpdateColliders();
}

bool CAnimationObject::CollisionCheck(CGameObject* other)
{
	auto otherAABB = other->GetAABB();
	for (int i = 0; i < otherAABB.size(); ++i) {
		bool result = CGameObject::CollisionCheck(otherAABB[i]);
		if (result) return true;
	} 
	return false;
}

void CAnimationObject::GetAABBToBuffer(vector<BoundingBox>& buffer)
{
	for (int i = 0; i < m_AABB.size(); ++i) {
		buffer.push_back(m_AABB[i]);
	}
	if (m_pSibling) m_pSibling->GetAABBToBuffer(buffer);
	if (m_pChild) m_pChild->GetAABBToBuffer(buffer);
}

void CAnimationObject::CollectAABBFromChilds()
{
	if (m_pSibling) m_pSibling->GetAABBToBuffer(m_AABB);
	if (m_pChild) m_pChild->GetAABBToBuffer(m_AABB);
}

CAnimationObject* CAnimationObject::LoadFrameHierarchyFromFile(CAnimationObject* pParent, FILE* pInFile)
{
	return nullptr;
}

CAnimationObject* CAnimationObject::LoadGeometryAndAnimationFromFile(char* pstrFileName, bool bHasAnimation)
{
	return nullptr;
}
