#include "stdafx.h"
#include "Mesh.h"
#include "Particle.h"
#include "GameObjectVer2.h"

CParticleVertex::CParticleVertex(const XMFLOAT3& xmf3Position, const XMFLOAT4& xmf4Diffuse, const XMFLOAT2& xmf2Time)
{
	m_xmf3Position = xmf3Position;
	m_xmf4Diffuse = xmf4Diffuse;
	m_xmf2Time = xmf2Time;
}
CParticleTextureVertex::CParticleTextureVertex(const XMFLOAT3& xmf3Position, const XMFLOAT2& xmf2Time,
 UINT textureCode)
{
	m_xmf3Position = xmf3Position; m_xmf2Time = xmf2Time; m_nTexture = textureCode;
}

#pragma region About Basic Meshes
CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
};

void CMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//메쉬의 프리미티브 유형을 설정한다.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//메쉬의 정점 버퍼 뷰를 설정한다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}

}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	bool isHeightHalfOn,
	float fWidth , float fHeight , float fDepth )
	: CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 6개의 면 가로(x-축) 길이
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	float fyStart = fy, fyEnd = -fy;
	if (isHeightHalfOn)
	{
		fyStart = fHeight * 0.5;
		fyEnd = -(fHeight * 0.5f);
	}
	CDiffusedVertex pVertices[36];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, -fz), XMFLOAT4(1, 0, 0, 0));
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, -fz), XMFLOAT4(1, 0, 0, 0));
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, -fz), XMFLOAT4(0, 0, 1, 0));
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, -fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, -fz), XMFLOAT4(0, 0, 1, 0));
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, +fz), RANDOM_COLOR);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, +fz), RANDOM_COLOR);
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, +fz), RANDOM_COLOR);
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, +fz), RANDOM_COLOR);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, -fz), RANDOM_COLOR);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, fyEnd, +fz), RANDOM_COLOR);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, +fz), RANDOM_COLOR);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyStart, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, fyEnd, -fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//정점 버퍼 뷰를 생성한다. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	bool isHeightHalfOn,
	float fWidth, float fHeight, float fDepth,
	const XMFLOAT3& shift) 
	: CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 6개의 면 가로(x-축) 길이
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	float fyStart = fy, fyEnd = -fy;
	float fxStart = fx, fxEnd = -fx;
	float fzStart = fz, fzEnd = -fz; 

	fxStart += shift.x;
	fxEnd += shift.x;
	fyStart += shift.y;
	fyEnd += shift.y;
	fzStart += shift.z;
	fzEnd += shift.z;

	BoundingOrientedBox;
	BoundingBox;
	CDiffusedVertex pVertices[36];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), XMFLOAT4(1, 0, 0, 0));
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), XMFLOAT4(1, 0, 0, 0));
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), XMFLOAT4(0, 0, 1, 0));
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), XMFLOAT4(0, 0, 1, 0));
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), RANDOM_COLOR);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), RANDOM_COLOR);
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//정점 버퍼 뷰를 생성한다. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	PulledModel pulledModelInfo, 
	float fWidth, float fHeight, float fDepth)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 6개의 면 가로(x-축) 길이
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	float fyStart = fy, fyEnd = -fy;
	float fxStart = fx, fxEnd = -fx;
	if (pulledModelInfo == PulledModel::Left)
	{
		fxStart = 0;
		fxEnd = fWidth;
	}
	else if (pulledModelInfo == PulledModel::Right) {
		fxStart = 0;
		fxEnd = -fWidth;
	}
	else if (pulledModelInfo == PulledModel::Top) {
		fyStart = 0;
		fyEnd = fHeight;
	}
	else if (pulledModelInfo == PulledModel::Bottom) {
		fyStart = 0;
		fyEnd = -fHeight;
	}
	BoundingOrientedBox;
	BoundingBox;
	CDiffusedVertex pVertices[36];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, -fz), XMFLOAT4(1, 0, 0, 0));
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, -fz), XMFLOAT4(1, 0, 0, 0));
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, -fz), XMFLOAT4(0, 0, 1, 0));
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, -fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, -fz), XMFLOAT4(0, 0, 1, 0));
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, +fz), RANDOM_COLOR);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, +fz), RANDOM_COLOR);
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, +fz), RANDOM_COLOR);
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, +fz), RANDOM_COLOR);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, -fz), RANDOM_COLOR);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, +fz), RANDOM_COLOR);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, +fz), RANDOM_COLOR);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, -fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//정점 버퍼 뷰를 생성한다. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	PulledModel pulledModelInfo, 
	float fWidth, float fHeight, float fDepth,
	const XMFLOAT3& shift) : CMesh(pd3dDevice, pd3dCommandList)
{	//직육면체는 6개의 면 가로(x-축) 길이
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//fWidth: 직육면체 가로(x-축) 길이, fHeight: 직육면체 세로(y-축) 길이, fDepth: 직육면체 깊이(z-축) 길이
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	float fyStart = fy, fyEnd = -fy;
	float fxStart = fx, fxEnd = -fx;
	float fzStart = fz, fzEnd = -fz;
	if (pulledModelInfo == PulledModel::Left)
	{
		fxStart = 0;
		fxEnd = fWidth; 
	}
	else if (pulledModelInfo == PulledModel::Right) {
		fxStart = 0;
		fxEnd = -fWidth;
	}
	else if (pulledModelInfo == PulledModel::Top) {
		fyStart = 0;
		fyEnd = fHeight;
	}
	else if (pulledModelInfo == PulledModel::Bottom) {
		fyStart = 0;
		fyEnd = -fHeight;
	}
	fxStart += shift.x;
	fxEnd += shift.x;
	fyStart += shift.y;
	fyEnd += shift.y; 
	fzStart += shift.z;
	fzEnd += shift.z;

	BoundingOrientedBox;
	BoundingBox;
	CDiffusedVertex pVertices[36];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), XMFLOAT4(1, 0, 0, 0));
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), XMFLOAT4(1, 0, 0, 0));
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), XMFLOAT4(0, 0, 1, 0));
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), XMFLOAT4(0, 0, 1, 0));
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), RANDOM_COLOR);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), RANDOM_COLOR);
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxEnd, fyEnd, fzStart), RANDOM_COLOR);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyStart, fzEnd), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzStart), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(fxStart, fyEnd, fzEnd), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//정점 버퍼 뷰를 생성한다. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{

}

CCubeMeshTextured::CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth
	, bool sameBackFace)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CTexturedVertex pVertices[36];
	int i = 0;

	XMFLOAT3 normals[6] = {
		XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)
	};
	///////////////////////////////////////////////////////////////////////////////////////Front
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), normals[0]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f), normals[0]);
	///////////////////////////////////////////////////////////////////////////////////////Top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f), normals[1]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f), normals[1]);
///////////////////////////////////////////////////////////////////////////////////////Back
	if (sameBackFace)
	{
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[2]);

		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), normals[2]);
	}
	else
	{
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[2]);

		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[2]);
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f), normals[2]);
	}
	///////////////////////////////////////////////////////////////////////////////////////Bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[3]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f), normals[3]);
	///////////////////////////////////////////////////////////////////////////////////////Left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), normals[4]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f),	normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f),	normals[4]);
	///////////////////////////////////////////////////////////////////////////////////////Right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[5]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f), normals[5]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshTextured::CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	XMFLOAT3 startPos, 
	float fWidth, float fHeight, float fDepth)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CTexturedVertex pVertices[36];
	int i = 0;

	XMFLOAT3 normals[6] = {
		XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////front
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z-fz), XMFLOAT2(0.0f, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z-fz), XMFLOAT2(1.0f, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z-fz), XMFLOAT2(1.0f, 1.0f), normals[0]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(0.0f, 1.0f), normals[0]);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(1.0f, 0.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f), normals[1]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(0.0f, 1.0f), normals[1]);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////back
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 0.0f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f), normals[2]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 1.0f), normals[2]);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(1.0f, 0.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f), normals[3]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(0.0f, 1.0f), normals[3]);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(1.0f, 0.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f), normals[4]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(0.0f, 1.0f), normals[4]);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(1.0f, 0.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f), normals[5]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(0.0f, 1.0f), normals[5]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshTextured::~CCubeMeshTextured()
{

}

