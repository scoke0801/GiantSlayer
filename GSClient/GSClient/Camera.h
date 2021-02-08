#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3   m_xmf3Position;
};

class CPlayer;

class CCamera
{
private:
	// Camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3			m_xmf3Position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3			m_xmf3Right = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3			m_xmf3Up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3			m_xmf3Look = { 0.0f, 0.0f, 1.0f };

	float						m_Speed = 15.0f;

	// Cache frustum properties.
	float						m_NearZ = 0.0f;
	float						m_FarZ = 0.0f;
	float						m_Aspect = 0.0f;
	float						m_FovY = 0.0f;
	float						m_NearWindowHeight = 0.0f;
	float						m_FarWindowHeight = 0.0f;

	bool						m_ViewDirty = true;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4			m_xmf4x4View;
	DirectX::XMFLOAT4X4			m_xmf4x4Proj;

	//ºäÆ÷Æ®¿Í ¾¾Àú »ç°¢Çü
	D3D12_VIEWPORT				m_d3dViewport;
	D3D12_RECT					m_d3dScissorRect; 

private:
	ID3D12Resource*				m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO*			m_pcbMappedCamera = NULL;

private:
	vector<LIGHT*>				m_Lights;

private:	// For Shake
	CGameTimer					m_TimerForShake;
	bool						m_isOnShake = false;
	float						m_ShakePower = 0.0f;
	float						m_ShakeTime = 0.0f;
	XMFLOAT3					m_xmf3PrevPos;

public:
	CCamera();
	~CCamera();

	// for Update Loop
	void Update(float elapsedTime);
	void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed, CPlayer* player);

	// Get/Set world camera position.
	DirectX::XMVECTOR GetPosition()const;
	DirectX::XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// Get camera basis vectors.
	DirectX::XMVECTOR GetRight()const;
	DirectX::XMFLOAT3 GetRight3f()const;
	DirectX::XMVECTOR GetUp()const;
	DirectX::XMFLOAT3 GetUp3f()const;
	DirectX::XMVECTOR GetLook()const;
	DirectX::XMFLOAT3 GetLook3f()const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Get/Set Camera speed.
	float GetSpeed() const { return m_Speed; }
	void SetSpeed(float speed) { m_Speed = speed; }

	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);
	void SetLens(float fovY, float witdh, float height, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
	void LookAt(const XMFLOAT3& lookAt, const XMFLOAT3& up);

	// Get View/Proj matrices.
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);
	void UpDown(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

public:
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight,
		float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex = 1);

public:
	void SetLight(LIGHT* light) { if (light) m_Lights.push_back(light); }

public:
	void SetShake(bool isOnShake, float shakeTime, float power);
};