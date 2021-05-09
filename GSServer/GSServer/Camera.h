#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CPlayer;

class CCamera
{
private:
	// Camera coordinate system with coordinates relative to world space.
	XMFLOAT3			m_xmf3Position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3			m_xmf3Right = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3			m_xmf3Up = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3			m_xmf3Look = { 0.0f, 0.0f, 1.0f };

	float				m_Speed = 100.0f;

	// Cache frustum properties.
	float				m_NearZ = 0.0f;
	float				m_FarZ = 0.0f;
	float				m_Aspect = 0.0f;
	float				m_FovY = 0.0f;
	float				m_NearWindowHeight = 0.0f;
	float				m_FarWindowHeight = 0.0f;

	bool				m_ViewDirty = true;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4			m_xmf4x4View;
	DirectX::XMFLOAT4X4			m_xmf4x4Proj;

private:
	CPlayer*			m_TargetPlayer = nullptr; 
	XMFLOAT4X4			m_TargetTransform;
	XMFLOAT3			m_xmf3Offset = { 0.0f, 0.0f, 0.0f };

private:
	// 각 축으로 회전한 각도
	XMFLOAT3					m_RotatedAngle = { 0.0f, 0.0f, 0.0f };

public:
	CCamera();
	~CCamera();

public:
	// for Update Loop
	void Update(float elapsedTime);
	void Update(const XMFLOAT3& xmf3LookAt);
	void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);

public:
	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
	void LookAt(const XMFLOAT3& lookAt, const XMFLOAT3& up);

	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);
	void SetLens(float fovY, float witdh, float height, float zn, float zf);

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
	// Get camera basis vectors.
	DirectX::XMVECTOR GetRight()const { return XMLoadFloat3(&m_xmf3Right); }
	DirectX::XMFLOAT3 GetRight3f()const { return m_xmf3Right; }
	DirectX::XMVECTOR GetUp()const { return XMLoadFloat3(&m_xmf3Up); }
	DirectX::XMFLOAT3 GetUp3f()const { return m_xmf3Up; }
	DirectX::XMVECTOR GetLook()const { return XMLoadFloat3(&m_xmf3Look); }
	DirectX::XMFLOAT3 GetLook3f()const { return m_xmf3Look; }

	// Get/Set world camera position.
	DirectX::XMVECTOR GetPosition()const { return XMLoadFloat3(&m_xmf3Position); }
	DirectX::XMFLOAT3 GetPosition3f()const { return m_xmf3Position; }
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	void SetTarget(CPlayer* target);
	CPlayer* GetTarget() const { return m_TargetPlayer; }

	// Get/Set Camera speed.
	float GetSpeed() const { return m_Speed; }
	void SetSpeed(float speed) { m_Speed = speed; }

	void SetOffset(XMFLOAT3 offset);
	XMFLOAT3 GetOffset() const { return m_xmf3Offset; }

	void MoveOffset(XMFLOAT3 shift);
private: 
	XMFLOAT3 CalcTargetRight()  { return XMFLOAT3(m_TargetTransform._11, m_TargetTransform._12, m_TargetTransform._13); }
	XMFLOAT3 CalcTargetUp() { return XMFLOAT3(m_TargetTransform._21, m_TargetTransform._22, m_TargetTransform._23); }
	XMFLOAT3 CalcTargetLook() { return XMFLOAT3(m_TargetTransform._31, m_TargetTransform._32, m_TargetTransform._33); }
};