CTexturedRectMesh::CTexturedRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float fx = (fWidth * 0.5f) + fxPosition, fy = (fHeight * 0.5f) + fyPosition, fz = (fDepth * 0.5f) + fzPosition;

	if (fWidth == 0.0f)
	{
		if (fxPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fHeight == 0.0f)
	{
		if (fyPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fDepth == 0.0f)
	{
		if (fzPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
	}

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CTexturedRectMesh::~CTexturedRectMesh()
{

}

CBillboardMesh::CBillboardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float sizeX, float sizeY)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 1;
	m_nStride = sizeof(CBillboardVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	CBillboardVertex* pVertice = new CBillboardVertex;
	pVertice->m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pVertice->m_xmf2Size = XMFLOAT2(sizeX, sizeY);
	pVertice->m_nTexture = 1;
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertice, 
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CBillboardMesh::CBillboardMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, 
	CBillboardVertex* pGeometryBillboardVertices,
	UINT nGeometryBillboardVertices)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = nGeometryBillboardVertices;
	m_nStride = sizeof(CBillboardVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pGeometryBillboardVertices, m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 
}

CBillboardMesh::~CBillboardMesh()
{

}

CPlaneMeshDiffused::CPlaneMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, 
	float fWidth, float fHeight, float fDepth,
	XMFLOAT4 xmf4Color,
	bool isVertical)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex pVertices[6];

	float widthHalf = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;

	if (isVertical)
	{
		pVertices[0] = CDiffusedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), xmf4Color);
		pVertices[1] = CDiffusedVertex(XMFLOAT3( widthHalf,  heightHalf, fDepth), xmf4Color);
		pVertices[2] = CDiffusedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), xmf4Color);
		pVertices[3] = CDiffusedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), xmf4Color);
		pVertices[4] = CDiffusedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), xmf4Color);
		pVertices[5] = CDiffusedVertex(XMFLOAT3(-widthHalf, -heightHalf, fDepth), xmf4Color);
	}
	else	// horizon
	{
		pVertices[0] = CDiffusedVertex(XMFLOAT3(-widthHalf, 0.0,  heightHalf), xmf4Color);
		pVertices[1] = CDiffusedVertex(XMFLOAT3( widthHalf, 0.0,  heightHalf), xmf4Color);
		pVertices[2] = CDiffusedVertex(XMFLOAT3( widthHalf, 0.0, -heightHalf), xmf4Color);
		pVertices[3] = CDiffusedVertex(XMFLOAT3(-widthHalf, 0.0,  heightHalf), xmf4Color);
		pVertices[4] = CDiffusedVertex(XMFLOAT3( widthHalf, 0.0, -heightHalf), xmf4Color);
		pVertices[5] = CDiffusedVertex(XMFLOAT3(-widthHalf, 0.0, -heightHalf), xmf4Color);
	}

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CPlaneMeshDiffused::~CPlaneMeshDiffused()
{

}

CPlaneMeshTextured::CPlaneMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float fWidth, float fHeight, float fDepth,
	bool isVertical,
	bool isHalfSize)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];
	
	float widthHalf = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;
	float uvXStart = 0.0f, uvXEnd = 1.0f;
	float uvYStart = 0.0f, uvYEnd = 1.0f;
	if (isHalfSize)
	{
		//uvXEnd = 0.5f; 
		uvYEnd = 0.5f;
	}
	if (isVertical)
	{
		pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
		pVertices[1] = CTexturedVertex(XMFLOAT3( widthHalf,  heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYStart));
		pVertices[2] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYEnd));
		pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
		pVertices[4] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYEnd));
		pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXStart, uvYEnd));
	}
	else	// horizon
	{
		pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf, 0.0, heightHalf),  XMFLOAT2(uvXStart, uvYStart));
		pVertices[1] = CTexturedVertex(XMFLOAT3(widthHalf, 0.0, heightHalf),   XMFLOAT2(uvXEnd, uvYStart));
		pVertices[2] = CTexturedVertex(XMFLOAT3(widthHalf, 0.0, -heightHalf),  XMFLOAT2(uvXEnd, uvYEnd));
		pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf, 0.0, heightHalf),  XMFLOAT2(uvXStart, uvYStart));
		pVertices[4] = CTexturedVertex(XMFLOAT3(widthHalf, 0.0, -heightHalf),  XMFLOAT2(uvXEnd, uvYEnd));
		pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, 0.0, -heightHalf), XMFLOAT2(uvXStart, uvYEnd));
	}

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, 
		pVertices, m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices; 
}

CPlaneMeshTextured::CPlaneMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	XMFLOAT3 startPos,
	float fWidth, float fHeight, float fDepth, 
	bool isVertical) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float widthHalf  = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;

	if (isVertical)
	{
		pVertices[0] = CTexturedVertex(XMFLOAT3(startPos.x + -widthHalf, startPos.y +  heightHalf, fDepth), XMFLOAT2(0.0f, 0.0f));
		pVertices[1] = CTexturedVertex(XMFLOAT3(startPos.x +  widthHalf, startPos.y +  heightHalf, fDepth), XMFLOAT2(1.0f, 0.0f));
		pVertices[2] = CTexturedVertex(XMFLOAT3(startPos.x +  widthHalf, startPos.y + -heightHalf, fDepth), XMFLOAT2(1.0f, 1.0f));
		pVertices[3] = CTexturedVertex(XMFLOAT3(startPos.x + -widthHalf, startPos.y +  heightHalf, fDepth), XMFLOAT2(0.0f, 0.0f));
		pVertices[4] = CTexturedVertex(XMFLOAT3(startPos.x +  widthHalf, startPos.y + -heightHalf, fDepth), XMFLOAT2(1.0f, 1.0f));
		pVertices[5] = CTexturedVertex(XMFLOAT3(startPos.x + -widthHalf, startPos.y + -heightHalf, fDepth), XMFLOAT2(0.0f, 1.0f));
	}
	else	// horizon
	{
		pVertices[0] = CTexturedVertex(XMFLOAT3(startPos.x + -widthHalf, fDepth, startPos.y +  heightHalf), XMFLOAT2(0.0f, 0.0f));
		pVertices[1] = CTexturedVertex(XMFLOAT3(startPos.x + widthHalf,  fDepth, startPos.y +  heightHalf), XMFLOAT2(1.0f, 0.0f));
		pVertices[2] = CTexturedVertex(XMFLOAT3(startPos.x + widthHalf,  fDepth, startPos.y + -heightHalf), XMFLOAT2(1.0f, 1.0f));
		pVertices[3] = CTexturedVertex(XMFLOAT3(startPos.x + -widthHalf, fDepth, startPos.y +  heightHalf), XMFLOAT2(0.0f, 0.0f));
		pVertices[4] = CTexturedVertex(XMFLOAT3(startPos.x + widthHalf,  fDepth, startPos.y + -heightHalf), XMFLOAT2(1.0f, 1.0f));
		pVertices[5] = CTexturedVertex(XMFLOAT3(startPos.x + -widthHalf, fDepth, startPos.y + -heightHalf), XMFLOAT2(0.0f, 1.0f));
	}

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CPlaneMeshTextured::CPlaneMeshTextured(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	float uvXStart, float uvYStart,
	float uvXEnd, float uvYEnd,
	float fWidth, float fHeight, float fDepth)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float widthHalf = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;
	 
	pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
	pVertices[1] = CTexturedVertex(XMFLOAT3( widthHalf,  heightHalf, fDepth), XMFLOAT2(uvXEnd,   uvYStart));
	pVertices[2] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd,   uvYEnd));
	pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
	pVertices[4] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd,   uvYEnd));
	pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXStart, uvYEnd));
	 
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}


CPlaneMeshTextured::~CPlaneMeshTextured()
{
}
#pragma endregion
#pragma region About FBXMeshes
//////////////////////////////////////////////////////////////////////////////
//

