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
protected:
	// Camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3			m_xmf3Position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3			m_xmf3Right = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3			m_xmf3Up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3			m_xmf3Look = { 0.0f, 0.0f, 1.0f };

	XMFLOAT4X4					m_xmf4x4ViewProjection;

	float						m_Speed = 100.0f;

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

private:
	CPlayer*					m_TargetPlayer = nullptr;
	XMFLOAT4X4					m_TargetTransform;
	XMFLOAT3					m_xmf3Offset = { 0.0f, 0.0f, 0.0f };
	 
public:
	CCamera();
	~CCamera();

	// for Update Loop
	void Update(float elapsedTime);
	void Update(const XMFLOAT3& xmf3LookAt);
	void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	void UpdateLights(float elapsedTime);

	// Get/Set world camera position.
	DirectX::XMVECTOR GetPosition()const;
	DirectX::XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);
	void GenerateViewMatrix();

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

	void SetRight(XMFLOAT3 xmf3Vector) { m_xmf3Right = xmf3Vector; }
	void SetUp(XMFLOAT3 xmf3Vector) { m_xmf3Up = xmf3Vector; }
	void SetLook(XMFLOAT3 xmf3Vector) { m_xmf3Look = xmf3Vector; }

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
	void LookAt(const XMFLOAT3& lookAt, const XMFLOAT3& up);

	// Get View/Proj matrices.
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	void SetTarget(CPlayer* target); 
	CPlayer* GetTarget() const { return m_TargetPlayer; }

	void SetOffset(XMFLOAT3 offset);
	XMFLOAT3 GetOffset() const { return m_xmf3Offset; }

	void MoveOffset(XMFLOAT3 shift);
public:
	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);
	void UpDown(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);
	void RotateAroundTarget(XMFLOAT3 pxmf3Axis, float fAngle);
	   
	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

public:
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight,
		float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex = 1);

public:
	void SetLight(LIGHT* light) { if (light) m_Lights.push_back(light); }

public:
	void SetShake(bool isOnShake, float shakeTime, float power);

public:
	XMFLOAT3 CalcTargetRight();
	XMFLOAT3 CalcTargetUp();
	XMFLOAT3 CalcTargetLook();
};

struct VS_CB_LIGHT_CAMERA_INFO
{
	XMFLOAT4X4	m_xmf4x4ViewProjection;
	XMFLOAT4X4	m_xmf4x4ShadowTransform;
};

class CLightCamera : public CCamera
{
protected:
	ID3D12Resource* m_pd3dcbLightCamera;
	VS_CB_LIGHT_CAMERA_INFO* m_pcbMappedLightCamera;

public:
	CLightCamera();
	virtual ~CLightCamera();

	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void ReleaseShaderVariables() override;
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex = 1) override;
};