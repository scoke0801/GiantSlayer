#include "stdafx.h"
#include "Camera.h"
#include "Player.h"
CCamera::CCamera()
{ 
}

CCamera::~CCamera()
{
}

void CCamera::Update(float elapsedTime)
{
	if (m_ViewDirty)
	{
		UpdateViewMatrix();
	}
}

void CCamera::Update(const XMFLOAT3& xmf3LookAt)
{
	if (m_TargetPlayer == nullptr) return;

	XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();

	XMFLOAT3 xmf3Right = CalcTargetRight();
	XMFLOAT3 xmf3Up = CalcTargetUp();
	XMFLOAT3 xmf3Look = CalcTargetLook();

	xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
	xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
	xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

	XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate);

	XMFLOAT3 xmf3Position = Vector3::Add(m_TargetPlayer->GetPosition(), xmf3Offset);
	XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
	float fLength = Vector3::Length(xmf3Direction);
	xmf3Direction = Vector3::Normalize(xmf3Direction);

	//float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
	float fTimeLagScale = 1.0f;
	float fDistance = fLength * fTimeLagScale;
	if (fDistance > fLength) fDistance = fLength;
	if (fLength < 0.01f) fDistance = fLength;
	if (fDistance > 0)
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
		LookAt(xmf3LookAt, m_TargetPlayer->GetUp());
	}
}

void CCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	if (m_TargetPlayer == nullptr) return;

	XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();

	XMFLOAT3 xmf3Right = CalcTargetRight();
	XMFLOAT3 xmf3Up = CalcTargetUp();
	XMFLOAT3 xmf3Look = CalcTargetLook();

	xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
	xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
	xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

	XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate);

	XMFLOAT3 xmf3Position = Vector3::Add(m_TargetPlayer->GetPosition(), xmf3Offset);
	XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
	float fLength = Vector3::Length(xmf3Direction);
	xmf3Direction = Vector3::Normalize(xmf3Direction);

	//float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
	float fTimeLagScale = 1.0f;
	float fDistance = fLength * fTimeLagScale;
	if (fDistance > fLength) fDistance = fLength;
	if (fLength < 0.01f) fDistance = fLength;
	if (fDistance > 0)
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
		LookAt(xmf3LookAt, m_TargetPlayer->GetUp());
	}
}

void CCamera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_xmf3Position, pos);
	XMStoreFloat3(&m_xmf3Look, L);
	XMStoreFloat3(&m_xmf3Right, R);
	XMStoreFloat3(&m_xmf3Up, U);

	m_ViewDirty = true;
}

void CCamera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	m_ViewDirty = true;
}

void CCamera::LookAt(const XMFLOAT3& lookAt, const XMFLOAT3& up)
{
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, lookAt, up);
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

void CCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_xmf4x4Proj, P);
}

void CCamera::SetLens(float fovY, float width, float height, float zn, float zf)
{
	// cache properties
	m_FovY = fovY;
	m_Aspect = width / height;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_xmf4x4Proj, P);
}

void CCamera::Strafe(float d)
{	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, r, p));

	m_ViewDirty = true;
}

void CCamera::Walk(float d)
{	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, l, p));

	m_ViewDirty = true;
}

void CCamera::UpDown(float d)
{	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR u = XMLoadFloat3(&m_xmf3Up);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, u, p));

	m_ViewDirty = true;
}

void CCamera::Pitch(float angle)
{	// Rotate up and look vector about the right vector.
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), angle);

	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_ViewDirty = true;
}

void CCamera::RotateY(float angle)
{	// Rotate the basis vectors about the world y-axis.
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), R));
	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_ViewDirty = true;
}

void CCamera::RotateAroundTarget(XMFLOAT3 pxmf3Axis, float fAngle)
{
	if (m_TargetPlayer == nullptr) return;
	
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis),
		XMConvertToRadians(fAngle));

	m_TargetTransform = Matrix4x4::Multiply(mtxRotate, m_TargetTransform);  
}

void CCamera::UpdateViewMatrix()
{
	if (!m_ViewDirty) return;

	XMVECTOR R = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR U = XMLoadFloat3(&m_xmf3Up);
	XMVECTOR L = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR P = XMLoadFloat3(&m_xmf3Position);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_xmf3Right, R);
	XMStoreFloat3(&m_xmf3Up, U);
	XMStoreFloat3(&m_xmf3Look, L);

	m_xmf4x4View(0, 0) = m_xmf3Right.x;
	m_xmf4x4View(1, 0) = m_xmf3Right.y;
	m_xmf4x4View(2, 0) = m_xmf3Right.z;
	m_xmf4x4View(3, 0) = x;

	m_xmf4x4View(0, 1) = m_xmf3Up.x;
	m_xmf4x4View(1, 1) = m_xmf3Up.y;
	m_xmf4x4View(2, 1) = m_xmf3Up.z;
	m_xmf4x4View(3, 1) = y;

	m_xmf4x4View(0, 2) = m_xmf3Look.x;
	m_xmf4x4View(1, 2) = m_xmf3Look.y;
	m_xmf4x4View(2, 2) = m_xmf3Look.z;
	m_xmf4x4View(3, 2) = z;

	m_xmf4x4View(0, 3) = 0.0f;
	m_xmf4x4View(1, 3) = 0.0f;
	m_xmf4x4View(2, 3) = 0.0f;
	m_xmf4x4View(3, 3) = 1.0f;

	m_ViewDirty = false;

	m_ViewDirty = false;
}

void CCamera::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
	m_ViewDirty = true;
}

void CCamera::SetPosition(const DirectX::XMFLOAT3& v)
{
	m_xmf3Position = v;
	m_ViewDirty = true;
}

void CCamera::SetTarget(CPlayer* target)
{
	m_TargetPlayer = target;
	m_TargetTransform = m_TargetPlayer->GetWorldTransform();
}

void CCamera::SetOffset(XMFLOAT3 offset)
{
	m_xmf3Offset = offset;
}

void CCamera::MoveOffset(XMFLOAT3 shift)
{
	m_xmf3Offset = Vector3::Add(m_xmf3Offset, shift);
}