FbxScene* LoadFbxSceneFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	FbxScene* pfbxScene = NULL;

	int nSDKMajor, nSDKMinor, nSDKRevision;
	FbxManager::GetFileFormatVersion(nSDKMajor, nSDKMinor, nSDKRevision);

	FbxIOSettings* pfbxIOSettings = pfbxSdkManager->GetIOSettings();
	FbxImporter* pfbxImporter = FbxImporter::Create(pfbxSdkManager, " ");
	bool bImportStatus = pfbxImporter->Initialize(pstrFbxFileName, -1, pfbxIOSettings);

	int nFileFormatMajor, nFileFormatMinor, nFileFormatRevision;
	pfbxImporter->GetFileVersion(nFileFormatMajor, nFileFormatMinor, nFileFormatRevision);

	pfbxScene = FbxScene::Create(pfbxSdkManager, " ");
	bool bStatus = pfbxImporter->Import(pfbxScene);

	FbxGeometryConverter fbxGeomConverter(pfbxSdkManager);
	fbxGeomConverter.Triangulate(pfbxScene, true);

	FbxSystemUnit fbxSceneSystemUnit = pfbxScene->GetGlobalSettings().GetSystemUnit();
	if (fbxSceneSystemUnit.GetScaleFactor() != 1.0) FbxSystemUnit::cm.ConvertScene(pfbxScene);

	pfbxImporter->Destroy();

	return(pfbxScene);
}

CFixedMesh::CFixedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	string fileName) : CMesh(pd3dDevice, pd3dCommandList)
{
	string filePath = "resources/FbxExported/" + fileName + ".bin";
	LoadFile(filePath);

	// Vertices
	vector<CTexturedVertex> tempVertex;

	for (int i = 0; i < vertices.size(); i++) {
		CTexturedVertex tempVert;
		tempVert.m_xmf3Position = vertices[i].pos;
		tempVert.m_xmf2TexCoord = vertices[i].uv;
		tempVert.m_xmf3Normal = vertices[i].normal;
		tempVertex.push_back(tempVert);
	}

	m_nVertices = tempVertex.size();
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, tempVertex.data(),
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	// Indices
	m_nIndices = indices.size();

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, indices.data(),
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CFixedMesh::~CFixedMesh()
{

}

void CFixedMesh::LoadFile(string fileName)
{
	ifstream file;
	file.open(fileName, ios::in | ios::binary);
	string temp;

	file >> temp >> nVertices;
	file >> temp >> nPolygons;
	file >> temp >> nBones;
	file >> temp;

	cout << "[VT]:" << nVertices << "\t[PG]:" << nPolygons << "\t[FName]:" << fileName << endl;

	for (int i = 0; i < nVertices; i++) {
		Vertex tempVertex;

		file >> tempVertex.pos.x >> tempVertex.pos.y >> tempVertex.pos.z >>
			tempVertex.uv.x >> tempVertex.uv.y >>
			tempVertex.normal.x >> tempVertex.normal.y >> tempVertex.normal.z;

		vertices.push_back(tempVertex);
	}
	file >> temp;

	for (int i = 0; i < (nPolygons * 3); i++) {
		int tempInt;
		file >> tempInt;
		indices.push_back(tempInt);
	}

	file.close();
}

CAnimatedMesh::CAnimatedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	vector<Vertex> vertices, vector<int> indices, vector<Bone> skeletons) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = vertices.size();
	m_nBones = skeletons.size();
	m_nIndices = indices.size();
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	XMFLOAT3* pos = new XMFLOAT3[m_nVertices];
	XMFLOAT2* uv = new XMFLOAT2[m_nVertices];
	XMFLOAT3* normal = new XMFLOAT3[m_nVertices];
	XMFLOAT4* boneWeights = new XMFLOAT4[m_nVertices];
	XMUINT4* boneIndices = new XMUINT4[m_nVertices];

	for (int i = 0; i < m_nVertices; i++) {
		pos[i] = vertices[i].pos;
		uv[i] = vertices[i].uv;
		normal[i] = vertices[i].normal;

		boneWeights[i].x = vertices[i].weights[0];
		boneWeights[i].y = vertices[i].weights[1];
		boneWeights[i].z = vertices[i].weights[2];
		boneWeights[i].w = vertices[i].weights[3];
		
		boneIndices[i].x = vertices[i].indices[0];
		boneIndices[i].y = vertices[i].indices[1];
		boneIndices[i].z = vertices[i].indices[2];
		boneIndices[i].w = vertices[i].indices[3];

		/*cout << boneWeights[i].x << " " << boneWeights[i].y << " " << boneWeights[i].z << " " <<
			boneIndices[i].x << " " << boneIndices[i].y << " " << boneIndices[i].z << " " << boneIndices[i].w << endl;*/
	}

	// Pos
	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pos,
		sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	
	// Uv
	m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, uv,
		sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
	m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
	m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
	
	// Normal
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, normal,
		sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	// BoneWeight
	m_pd3dBoneWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, boneWeights,
		sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneWeightUploadBuffer);
	m_d3dBoneWeightBufferView.BufferLocation = m_pd3dBoneWeightBuffer->GetGPUVirtualAddress();
	m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dBoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	// BoneIndex
	m_pd3dBoneIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, boneIndices,
		m_nVertices * sizeof(XMUINT4), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndexUploadBuffer);
	m_d3dBoneIndexBufferView.BufferLocation = m_pd3dBoneIndexBuffer->GetGPUVirtualAddress();
	m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
	m_d3dBoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * m_nVertices;

	// VertexIndices
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, indices.data(),
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CAnimatedMesh::~CAnimatedMesh()
{

}

void CAnimatedMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dBoneWeightBufferView, m_d3dBoneIndexBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);

	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	} 
}

CMeshFbx::CMeshFbx(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	FbxManager* pfbxSdkManager, char* pstrFbxFileName,
	bool rotateFlag)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	FbxScene* m_pfbxScene = FbxScene::Create(pfbxSdkManager, "");
	m_pfbxScene = LoadFbxSceneFromFile(pd3dDevice, pd3dCommandList, pfbxSdkManager, pstrFbxFileName);

	FbxGeometryConverter geometryConverter(pfbxSdkManager);
	geometryConverter.Triangulate(m_pfbxScene, true);

	FbxAxisSystem sceneAxisSystem = m_pfbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem::DirectX.ConvertScene(m_pfbxScene);

	CMeshinfo fbxmesh;
	fbxmesh.vertics = 0;
	CMeshinfo* temp = &fbxmesh;

	cout << "-메쉬 로드:" << pstrFbxFileName << endl;

	LoadMesh(m_pfbxScene->GetRootNode(), temp, rotateFlag);

	m_nVertices = fbxmesh.vertics;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	cout << "-메쉬 로드 끝 ||| [정점]: " << m_nVertices << "개 " << endl;

	CTexturedVertex* pVertices = new CTexturedVertex[fbxmesh.vertex.size()];
	copy(fbxmesh.vertex.begin(), fbxmesh.vertex.end(), pVertices);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//정점 버퍼 뷰를 생성한다. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CMeshFbx::~CMeshFbx()
{

}

