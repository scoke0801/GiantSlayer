#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

string ConvertToObjectName(const OBJ_NAME& name)
{
	switch (name)
	{
	case OBJ_NAME::None:
		return "None";
	case OBJ_NAME::Box:
		return "Box";
	case OBJ_NAME::Terrain:
		return "Terrain";
	default:
		assert(!"UnDefinedObjectName");
	}
}

CGameObject::CGameObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}
CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}
void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();

	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();

}
void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();

	m_pMesh = pMesh;

	if (m_pMesh) m_pMesh->AddRef();
}
void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, 
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObjInfo);
}
void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{ 
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World))); 
	::memcpy(&m_pcbMappedGameObjInfo->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4)); 
	::memcpy(&m_pcbMappedGameObjInfo->m_Material, m_Material, sizeof(MATERIAL));
	::memcpy(&m_pcbMappedGameObjInfo->m_nTextureIndex, &m_nTextureIndex, sizeof(UINT)); 
}
void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}
void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	//if (m_Material)
	//{
	//	m_Material->UpdateShaderVariables(pd3dCommandList, m_MaterialParameterIndex);
	//}
	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}

void CGameObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3Position = pos;

	m_xmf4x4World._41 = pos.x;
	m_xmf4x4World._42 = pos.y;
	m_xmf4x4World._43 = pos.z;
}

void CGameObject::SetVelocity(XMFLOAT3 pos)
{
	m_xmf3Velocity = pos;
}

void CGameObject::SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents)
{	
	//m_pMesh->
}

void CGameObject::Move(XMFLOAT3 pos)
{
	SetPosition(Vector3::Add(GetPosition(), pos));
}

void CGameObject::Move()
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

CRotatingObject::CRotatingObject()
{
	m_Name = OBJ_NAME::None;
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}
CRotatingObject::~CRotatingObject()
{
}
void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}
  
CBox::CBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	
	m_Name = OBJ_NAME::Box;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);

	SetMesh(pCubeMeshTex);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CBox::~CBox()
{
}

