#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}
CCamera::~CCamera()
{
}
void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight,
	float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}
void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}
void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance,
	float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle),
		fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}
void CCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf4x4View = Matrix4x4::LookAtLH(xmf3Position, xmf3LookAt, xmf3Up);
}
void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	 
	pd3dCommandList->SetGraphicsRoot32BitConstants(rootParameterIndex, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection,
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(rootParameterIndex, 16, &xmf4x4Projection, 16);
}
void CCamera::ReleaseShaderVariables()
{
}
void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

CTestCamera::CTestCamera()
{
	m_xmf3EyePos = { 0.0f, 0.0f, 0.0f };
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Proj = Matrix4x4::Identity();
	
	m_Theta = 1.5f * XM_PI;
	m_Phi = 0.2f * XM_PI;
	m_Radius = 15.0f;

}

void CTestCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight,
	float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}
void CTestCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom; 
}

void CTestCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CTestCamera::CreatProjectionMatrix()
{	
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.55f * PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_xmf4x4Proj, P);
}

void CTestCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));

	//루트 파라메터 인덱스 1의
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection,
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Proj)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
}

float CTestCamera::AspectRatio() const
{ 
	return static_cast<float>(m_d3dScissorRect.right - m_d3dScissorRect.left) / (m_d3dScissorRect.bottom - m_d3dScissorRect.top);
}

void CTestCamera::Update(double elapsedTime)
{
	// Convert Spherical to Cartesian coordinates.
	m_xmf3EyePos.x = m_Radius * sinf(m_Phi) * cosf(m_Theta);
	m_xmf3EyePos.z = m_Radius * sinf(m_Phi) * sinf(m_Theta);
	m_xmf3EyePos.y = m_Radius * cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(m_xmf3EyePos.x, m_xmf3EyePos.y, m_xmf3EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_xmf4x4View, view);
}

void CTestCamera::Rotate(int x, int y, int prevX, int prevY)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.05f * static_cast<float>(x - prevX));
	float dy = XMConvertToRadians(0.05f * static_cast<float>(y - prevY));

	// Update angles based on input to orbit camera around box.
	m_Theta += dx;
	m_Phi += dy;

	// Restrict the angle mPhi.
	m_Phi = Clamp(m_Phi, 0.1f, float(PI - 0.1f));
}

void CTestCamera::Zoom(int x, int y, int prevX, int prevY)
{       
	// Make each pixel correspond to 0.2 unit in the scene.
	float dx = 0.05f * static_cast<float>(x - prevX);
	float dy = 0.05f * static_cast<float>(y - prevY);

	// Update the camera radius based on input.
	m_Radius += dx - dy;

	// Restrict the radius.
	m_Radius = Clamp(m_Radius, 5.0f, 150.0f);
}


CTestCamera2::CTestCamera2()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Proj = Matrix4x4::Identity();
	SetLens(0.25f * PI, 1.0f, 1.0f, 1000.0f);
}

CTestCamera2::~CTestCamera2()
{
}

XMVECTOR CTestCamera2::GetPosition()const
{
	return XMLoadFloat3(&m_xmf3Position);
}

XMFLOAT3 CTestCamera2::GetPosition3f()const
{
	return m_xmf3Position;
}

void CTestCamera2::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
	m_ViewDirty = true;
}

void CTestCamera2::SetPosition(const XMFLOAT3& v)
{
	m_xmf3Position = v;
	m_ViewDirty = true;
}

XMVECTOR CTestCamera2::GetRight()const
{
	return XMLoadFloat3(&m_xmf3Right);
}

XMFLOAT3 CTestCamera2::GetRight3f()const
{
	return m_xmf3Right;
}

XMVECTOR CTestCamera2::GetUp()const
{
	return XMLoadFloat3(&m_xmf3Up);
}

XMFLOAT3 CTestCamera2::GetUp3f()const
{
	return m_xmf3Up;
}

XMVECTOR CTestCamera2::GetLook()const
{
	return XMLoadFloat3(&m_xmf3Look);
}

XMFLOAT3 CTestCamera2::GetLook3f()const
{
	return m_xmf3Look;
}

float CTestCamera2::GetNearZ()const
{
	return m_NearZ;
}

float CTestCamera2::GetFarZ()const
{
	return m_FarZ;
}

float CTestCamera2::GetAspect()const
{
	return m_Aspect;
}

float CTestCamera2::GetFovY()const
{
	return m_FovY;
}

float CTestCamera2::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_NearZ);
}

float CTestCamera2::GetNearWindowWidth()const
{
	return m_Aspect * m_NearWindowHeight;
}

float CTestCamera2::GetNearWindowHeight()const
{
	return m_NearWindowHeight;
}

float CTestCamera2::GetFarWindowWidth()const
{
	return m_Aspect * m_FarWindowHeight;
}

float CTestCamera2::GetFarWindowHeight()const
{
	return m_FarWindowHeight;
}

void CTestCamera2::SetLens(float fovY, float aspect, float zn, float zf)
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

void CTestCamera2::SetLens(float fovY, float width, float height, float zn, float zf)
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

void CTestCamera2::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
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

void CTestCamera2::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	m_ViewDirty = true;
}

XMMATRIX CTestCamera2::GetView()const
{
	assert(!m_ViewDirty);
	return XMLoadFloat4x4(&m_xmf4x4View);
}

XMMATRIX CTestCamera2::GetProj()const
{
	return XMLoadFloat4x4(&m_xmf4x4Proj);
}
 
XMFLOAT4X4 CTestCamera2::GetView4x4f()const
{
	assert(!m_ViewDirty);
	return m_xmf4x4View;
}

XMFLOAT4X4 CTestCamera2::GetProj4x4f()const
{
	return m_xmf4x4Proj;
}

void CTestCamera2::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, r, p));

	m_ViewDirty = true;
}

void CTestCamera2::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, l, p));

	m_ViewDirty = true;
}

void CTestCamera2::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), angle);

	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_ViewDirty = true;
}

void CTestCamera2::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), R));
	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_ViewDirty = true;
}

void CTestCamera2::UpdateViewMatrix()
{
	if (m_ViewDirty)
	{
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
	}
}
void CTestCamera2::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight,
	float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}
void CTestCamera2::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void CTestCamera2::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CTestCamera2::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(rootParameterIndex, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Proj;
	XMStoreFloat4x4(&xmf4x4Proj,
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Proj)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(rootParameterIndex, 16, &xmf4x4Proj, 16);

}