void CMeshFbx::LoadMesh(FbxNode* node, CMeshinfo* info, bool rotateFlag)
{
	FbxNodeAttribute* pfbxNodeAttribute = node->GetNodeAttribute();

	if ((pfbxNodeAttribute != NULL) &&
		(pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = node->GetMesh();

		//info->vertics += pfbxMesh->GetControlPointsCount();
		int nPolygons = pfbxMesh->GetPolygonCount();
		int v = pfbxMesh->GetControlPointsCount();
		int vv = info->vertics;
		int vvv = v + vv;

		cout << "[메쉬발견] 현재 노드 정점 수: " << v << "  기존 정점 수: " << vv << "  합: " << vvv << endl;

		for (int pindex = 0; pindex < nPolygons; pindex++) {
			for (int vindex = 0; vindex < 3; vindex++) {
				int pvindex = pfbxMesh->GetPolygonVertex(pindex, vindex);
				int uvindex = pfbxMesh->GetTextureUVIndex(pindex, vindex, FbxLayerElement::eTextureDiffuse);

				FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
				FbxLayerElementUV* fbxLayerUV = pfbxMesh->GetLayer(0)->GetUVs();

				fbxUV = fbxLayerUV->GetDirectArray().GetAt(uvindex);

				float uv1 = fbxUV[0];
				float uv2 = 1.0f - fbxUV[1];

				float y = pfbxMesh->GetControlPointAt(pvindex).mData[2];
				float z = pfbxMesh->GetControlPointAt(pvindex).mData[1];
				if (rotateFlag)
				{
					y = pfbxMesh->GetControlPointAt(pvindex).mData[1];
					z = pfbxMesh->GetControlPointAt(pvindex).mData[2];
				}
				
				FbxGeometryElementNormal* pnormal = pfbxMesh->GetElementNormal(0);

				info->vertex.push_back(
					CTexturedVertex(
						XMFLOAT3(
							pfbxMesh->GetControlPointAt(pvindex).mData[0],
							y,
							z
						),
						XMFLOAT2(
							uv1,
							uv2
						),
						XMFLOAT3(
							pnormal->GetDirectArray().GetAt(pvindex).mData[0],
							pnormal->GetDirectArray().GetAt(pvindex).mData[1],
							pnormal->GetDirectArray().GetAt(pvindex).mData[2]
						)
					)
				);
			}
		}
		info->vertics += nPolygons * 3;
	}

	int nChilds = node->GetChildCount();
	cout << "연결된 차일드 노드 수: " << nChilds << endl;
	for (int i = 0; i < nChilds; i++)
		LoadMesh(node->GetChild(i), info, rotateFlag);
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardMesh::CStandardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CStandardMesh::~CStandardMesh()
{
	if (m_pd3dTextureCoord0Buffer) m_pd3dTextureCoord0Buffer->Release();
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dTangentBuffer) m_pd3dTangentBuffer->Release();
	if (m_pd3dBiTangentBuffer) m_pd3dBiTangentBuffer->Release();

	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;
	if (m_pxmf3Tangents) delete[] m_pxmf3Tangents;
	if (m_pxmf3BiTangents) delete[] m_pxmf3BiTangents;
	if (m_pxmf2TextureCoords0) delete[] m_pxmf2TextureCoords0;
	if (m_pxmf2TextureCoords1) delete[] m_pxmf2TextureCoords1;
}

void CStandardMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;

	if (m_pd3dTangentUploadBuffer) m_pd3dTangentUploadBuffer->Release();
	m_pd3dTangentUploadBuffer = NULL;

	if (m_pd3dBiTangentUploadBuffer) m_pd3dBiTangentUploadBuffer->Release();
	m_pd3dBiTangentUploadBuffer = NULL;
}

void CStandardMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);
	//pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CStandardMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(m_pstrMeshName, sizeof(char), nStrLength, pInFile);
	m_pstrMeshName[nStrLength] = '\0';

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);

				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;
				m_pxmf2TextureCoords1 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords1, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

				m_d3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);

				m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

				m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
				m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

				m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

				m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
				m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, pInFile);
			if (m_nSubMeshes > 0)
			{
				m_pnSubSetIndices = new int[m_nSubMeshes];
				m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

				m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
					pstrToken[nStrLength] = '\0';
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile);
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT), m_pnSubSetIndices[i], pInFile);

							m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);

							m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

void CStandardMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedMesh::CSkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CStandardMesh(pd3dDevice, pd3dCommandList)
{
}

CSkinnedMesh::~CSkinnedMesh()
{
	if (m_pxmu4BoneIndices) delete[] m_pxmu4BoneIndices;
	if (m_pxmf4BoneWeights) delete[] m_pxmf4BoneWeights;

	if (m_pxmf4x4BindPoseBoneOffsets) delete[] m_pxmf4x4BindPoseBoneOffsets;
	if (m_ppstrSkinningBoneNames) delete[] m_ppstrSkinningBoneNames;

	if (m_pd3dBoneIndexBuffer) m_pd3dBoneIndexBuffer->Release();
	if (m_pd3dBoneWeightBuffer) m_pd3dBoneWeightBuffer->Release();

	ReleaseShaderVariables();
}

void CSkinnedMesh::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	m_pd3dcbBoneOffsets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbBoneOffsets->Map(0, NULL, (void**)&m_pcbxmf4x4BoneOffsets);

	m_pd3dcbBoneTransforms = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbBoneTransforms->Map(0, NULL, (void**)&m_pcbxmf4x4BoneTransforms);
}

void CSkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbBoneOffsets && m_pd3dcbBoneTransforms)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbBoneOffsets->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dcbBoneOffsetsGpuVirtualAddress); //Skinned Bone Offsets
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = m_pd3dcbBoneTransforms->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(8, d3dcbBoneTransformsGpuVirtualAddress); //Skinned Bone Transforms
		 
		for (int i = 0; i < m_nSkinningBones; i++)
		{
			XMStoreFloat4x4(&m_pcbxmf4x4BoneOffsets[i], XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4BindPoseBoneOffsets[i])));
			XMStoreFloat4x4(&m_pcbxmf4x4BoneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World)));
			//cout << " i : " << i << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._11 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._12 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._13 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._14 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._21 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._22 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._23 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._24 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._31 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._32 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._33 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._34 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._41 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._42 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._43 << " ";
			//cout << m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._44 << "\n";
		}
	}
}

void CSkinnedMesh::ReleaseShaderVariables()
{
	if (m_pd3dcbBoneOffsets) m_pd3dcbBoneOffsets->Release();
	if (m_pd3dcbBoneTransforms) m_pd3dcbBoneTransforms->Release();
}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	CStandardMesh::ReleaseUploadBuffers();

	if (m_pd3dBoneIndexUploadBuffer) m_pd3dBoneIndexUploadBuffer->Release();
	m_pd3dBoneIndexUploadBuffer = NULL;

	if (m_pd3dBoneWeightUploadBuffer) m_pd3dBoneWeightUploadBuffer->Release();
	m_pd3dBoneWeightUploadBuffer = NULL;
}

void CSkinnedMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);
	///pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CSkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(m_pstrSkinnedMeshName, sizeof(char), nStrLength, pInFile);
	m_pstrSkinnedMeshName[nStrLength] = '\0';

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			nReads = (UINT)::fread(&m_nBonesPerVertex, sizeof(int), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			nReads = (UINT)::fread(&m_nSkinningBones, sizeof(int), 1, pInFile);
			if (m_nSkinningBones > 0)
			{
				m_ppstrSkinningBoneNames = new char[m_nSkinningBones][64];
				m_ppSkinningBoneFrameCaches = new CGameObjectVer2 * [m_nSkinningBones];
				for (int i = 0; i < m_nSkinningBones; i++)
				{
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(m_ppstrSkinningBoneNames[i], sizeof(char), nStrLength, pInFile);
					m_ppstrSkinningBoneNames[i][nStrLength] = '\0';

					m_ppSkinningBoneFrameCaches[i] = NULL;
				}
			}
			//for (int i = 0; i < m_nSkinningBones; ++i) {
			//	cout << "BoneName: " << m_ppstrSkinningBoneNames[i] << endl;
			//}
		}
		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			nReads = (UINT)::fread(&m_nSkinningBones, sizeof(int), 1, pInFile);
			if (m_nSkinningBones > 0)
			{
				m_pxmf4x4BindPoseBoneOffsets = new XMFLOAT4X4[m_nSkinningBones];
				nReads = (UINT)::fread(m_pxmf4x4BindPoseBoneOffsets, sizeof(float), 16 * m_nSkinningBones, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
			if (m_nVertices > 0)
			{
				m_pxmu4BoneIndices = new XMUINT4[m_nVertices];
				m_pxmf4BoneWeights = new XMFLOAT4[m_nVertices];

				nReads = (UINT)::fread(m_pxmu4BoneIndices, sizeof(XMUINT4), m_nVertices, pInFile);
				m_pd3dBoneIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmu4BoneIndices, sizeof(XMUINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndexUploadBuffer);

				m_d3dBoneIndexBufferView.BufferLocation = m_pd3dBoneIndexBuffer->GetGPUVirtualAddress();
				m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
				m_d3dBoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * m_nVertices;

				nReads = (UINT)::fread(m_pxmf4BoneWeights, sizeof(XMFLOAT4), m_nVertices, pInFile);
				m_pd3dBoneWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf4BoneWeights, sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneWeightUploadBuffer);

				m_d3dBoneWeightBufferView.BufferLocation = m_pd3dBoneWeightBuffer->GetGPUVirtualAddress();
				m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_d3dBoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "</SkinningInfo>"))
		{
			break;
		}
	}
}

void CSkinnedMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[7] = { m_d3dPositionBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView, m_d3dBoneIndexBufferView, m_d3dBoneWeightBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 7, pVertexBufferViews);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CMinimapMesh::CMinimapMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float radius)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 72;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	 
	CTexturedVertex vertices[72]; 
	float angle = 15;
	
	for (int i = 0; i < 24; ++i)
	{
		vertices[i * 3].m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertices[i * 3 + 1].m_xmf3Position = XMFLOAT3(std::cos(GetRadian((i + 1) * angle)) * radius, std::sin(GetRadian(i + 1) * angle) * radius, 0.0f);
		vertices[i * 3 + 2].m_xmf3Position = XMFLOAT3(std::cos(GetRadian(i * angle)) * radius, std::sin(GetRadian(i * angle)) * radius, 0.0f);

		vertices[i * 3].m_xmf2TexCoord = XMFLOAT2(0.5f, 0.5f);
		vertices[i * 3 + 1].m_xmf2TexCoord = XMFLOAT2(std::cos(GetRadian((i + 1) * angle)) * 0.5 + 0.5f, 1 - (std::sin(GetRadian(i + 1) * angle) * 0.5f + 0.5f));
		vertices[i * 3 + 2].m_xmf2TexCoord = XMFLOAT2(std::cos(GetRadian(i * angle)) * 0.5f + 0.5f, 1 - (std::sin(GetRadian(i * angle)) * 0.5f + 0.5f));
	}
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		vertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CMinimapMesh::~CMinimapMesh()
{

}
#pragma region About Terrain Meshes
///////////////////////////////////////////////////////////////////////////////
// 
CTerrainMesh::CTerrainMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, int heights[25],
	XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1],
	int xNomalPos, int zNormalPos)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	int WidthBlock_Count = 9, DepthBlock_Count = 9;
	int WidthBlock_Index = 257, DepthBlock_Index = 257;
	int xStart = 0, zStart = 0;

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	m_nVertices = 25;
	m_nStride = sizeof(CTerrainVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

	CTerrainVertex* pVertices = new CTerrainVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, j = 0, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2, ++j)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x += 2, i++)
		{
			if (i >= 25) break;

			pVertices[i].m_xmf3Position = XMFLOAT3(x / 2, heights[i], z / 2);
			pVertices[i].m_xmf3Normal = normals[zNormalPos][xNomalPos];
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(x / 8, z / 9);
			//pVertices[i].m_xmf4Color = XMFLOAT4(1, 1, 1, 0);
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices; 
}

CTerrainMesh::CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	bool xZero, bool zZero,
	int* heights,
	XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1],
	int xNomalPos, int zNormalPos)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	int WidthBlock_Count = 9, DepthBlock_Count = 9;
	int WidthBlock_Index = 257, DepthBlock_Index = 257;
	int xStart = 0,zStart = 0;

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	m_nVertices = 25;
	m_nStride = sizeof(CTerrainVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

	CTerrainVertex* pVertices = new CTerrainVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, j = 0, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2, ++j) 
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x += 2, i++)
		{
			if (i >= 25) break; 
			 
			if(xZero){
				pVertices[i].m_xmf3Position = XMFLOAT3(xStart, heights[i], z / 2);
			}
			else if (zZero) {
				pVertices[i].m_xmf3Position = XMFLOAT3(x / 2, heights[i], zStart);
			}
			pVertices[i].m_xmf3Normal = normals[zNormalPos][xNomalPos];
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(x / 8, z / 9);
			//pVertices[i].m_xmf4Color = XMFLOAT4(1, 1, 1, 0); 
		}
	} 

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices; 
}
CTerrainMesh::CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	int xIndex, int zIndex, 
	int heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1],
	XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1])
	: CMesh(pd3dDevice, pd3dCommandList)
{
	int WidthBlock_Count = 9, DepthBlock_Count = 9;
	int WidthBlock_Index = 257, DepthBlock_Index = 257;
	int xStart = 0, zStart = 0;

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	m_nVertices = 25;
	m_nStride = sizeof(CTerrainVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

	CTerrainVertex* pVertices = new CTerrainVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, j = 4, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2, --j)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x += 2, i++)
		{
			if (i >= 25) break; 
			 
			pVertices[i].m_xmf3Position = XMFLOAT3(x / 2, heights[zIndex + j][xIndex + i % 5], z / 2);
			pVertices[i].m_xmf3Normal = normals[zIndex + j][xIndex + i % 5];
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(x / 8, z / 9); 
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices; 
}
CTerrainMesh::~CTerrainMesh()
{
}
 
CBindingTerrainMesh::CBindingTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	int verticesCount)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = verticesCount;
	m_Vertices = new CTerrainVertex[m_nVertices]; 
	m_CurrentVertexIndex = 0;

	m_nStride = sizeof(CTerrainVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
}

CBindingTerrainMesh::~CBindingTerrainMesh()
{
}

void CBindingTerrainMesh::CreateWallMesh(ID3D12Device* pd3dDevice,
 ID3D12GraphicsCommandList* pd3dCommandList, 
	const XMFLOAT3& shift, BYTE textureInfo, 
	int heights[25],
	XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1], int xNomalPos, int zNormalPos) 
{
	int WidthBlock_Count = 9, DepthBlock_Count = 9;
	int WidthBlock_Index = 257, DepthBlock_Index = 257;
	int xStart = 0, zStart = 0;

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;
	  
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, j = 0, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2, ++j)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x += 2, i++)
		{
			if (i >= 25) break;

			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(shift.x + x / 2, heights[i], shift.z + z / 2);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Normal = normals[zNormalPos][xNomalPos];
			m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(x / 8, z / 9); 
			m_Vertices[m_CurrentVertexIndex].m_TextureInfo = textureInfo;
			++m_CurrentVertexIndex;
		}
	} 
}

void CBindingTerrainMesh::CreateGridMesh(ID3D12Device* pd3dDevice,
 ID3D12GraphicsCommandList* pd3dCommandList,
	const XMFLOAT3& shift, BYTE textureInfo,
	int xIndex, int zIndex,
	int heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1], XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1])
{  
	int WidthBlock_Count = 9, DepthBlock_Count = 9;
	int WidthBlock_Index = 257, DepthBlock_Index = 257;
	int xStart = 0, zStart = 0;

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1; 
	 
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, j = 4, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2, --j)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x += 2, i++)
		{
			if (i >= 25) break;

			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(shift.x + x / 2, heights[zIndex + j][xIndex + i % 5], shift.z + z / 2);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Normal = normals[zIndex + j][xIndex + i % 5];
			m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(x / 8, z / 9);  
			m_Vertices[m_CurrentVertexIndex].m_TextureInfo = textureInfo;
			++m_CurrentVertexIndex;
		}
	} 
}

void CBindingTerrainMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] m_Vertices;
}
///////////////////////////////////////////////////////////////////////////////
//
CDoorMesh::CDoorMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth,
	bool isLeft)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CTexturedVertex pVertices[36];
	int i = 0;
	float xStart = 0.0f;
	float xEnd = 0.5f;
	if (!isLeft)
	{
		xStart = 0.51f;
		xEnd = 1.0f;
	}

	XMFLOAT3 normals[6] = {
		XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, -1.0f),
		 XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)
	};
	///////////////////////////////////////////////////////////////////////////////////////Front
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(xStart, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(xEnd, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f), normals[0]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(xStart, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(xStart, 0.7f), normals[0]);

	///////////////////////////////////////////////////////////////////////////////////////Top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.78f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.5f, 0.78f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f), normals[1]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.78f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.9f), normals[1]);
	///////////////////////////////////////////////////////////////////////////////////////Back 
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(xStart, 0.7f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(xEnd, 0.7f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f), normals[2]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(xStart, 0.7f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(xStart, 0.0f), normals[2]);
	///////////////////////////////////////////////////////////////////////////////////////Bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.78f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.5f, 0.78f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f), normals[3]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.78f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.9f), normals[3]);
	///////////////////////////////////////////////////////////////////////////////////////Left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.91f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.74f, 0.91f), normals[4]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.74f, 0.91f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.74f, 1.0f), normals[4]);
	///////////////////////////////////////////////////////////////////////////////////////Right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.91f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f), normals[5]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f), normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.74f, 1.0f), normals[5]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CDoorMesh::~CDoorMesh()
{
}

CDoorMeshTest::CDoorMeshTest(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth, bool isLeft)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CTexturedVertex pVertices[36];
	int i = 0;
	float xStart = 0.0f;
	float xEnd = 0.5f;
	if (!isLeft)
	{
		xStart = 0.51f;
		xEnd = 1.0f;
		//fx = -fWidth;
	}
	XMFLOAT3 normals[6] = {
		XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, -1.0f),
		 XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),XMFLOAT3(1.0f, 0.0f, 0.0f)
	};
	///////////////////////////////////////////////////////////////////////////////////////Front
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(xStart, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(xEnd, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f), normals[0]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(xStart, 0.0f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f), normals[0]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(xStart, 0.7f), normals[0]);

	///////////////////////////////////////////////////////////////////////////////////////Top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 0.78f)  , normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.5f, 0.78f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f) , normals[1]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 0.78f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f), normals[1]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(0.0f, 0.9f), normals[1]);
	///////////////////////////////////////////////////////////////////////////////////////Back 
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(xStart, 0.7f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(xEnd, 0.7f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f), normals[2]);
																				   
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(xStart, 0.7f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f), normals[2]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(xStart, 0.0f), normals[2]);
	///////////////////////////////////////////////////////////////////////////////////////Bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.0f, 0.78f),  normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.5f, 0.78f),normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f), normals[3]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.0f, 0.78f) , normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f), normals[3]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(0.0f, 0.9f), normals[3]);
	///////////////////////////////////////////////////////////////////////////////////////Left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 1.0f)  , normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(0.0f, 0.91f),  normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.74f, 0.91f), normals[4]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 1.0f)  , normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.74f, 0.91f), normals[4]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(0.74f, 1.0f),  normals[4]);
	///////////////////////////////////////////////////////////////////////////////////////Right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f)  ,normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.91f) ,normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f),normals[5]);

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f),  normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f),normals[5]);
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.74f, 1.0f), normals[5]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CDoorMeshTest::~CDoorMeshTest()
{
}

///////////////////////////////////////////////////////////////////////////////
//


CSphereMeshDiffused::CSphereMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	PulledModel pulledModelInfo,
	float radius, UINT32 sliceCount, UINT32 stackCount,
	const XMFLOAT3& shift)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = stackCount * sliceCount + 1;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float xCorrectionValue = 0.0f, yCorrectionValue = 0.0f;		
	if (pulledModelInfo == PulledModel::Left) {
		xCorrectionValue = -radius;
	}
	else if (pulledModelInfo == PulledModel::Right) {
		xCorrectionValue = radius;
	}
	else if (pulledModelInfo == PulledModel::Top) {
		yCorrectionValue = radius;
	}
	else if (pulledModelInfo == PulledModel::Bottom) {
		yCorrectionValue = -radius;
	}
	XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f); 

	CDiffusedVertex topVertex(XMFLOAT3(shift.x + xCorrectionValue, shift.y + yCorrectionValue +radius, shift.z), xmf4Color);
	CDiffusedVertex bottomVertex(XMFLOAT3(shift.x + xCorrectionValue, shift.y + yCorrectionValue -radius, shift.z), xmf4Color);

	const int a = m_nVertices;
	CDiffusedVertex* pVertices = new CDiffusedVertex[m_nVertices];
	pVertices[0] = topVertex;

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;
	// Compute vertices for each stack ring (do not count the poles as rings).
	int idx = 1;
	for (UINT32 i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT32 j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			// spherical to cartesian
			float x = shift.x + xCorrectionValue + radius * sinf(phi) * cosf(theta);
			float y = shift.y + yCorrectionValue + radius * cosf(phi);
			float z = shift.z + radius * sinf(phi) * sinf(theta);
			 
			pVertices[idx++] = CDiffusedVertex(
				XMFLOAT3(x, y, z),
				xmf4Color
			);
		}
	}
	pVertices[idx++] = bottomVertex;

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	vector<UINT> pnVecIndices;

	for (UINT32 i = 1; i <= sliceCount; ++i)
	{
		pnVecIndices.push_back(0);
		pnVecIndices.push_back(i + 1);
		pnVecIndices.push_back(i);
	}

	UINT32 baseIndex = 1;
	UINT32 ringVertexCount = sliceCount + 1;
	for (UINT32 i = 0; i < stackCount - 2; ++i)
	{
		for (UINT32 j = 0; j < sliceCount; ++j)
		{
			pnVecIndices.push_back(baseIndex + i * ringVertexCount + j);
			pnVecIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			pnVecIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			pnVecIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			pnVecIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			pnVecIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	UINT32 southPoleIndex = (UINT32)m_nVertices - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT32 i = 0; i < sliceCount; ++i)
	{
		pnVecIndices.push_back(southPoleIndex);
		pnVecIndices.push_back(baseIndex + i);
		pnVecIndices.push_back(baseIndex + i + 1);
	}
	m_nIndices = pnVecIndices.size();
	UINT* pnIndices = new UINT[m_nIndices];

	copy(pnVecIndices.begin(), pnVecIndices.end(), pnIndices);

	m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}


CSphereMesh::CSphereMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	float radius, UINT32 sliceCount, UINT32 stackCount)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = stackCount * sliceCount + 1;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex topVertex(0.0f, +radius, 0.0f, XMFLOAT2{ 0.0f, 0.0f });
	CTexturedVertex bottomVertex(0.0f, -radius, 0.0f, XMFLOAT2{ 0.0f, 1.0f });

	const int a = m_nVertices;
	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];
	pVertices[0] = topVertex;

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;
	// Compute vertices for each stack ring (do not count the poles as rings).
	int idx = 1;
	for (UINT32 i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT32 j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			// spherical to cartesian
			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);

			float uvX = theta / XM_2PI;
			float uvY = phi / XM_PI;
			pVertices[idx++] = CTexturedVertex(
				x, y, z,
				uvX, uvY
			);
		}
	}
	pVertices[idx++] = bottomVertex;

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	vector<UINT> pnVecIndices;

	for (UINT32 i = 1; i <= sliceCount; ++i)
	{
		pnVecIndices.push_back(0);
		pnVecIndices.push_back(i + 1);
		pnVecIndices.push_back(i);
	}

	UINT32 baseIndex = 1;
	UINT32 ringVertexCount = sliceCount + 1;
	for (UINT32 i = 0; i < stackCount - 2; ++i)
	{
		for (UINT32 j = 0; j < sliceCount; ++j)
		{
			pnVecIndices.push_back(baseIndex + i * ringVertexCount + j);
			pnVecIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			pnVecIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			pnVecIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			pnVecIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			pnVecIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	UINT32 southPoleIndex = (UINT32)m_nVertices - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT32 i = 0; i < sliceCount; ++i)
	{
		pnVecIndices.push_back(southPoleIndex);
		pnVecIndices.push_back(baseIndex + i);
		pnVecIndices.push_back(baseIndex + i + 1);
	}
	m_nIndices = pnVecIndices.size();
	UINT* pnIndices = new UINT[m_nIndices];

	copy(pnVecIndices.begin(), pnVecIndices.end(), pnIndices);

	m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

CSphereMesh::~CSphereMesh()
{
}

CMinimapAroowMesh::CMinimapAroowMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth) 
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float widthHalf = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;
	float uvXStart = 0.25f, uvXEnd = 1.0f;
	float uvYStart = 0.0f, uvYEnd = 1.0f;
	   
	pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf, heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
	pVertices[1] = CTexturedVertex(XMFLOAT3(widthHalf, heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYStart));
	pVertices[2] = CTexturedVertex(XMFLOAT3(widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYEnd));
	pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf, heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
	pVertices[4] = CTexturedVertex(XMFLOAT3(widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYEnd));
	pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXStart, uvYEnd));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CMinimapAroowMesh::~CMinimapAroowMesh()
{
}

