#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

class CGameObject
{
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();
private:
	int			m_nReferences = 0;
	int			m_nMeshes;
	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Velocity;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
protected:
	XMFLOAT4X4 m_xmf4x4World;
	CMesh* m_pMesh = NULL;
	CMesh** m_ppMeshes;
	CShader* m_pShader = NULL;

	ID3D12Resource* m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	void Update();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
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

// 스카이박스
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

