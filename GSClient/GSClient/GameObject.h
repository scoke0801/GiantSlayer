#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

class CGameObject
{
private:
	int			m_nReferences = 0;
	 
protected:
	XMFLOAT4X4	m_xmf4x4World;

	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Velocity;

	CMesh*		m_pMesh = NULL;
	CShader*	m_pShader = NULL;

public:
	FbxScene*	m_pfbxScene = NULL;

public:
	CGameObject();
	virtual ~CGameObject();

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	
	void ReleaseUploadBuffers();

public:
	virtual void Animate(float fTimeElapsed);
	virtual void Update() {};

	virtual void OnPrepareRender();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	virtual void Move(XMFLOAT3 pos);
	void Move();
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	bool CollisionCheck() { return false; };

public:
	XMFLOAT3 GetPosition() { return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }

	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	void SetPosition(XMFLOAT3 pos);
	void SetVelocity(XMFLOAT3 pos);
	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
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

class CBox : public CGameObject
{
public:
	CBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float width, float height, float depth);
	virtual ~CBox();

private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed) {}
};

//////////////////////////////////////////////////////////////////////////////
//

class CTree : public CGameObject
{
public:
	CTree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, FbxScene* pfbxScene);
	virtual ~CTree();
};