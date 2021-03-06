#include "stdafx.h"
#include "Mesh.h"
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

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
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

	CDiffusedVertex pVertices[36];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(1, 0, 0, 0));
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(1, 0, 0, 0));
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(0, 0, 1, 0));
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(0, 0, 1, 0));
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);

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

	///////////////////////////////////////////////////////////////////////////////////////Front
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
	///////////////////////////////////////////////////////////////////////////////////////Top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));
///////////////////////////////////////////////////////////////////////////////////////Back
	if (sameBackFace)
	{
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));

		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	}
	else
	{
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));

		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));
		pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f));
	}
	///////////////////////////////////////////////////////////////////////////////////////Bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
	///////////////////////////////////////////////////////////////////////////////////////Left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
	///////////////////////////////////////////////////////////////////////////////////////Right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

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

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z-fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z-fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z-fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z - fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + -fx, startPos.y + -fy, startPos.z +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z + fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + +fy, startPos.z - fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z + fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(startPos.x + +fx, startPos.y + -fy, startPos.z -fz), XMFLOAT2(0.0f, 1.0f));

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

CBillboardMesh::CBillboardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) 
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 1;
	m_nStride = sizeof(CBillboardVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	CBillboardVertex* pVertice = new CBillboardVertex;
	pVertice->m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pVertice->m_xmf2Size = XMFLOAT2(160.0f, 160.0f);
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

CPlaneMeshTextured::~CPlaneMeshTextured()
{
}
#pragma endregion
#pragma region About FBXMeshes
//////////////////////////////////////////////////////////////////////////////
//
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

	Meshinfo fbxmesh;
	fbxmesh.vertics = 0;
	Meshinfo* temp = &fbxmesh;

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

void CMeshFbx::LoadMesh(FbxNode* node, Meshinfo* info, bool rotateFlag)
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
						)
					)
				); 
			}
		} 
		info->vertics += nPolygons*3;
	}

	int nChilds = node->GetChildCount();
	cout << "연결된 차일드 노드 수: " << nChilds << endl;
	for (int i = 0; i < nChilds; i++) 
		LoadMesh(node->GetChild(i), info, rotateFlag);
}

CMeshFromFbx::CMeshFromFbx(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nVertices, int nIndices, int* pnIndices)
{
	m_nVertices = nVertices;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dPositionBuffer->Map(0, NULL, (void**)&m_pxmf4MappedPositions);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT4);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;

	m_nIndices = nIndices;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}


CMeshFromFbx::~CMeshFromFbx()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
}

void CMeshFromFbx::ReleaseUploadBuffers()
{
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}

