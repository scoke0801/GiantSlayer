#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Proj = Matrix4x4::Identity();
	SetLens(0.25f * PI, 1.0f, 1.0f, 1000.0f);
}

CCamera::~CCamera()
{
}

void CCamera::Update(float elapsedTime)
{
	if (m_isOnShake)
	{
		m_TimerForShake.UpdateElapsedTime();
		auto res = m_TimerForShake.GetElapsedTime();
		cout << res << "\n";
		//if (res >= 0.1 && res < 0.3)
		//{ 
		//	Strafe(-5.0f); 
		//}
		//else if (res >= 0.4)
		//{
		//	SetShake(false, 0.0f, 0.0f); 
		//}
		//else
		//{ 
		//	Strafe(5.0f); 
		//} 
		if (res >= m_ShakeTime)
		{
			SetShake(false, 0.0f, 0.0f);
		}
		else
		{
			XMFLOAT3 xmf3Pos = m_xmf3PrevPos;
			xmf3Pos.x += RandomRange(-m_ShakePower, m_ShakePower);
			xmf3Pos.y += RandomRange(-m_ShakePower, m_ShakePower);
			xmf3Pos.z += RandomRange(-m_ShakePower, m_ShakePower);
			
			//Strafe(xmf3Pos.x - m_xmf3PrevPos.x);
			//UpDown(xmf3Pos.y - m_xmf3PrevPos.y);
			//Walk(xmf3Pos.z - m_xmf3PrevPos.z);
			SetPosition(xmf3Pos);
		}
		UpdateViewMatrix(); 
	} 
}

XMVECTOR CCamera::GetPosition()const
{
	return XMLoadFloat3(&m_xmf3Position);
}

XMFLOAT3 CCamera::GetPosition3f()const
{
	return m_xmf3Position;
}

void CCamera::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
	m_ViewDirty = true;
}

void CCamera::SetPosition(const XMFLOAT3& v)
{
	m_xmf3Position = v;
	m_ViewDirty = true;
}

XMVECTOR CCamera::GetRight()const
{
	return XMLoadFloat3(&m_xmf3Right);
}

XMFLOAT3 CCamera::GetRight3f()const
{
	return m_xmf3Right;
}

XMVECTOR CCamera::GetUp()const
{
	return XMLoadFloat3(&m_xmf3Up);
}

XMFLOAT3 CCamera::GetUp3f()const
{
	return m_xmf3Up;
}

XMVECTOR CCamera::GetLook()const
{
	return XMLoadFloat3(&m_xmf3Look);
}

XMFLOAT3 CCamera::GetLook3f()const
{
	return m_xmf3Look;
}

float CCamera::GetNearZ()const
{
	return m_NearZ;
}

float CCamera::GetFarZ()const
{
	return m_FarZ;
}

float CCamera::GetAspect()const
{
	return m_Aspect;
}

float CCamera::GetFovY()const
{
	return m_FovY;
}

float CCamera::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_NearZ);
}

float CCamera::GetNearWindowWidth()const
{
	return m_Aspect * m_NearWindowHeight;
}

float CCamera::GetNearWindowHeight()const
{
	return m_NearWindowHeight;
}

float CCamera::GetFarWindowWidth()const
{
	return m_Aspect * m_FarWindowHeight;
}

float CCamera::GetFarWindowHeight()const
{
	return m_FarWindowHeight;
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

void CCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
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

void CCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	m_ViewDirty = true;
}

XMMATRIX CCamera::GetView()const
{
	assert(!m_ViewDirty);
	return XMLoadFloat4x4(&m_xmf4x4View);
}

XMMATRIX CCamera::GetProj()const
{
	return XMLoadFloat4x4(&m_xmf4x4Proj);
}

XMFLOAT4X4 CCamera::GetView4x4f()const
{
	assert(!m_ViewDirty);
	return m_xmf4x4View;
}

XMFLOAT4X4 CCamera::GetProj4x4f()const
{
	return m_xmf4x4Proj;
}

void CCamera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, r, p));

	m_ViewDirty = true;
}

void CCamera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, l, p));

	m_ViewDirty = true;
}

void CCamera::UpDown(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR u = XMLoadFloat3(&m_xmf3Up);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, u, p));

	m_ViewDirty = true;
}

void CCamera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), angle);

	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_ViewDirty = true;
}

void CCamera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), R));
	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	m_ViewDirty = true;
}

void CCamera::UpdateViewMatrix()
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

		if (m_Lights.size() < 0) return;

		for(auto light : m_Lights)
		{
			light->m_xmf3Position = GetPosition3f();
			light->m_xmf3Direction = GetLook3f();
		}
	}
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

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void**)&m_pcbMappedCamera);
}

void CCamera::ReleaseShaderVariables()
{
	if (m_pd3dcbCamera)
	{
		m_pd3dcbCamera->Unmap(0, NULL);
		m_pd3dcbCamera->Release();
	}
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4View, &xmf4x4View, sizeof(XMFLOAT4X4));

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Proj)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4Projection, &xmf4x4Projection, sizeof(XMFLOAT4X4));

	::memcpy(&m_pcbMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, d3dGpuVirtualAddress);
}

void CCamera::SetShake(bool isOnShake, float shakeTime, float power)
{
	if (m_isOnShake != isOnShake)
	{ 
		m_TimerForShake.Init();
		m_isOnShake = isOnShake; 
	}
	else return;

	if (m_isOnShake)
	{
		m_ShakeTime = shakeTime;
		m_ShakePower = power;
		m_xmf3PrevPos = m_xmf3Position;
	}
	else
	{
		shakeTime = 0.0f; 
		m_xmf3Position = m_xmf3PrevPos;   
		SetPosition(m_xmf3Position);
	}
}
