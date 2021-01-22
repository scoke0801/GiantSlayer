#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

protected:
	XMFLOAT3				m_xmf3Position;
	XMFLOAT3				m_xmf3Velocity;

	int						m_nReferences = 0;

	XMFLOAT4X4				m_xmf4x4World;
	CMesh*					m_pMesh = NULL;
	CShader*				m_pShader = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetMesh(CMesh* pMesh);
	void SetShader(CShader* pShader);

	virtual void Update() {};
	virtual void Animate(float fTimeElapsed);
	virtual void Move(XMFLOAT3 pos);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	virtual void OnPrepareRender();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();

	void SetPosition(XMFLOAT3 pos);
	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
	bool CollisionCheck() { return false; };
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