void CMeshFromFbx::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dPositionBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
	pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
}
#pragma endregion

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
	ID3D12GraphicsCommandList* pd3dCommandList,
	int x_Index,int z_Index,
	int WidthBlock_Count,int DepthBlock_Count,
	int WidthBlock_Index,int DepthBlock_Index,
	MapMeshHeightType heightType)
	:CMesh(pd3dDevice, pd3dCommandList)
{
	int xStart = x_Index * (WidthBlock_Count - 1);
	int zStart = z_Index * (DepthBlock_Count - 1);

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	m_nVertices = 25;
	m_nStride = sizeof(CTerrainVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

	CTerrainVertex* pVertices = new CTerrainVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	
	for (int i = 0, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x+=2, i++)
		{
			if (i >= 25) break;
			// 정점의 높이와 색상을 높이 맵으로부터 구한다.
			float tempheight = 0;
			switch (heightType)
			{
			case MapMeshHeightType::Plane:
				tempheight = GetHeightPlane(x,z);
				break;
			case MapMeshHeightType::UpRidge:
				tempheight = GetHeightUpRidge(x, z, 500);
				break;
			case MapMeshHeightType::DownRidge:
				tempheight = GetHeightDownRidge(x, z, 500);
				break;
			default:
				assert(!"그런 지형 높이 함수는 없는데요");
				break;
			} 
			pVertices[i].m_xmf3Position = XMFLOAT3(x/2 , tempheight, z/2);
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(x / 8, z / 9);
			pVertices[i].m_xmf4Color = XMFLOAT4(1, 1, 1, 0);
			 
			if (tempheight < fMinHeight) tempheight = fMinHeight;
			if (tempheight > fMaxHeight)  tempheight = fMaxHeight; 
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

float CTerrainMesh::GetHeightUpRidge(float x, float z, float waveSize)
{
	return waveSize * cosf(XMConvertToRadians((x - 4) / 8) * 180);
}

float CTerrainMesh::GetHeightDownRidge(float x, float z, float waveSize)
{
	return waveSize * cosf(XMConvertToRadians((x - 4 + 8) / 8) * 180);
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
	///////////////////////////////////////////////////////////////////////////////////////Front
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(xStart, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(xEnd, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(xStart, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(xStart, 0.7f));

	///////////////////////////////////////////////////////////////////////////////////////Top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.5f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.9f));
	///////////////////////////////////////////////////////////////////////////////////////Back 
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(xStart, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(xEnd, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(xStart, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(xStart, 0.0f));
	///////////////////////////////////////////////////////////////////////////////////////Bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.5f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.9f));
	///////////////////////////////////////////////////////////////////////////////////////Left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.74f, 0.91f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.74f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.74f, 1.0f));
	///////////////////////////////////////////////////////////////////////////////////////Right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.74f, 1.0f));

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
	///////////////////////////////////////////////////////////////////////////////////////Front
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(xStart, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(xEnd, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(xStart, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(xStart, 0.7f));

	///////////////////////////////////////////////////////////////////////////////////////Top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.5f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(0.0f, 0.9f));
	///////////////////////////////////////////////////////////////////////////////////////Back 
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(xStart, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(xEnd, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(xStart, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(xStart, 0.0f));
	///////////////////////////////////////////////////////////////////////////////////////Bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.5f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(0.0f, 0.9f));
	///////////////////////////////////////////////////////////////////////////////////////Left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, -fz), XMFLOAT2(0.0f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.74f, 0.91f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, +fy, +fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, -fz), XMFLOAT2(0.74f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(0, -fy, +fz), XMFLOAT2(0.74f, 1.0f));
	///////////////////////////////////////////////////////////////////////////////////////Right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.74f, 1.0f));

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

CMeshIlluminated::CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Normalize(Vector3::Add(xmf3SumOfNormal, xmf3Normal));
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Normalize(Vector3::Add(xmf3SumOfNormal, xmf3Normal));
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}

CSphereMeshIlluminated::CSphereMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, UINT nSlices, UINT nStacks)
	: CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(CIlluminatedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	int k = 0;

	float fDeltaPhi = float(XM_PI / nStacks);
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);

	m_nVertices = 2 + (nSlices * (nStacks - 1));

	XMFLOAT3* pxmf3Positions = new XMFLOAT3[m_nVertices];

	pxmf3Positions[k++] = XMFLOAT3(0.0f, +fRadius, 0.0f);
	float theta_i, phi_j;
	for (UINT j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (UINT i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			pxmf3Positions[k++] = XMFLOAT3(fRadius * sinf(phi_j) * cosf(theta_i), fRadius * cosf(phi_j), fRadius * sinf(phi_j) * sinf(theta_i));
		}
	}
	pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius, 0.0f);

	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	UINT* pnIndices = new UINT[m_nIndices];

	k = 0;
	for (UINT i = 0; i < nSlices; i++)
	{
		pnIndices[k++] = 0;
		pnIndices[k++] = 1 + ((i + 1) % nSlices);
		pnIndices[k++] = 1 + i;
	}
	for (UINT j = 0; j < nStacks - 2; j++)
	{
		for (UINT i = 0; i < nSlices; i++)
		{
			pnIndices[k++] = 1 + (i + (j * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (UINT i = 0; i < nSlices; i++)
	{
		pnIndices[k++] = (m_nVertices - 1);
		pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	XMFLOAT3* pxmf3Normals = new XMFLOAT3[m_nVertices];
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);

	CIlluminatedVertex* pVertices = new CIlluminatedVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	if (pxmf3Positions) delete[] pxmf3Positions;
	if (pxmf3Normals) delete[] pxmf3Normals;
	if (pVertices) delete[] pVertices;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	if (pnIndices) delete[] pnIndices;

}

CSphereMeshIlluminated::~CSphereMeshIlluminated()
{
}
