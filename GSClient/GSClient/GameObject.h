#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Velocity;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
protected:
	XMFLOAT4X4 m_xmf4x4World;
	CMesh* m_pMesh = NULL;
	CShader* m_pShader = NULL;
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	void Update();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void SetPosition(XMFLOAT3 pos);
	void SetVelocity(XMFLOAT3 pos);

	void Move();

public:
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed);
};