CHpSpPercentMesh::CHpSpPercentMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth,
	bool isHp)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float widthHalf = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;
	float uvXStart = 0.0f, uvXEnd = 0.25f;
	float uvYStart = 0.0f, uvYEnd = 1.0f;
	if (isHp) uvYEnd = 0.5f;
	else
	{
		uvYStart = 0.5f;
		uvYEnd = 1.0f;
	}

	pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf, heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
	pVertices[1] = CTexturedVertex(XMFLOAT3(widthHalf, heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYStart));
	pVertices[2] = CTexturedVertex(XMFLOAT3(widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYEnd));
	pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf, heightHalf, fDepth), XMFLOAT2(uvXStart, uvYStart));
	pVertices[4] = CTexturedVertex(XMFLOAT3(widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXEnd, uvYEnd));
	pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, -heightHalf, fDepth), XMFLOAT2(uvXStart, uvYEnd));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CHpSpPercentMesh::~CHpSpPercentMesh()
{
}

CArrowParticleMesh::CArrowParticleMesh(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList,
	int particleCount) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = particleCount * 6;
	m_Vertices = new CParticleVertex[m_nVertices];
	m_CurrentVertexIndex = 0;
	 
	m_nStride = sizeof(CParticleVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;  

	CreateMeshes(pd3dDevice, pd3dCommandList, particleCount);
	CreateVertexBuffer(pd3dDevice, pd3dCommandList);
}

CArrowParticleMesh::~CArrowParticleMesh()
{
}

void CArrowParticleMesh::CreateMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count)
{  
	const float PARTICLE_SIZE = 0.2f;

	float goalSize = 30.0f;
	float perSize = goalSize / count;

	float goalSpeed = 30.0f;
	float perSpeed = goalSpeed / count;
	for (int i = 0; i < count; ++i) {
		XMFLOAT3 pos = GetRandomVector3(1000, 1, 50);
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
		XMFLOAT4 color = XMFLOAT4(GetRandomValue(1.0f, 0.7f, 0.7f),
			GetRandomValue(1.0f, 0.7f, 0.7f), 0.3f, 1.0f);
		XMFLOAT3 speed = GetRandomVector3(200.0f, -400.0f, -200.0f); 
		speed.z = perSpeed * (count - i);

		XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(ARROW_PARTICLE_LIFE_TIME, ARROW_PARTICLE_LIFE_TIME * 0.5f, ARROW_PARTICLE_LIFE_TIME * 0.5f)); 
		// 매개변수 방정식 값, 원 크기, 원 주기
		XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(5.0f, 0.0f, 0.0f), perSize * (count - i), GetRandomValue(2.0f, 0.0f, 0.0f)); 
		// v0 
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v1
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v2
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v3
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v4
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v5
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;
	}
}

void CArrowParticleMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] m_Vertices;
}


CTexParticleMesh::CTexParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	int particleCount) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = particleCount * 6;
	m_Vertices = new CParticleTextureVertex[m_nVertices];
	m_CurrentVertexIndex = 0; 

	m_nStride = sizeof(CParticleTextureVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CreateMeshes(pd3dDevice, pd3dCommandList, particleCount, PARTICLE_TYPE::HitParticleTex);
	CreateVertexBuffer(pd3dDevice, pd3dCommandList);
}

CTexParticleMesh::~CTexParticleMesh()
{
}
 
void CTexParticleMesh::CreateMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count, PARTICLE_TYPE type)
{
	const float PARTICLE_SIZE = 0.2f;

	float goalSize = 15;
	float perSize = goalSize / count;

	float goalSpeed = 165.0f * 3.0f;
	float perSpeed = goalSpeed / count;
	UINT texIndex = 0x01;
	for (int i = 0; i < count; ++i) {
		XMFLOAT3 pos = GetRandomVector3(1000, 1, 50);
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f; 
		XMFLOAT3 speed = GetRandomVector3(200.0f, -400.0f, -200.0f);
		speed.z = perSpeed * i;
		XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(ARROW_PARTICLE_LIFE_TIME, ARROW_PARTICLE_LIFE_TIME * 0.5f, ARROW_PARTICLE_LIFE_TIME * 0.5f));

		// 매개변수 방정식 값, 원 크기, 원 주기
		XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(10.0f, 0.0f, 0.0f), perSize * (count - i), GetRandomValue(2.0f, 0.0f, 0.0f));
		
		// v0 
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed; 
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v1
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed; 
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v2
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed; 
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v3
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed; 
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v4
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed; 
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v5
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed; 
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;
	}
}

void CTexParticleMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] m_Vertices;
}

CFogParticleMesh::CFogParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int particleCount)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = particleCount * 6;
	m_Vertices = new CParticleVertex[m_nVertices];
	m_CurrentVertexIndex = 0;

	m_nStride = sizeof(CParticleVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CreateMeshes(pd3dDevice, pd3dCommandList, particleCount);
	CreateVertexBuffer(pd3dDevice, pd3dCommandList);
}

CFogParticleMesh::~CFogParticleMesh()
{

}

void CFogParticleMesh::CreateMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count)
{
	const float PARTICLE_SIZE = 20.0f;

	for (int i = 0; i < count; ++i) {
		XMFLOAT3 pos = GetRandomVector3(1000, 1, 50);
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
		/*XMFLOAT4 color = XMFLOAT4(GetRandomValue(1.0f, 1.0f, 1.0f),
			GetRandomValue(1.0f, 1.0f, 1.0f), GetRandomValue(1.0f, 1.0f, 1.0f), 1.0f);*/
		XMFLOAT4 color = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			
		XMFLOAT3 speed = { float(rand() % 1000)-500.f,float(rand() % 1000)-500.f,float(rand() % 1000)-500.f };

		//XMFLOAT3 speed = GetRandomVector3(200.0f, -400.0f, -200.0f);
		
		XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(RADIAL_PARTICLE_LIFE_TIME, RADIAL_PARTICLE_LIFE_TIME * 0.5f, RADIAL_PARTICLE_LIFE_TIME * 0.5f));

		// 매개변수 방정식 값, 원 크기, 원 주기
		XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(5.0f, 0.0f, 0.0f), GetRandomValue(1.0f,1.0f,1.0f) , GetRandomValue(2.0f, 0.0f, 0.0f));
		// v0 
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v1
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v2
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v3
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v4
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;

		// v5
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		++m_CurrentVertexIndex;
	}
}

void CFogParticleMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] m_Vertices;
}



CRainParticleMesh::CRainParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int particleCount)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = particleCount * 6;
	m_Vertices = new CParticleTextureVertex[m_nVertices];
	m_CurrentVertexIndex = 0;

	m_nStride = sizeof(CParticleTextureVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CreateMeshes(pd3dDevice, pd3dCommandList, particleCount, PARTICLE_TYPE::RainParticle);
	CreateVertexBuffer(pd3dDevice, pd3dCommandList);
}

CRainParticleMesh::~CRainParticleMesh()
{
}

void CRainParticleMesh::CreateMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count,PARTICLE_TYPE type)
{
	const float PARTICLE_SIZE = 3.0f;

	UINT texIndex = 0x01;
	
	for (int i = 0; i < count; ++i) {
		XMFLOAT3 pos = GetRandomVector3(1000, 1, 50);
		pos.x = float(rand() % 5000)-3500.0f;
		pos.y = 2000.0f;
		pos.z = float(rand() % 2500)-800.0f;
		XMFLOAT4 color = XMFLOAT4(0.0f, 0.7f, 1.0f, 0.6f);

		XMFLOAT3 speed = { 10.f,-float(rand() % 700) -500.f,10.f };

		//XMFLOAT3 speed = GetRandomVector3(200.0f, -400.0f, -200.0f);

		XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(RADIAL_PARTICLE_LIFE_TIME, RADIAL_PARTICLE_LIFE_TIME * 0.5f, RADIAL_PARTICLE_LIFE_TIME * 0.5f));

		// 매개변수 방정식 값, 원 크기, 원 주기
		XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(5.0f, 0.0f, 0.0f), GetRandomValue(1.0f, 1.0f, 1.0f), GetRandomValue(2.0f, 0.0f, 0.0f));

		// v0 
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v1
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v2
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v3
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v4
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v5
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;
	}
}

void CRainParticleMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] m_Vertices;
}


