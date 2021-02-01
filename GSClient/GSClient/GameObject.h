#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

enum class OBJ_NAME
{
	None = 0,
	Terrain = 1,
	Box = 2
};

string ConvertToObjectName(const OBJ_NAME& name);

struct GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	MATERIAL						m_Material;
	UINT							m_nTextureIndex;
};

class CGameObject
{
private:
	int					m_nReferences = 0;
	 
protected:
	XMFLOAT4X4			m_xmf4x4World;

	XMFLOAT3			m_xmf3Position;
	XMFLOAT3			m_xmf3Velocity;

	CMesh*				m_pMesh = NULL;
	CShader*			m_pShader = NULL;

	UINT				m_nTextureIndex = 0x00;
	 
	MATERIAL*			m_Material;
	 
	OBJ_NAME			m_Name;

private:
	ID3D12Resource*		m_pd3dcbGameObject = NULL;
	GAMEOBJECT_INFO*	m_pcbMappedGameObjInfo = NULL;
	 
	UINT		m_nTextureIndex = 0x00;
	 
//CMaterial*	m_Material = nullptr;
//int			m_MaterialParameterIndex = 2;
	 
public:
	CGameObject();
	virtual ~CGameObject();


public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}

	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
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
	string GetObjectName() const { return ConvertToObjectName(m_Name); }

	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	void SetPosition(XMFLOAT3 pos);
	void SetVelocity(XMFLOAT3 pos);
	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
	void SetTextureIndex(UINT index) { m_nTextureIndex = index; }
	//void SetMaterial(CMaterial* pMaterial, int rootParameterIndex) { m_Material = pMaterial; m_MaterialParameterIndex = rootParameterIndex; }
	void SetObjectName(const OBJ_NAME& name) { m_Name = name; }
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