CSandParticleMesh::CSandParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int particleCount)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = particleCount * 6;
	m_Vertices = new CParticleTextureVertex[m_nVertices];
	m_CurrentVertexIndex = 0;

	m_nStride = sizeof(CParticleTextureVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CreateMeshes(pd3dDevice, pd3dCommandList, particleCount, PARTICLE_TYPE::SandParticle);
	CreateVertexBuffer(pd3dDevice, pd3dCommandList);
}

CSandParticleMesh::~CSandParticleMesh()
{
}

void CSandParticleMesh::CreateMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count, PARTICLE_TYPE type)
{
	const float PARTICLE_SIZE = 3.0f;

	UINT texIndex = 0x01;

	for (int i = 0; i < count; ++i) {
		XMFLOAT3 pos = GetRandomVector3(1000, 1, 50);
		pos.x = float(rand() % 5000) - 3500.0f;
		pos.y = 2000.0f;
		pos.z = float(rand() % 2500) - 800.0f;
		XMFLOAT4 color = XMFLOAT4(0.85f, 0.7f, 0.56f, 1.0f);

		XMFLOAT3 speed = { 10.f,0.f,-float(rand() % 700) - 500.f };

		XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(RADIAL_PARTICLE_LIFE_TIME, RADIAL_PARTICLE_LIFE_TIME * 0.5f, RADIAL_PARTICLE_LIFE_TIME * 0.5f));

		// 매개변수 방정식 값, 원 크기, 원 주기
		XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(5.0f, 0.0f, 0.0f), GetRandomValue(1.0f, 1.0f, 1.0f), GetRandomValue(2.0f, 0.0f, 0.0f));

		// v0 
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v1
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v2
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v3
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v4
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(1.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;

		// v5
		m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
		m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
		m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
		m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
		m_Vertices[m_CurrentVertexIndex].m_xmf2TexCoord = XMFLOAT2(0.0f, 1.0f);
		m_Vertices[m_CurrentVertexIndex].m_nTexture = texIndex;
		++m_CurrentVertexIndex;
	}
}

void CSandParticleMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] m_Vertices;
}

CMeshFbxTextured::CMeshFbxTextured(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, 
	int nVertices, int nIndices, int* pnIndices) 
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = nVertices;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dIndexUploadBuffer);

	m_pd3dVertexBuffer->Map(0, NULL, (void**)&m_pxmf4MappedPositions);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = nIndices;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CMeshFbxTextured::~CMeshFbxTextured()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

}

}



CMummyLaserParticleMesh::CMummyLaserParticleMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	int particleCount,int idx) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = particleCount * 6;
	m_Vertices = new CParticleVertex[m_nVertices];
	m_CurrentVertexIndex = 0;

	m_nStride = sizeof(CParticleVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CreateMeshes(pd3dDevice, pd3dCommandList, particleCount,idx);
	CreateVertexBuffer(pd3dDevice, pd3dCommandList);
}

CMummyLaserParticleMesh::~CMummyLaserParticleMesh()
{
}

void CMummyLaserParticleMesh::CreateMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int count,int idx)
{
	if (idx == 1)
	{
		const float PARTICLE_SIZE = 20.0f;

		float goalSize = 10.0f;
		float perSize = goalSize / count;

		float goalSpeed = 30.0f;
		float perSpeed = goalSpeed / count;
		for (int i = 0; i < count; ++i) {
			XMFLOAT3 pos = GetRandomVector3(1000, 1, 0);
			pos.x = 0.0f;
			pos.y = 0.0f;
			pos.z = 0.0f;
			XMFLOAT4 color = XMFLOAT4(0.1, 0.9, 0, 1);

			/*XMFLOAT4 color = XMFLOAT4(GetRandomValue(1.0f, 0.7f, 0.7f),
				GetRandomValue(1.0f, 0.7f, 0.7f), 0.3f, 1.0f);*/
			XMFLOAT3 speed = GetRandomVector3(200.0f, -400.0f, -200.0f);
			speed.z = perSpeed * (count - i);

			XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(ARROW_PARTICLE_LIFE_TIME, ARROW_PARTICLE_LIFE_TIME * 0.5f, ARROW_PARTICLE_LIFE_TIME * 0.5f));
			// 매개변수 방정식 값, 원 크기, 원 주기
			//XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(1.0f, 0.0f, 0.0f), perSize * (count - i), GetRandomValue(10.0f, 0.0f, 0.0f));
			XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(0.2f, 0.0f, 0.0f), perSize * (count - i), 10.0f);
			// v0 
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v1
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v2
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v3
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v4
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v5
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;
		}
	}

	if (idx == 2)
	{
		const float PARTICLE_SIZE = 5.0f;

		float goalSize = 10.0f;
		float perSize = goalSize / count;

		float goalSpeed = 30.0f;
		float perSpeed = goalSpeed / count;
		for (int i = 0; i < count; ++i) {
			XMFLOAT3 pos = GetRandomVector3(1000, 1, 0);
			pos.x = 0.0f;
			pos.y = 0.0f;
			pos.z = 0.0f;
			XMFLOAT4 color = XMFLOAT4(0.1, 0.1, 0.9, 1);

			/*XMFLOAT4 color = XMFLOAT4(GetRandomValue(1.0f, 0.7f, 0.7f),
				GetRandomValue(1.0f, 0.7f, 0.7f), 0.3f, 1.0f);*/
			XMFLOAT3 speed = GetRandomVector3(200.0f, -400.0f, -200.0f);
			speed.z = perSpeed * (count - i);

			XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(ARROW_PARTICLE_LIFE_TIME, ARROW_PARTICLE_LIFE_TIME * 0.5f, ARROW_PARTICLE_LIFE_TIME * 0.5f));
			// 매개변수 방정식 값, 원 크기, 원 주기
			//XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(1.0f, 0.0f, 0.0f), perSize * (count - i), GetRandomValue(10.0f, 0.0f, 0.0f));
			XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(0.2f, 0.0f, 0.0f), perSize * (count - i), 10.0f);
			// v0 
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v1
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v2
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v3
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v4
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v5
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;
		}
	}

	if (idx == 3)
	{
		const float PARTICLE_SIZE = 50.0f;

		float goalSize = 10.0f;
		float perSize = goalSize / count;

		float goalSpeed = 30.0f;
		float perSpeed = goalSpeed / count;
		for (int i = 0; i < count; ++i) {
			XMFLOAT3 pos = GetRandomVector3(1000, 1, 0);
			pos.x = 0.0f;
			pos.y = 0.0f;
			pos.z = 0.0f;
			XMFLOAT4 color = XMFLOAT4(0.9, 0.1, 0, 1);

			/*XMFLOAT4 color = XMFLOAT4(GetRandomValue(1.0f, 0.7f, 0.7f),
				GetRandomValue(1.0f, 0.7f, 0.7f), 0.3f, 1.0f);*/
			XMFLOAT3 speed = GetRandomVector3(200.0f, -400.0f, -200.0f);
			speed.z = perSpeed * (count - i);

			XMFLOAT2 time = XMFLOAT2(0.0f, GetRandomValue(ARROW_PARTICLE_LIFE_TIME, ARROW_PARTICLE_LIFE_TIME * 0.5f, ARROW_PARTICLE_LIFE_TIME * 0.5f));
			// 매개변수 방정식 값, 원 크기, 원 주기
			//XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(1.0f, 0.0f, 0.0f), perSize * (count - i), GetRandomValue(10.0f, 0.0f, 0.0f));
			XMFLOAT3 randValues = XMFLOAT3(GetRandomValue(0.2f, 0.0f, 0.0f), perSize * (count - i), 10.0f);
			// v0 
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v1
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v2
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v3
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y + PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v4
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x + PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;

			// v5
			m_Vertices[m_CurrentVertexIndex].m_xmf3Position = XMFLOAT3(pos.x - PARTICLE_SIZE, pos.y - PARTICLE_SIZE, pos.z);
			m_Vertices[m_CurrentVertexIndex].m_xmf3Speed = speed;
			m_Vertices[m_CurrentVertexIndex].m_xmf4Diffuse = color;
			m_Vertices[m_CurrentVertexIndex].m_xmf2Time = time;
			m_Vertices[m_CurrentVertexIndex].m_xmf2RandomValue = randValues;
			++m_CurrentVertexIndex;
		}
	}
}

void CMummyLaserParticleMesh::CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Vertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] m_Vertices;
}

