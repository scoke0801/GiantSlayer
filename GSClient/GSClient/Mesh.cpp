#include "stdafx.h"
#include "Mesh.h"

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
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//�޽��� ������Ƽ�� ������ �����Ѵ�.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//�޽��� ���� ���� �並 �����Ѵ�.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�. 
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
	//������ü�� 6���� �� ����(x-��) ����
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//fWidth: ������ü ����(x-��) ����, fHeight: ������ü ����(y-��) ����, fDepth: ������ü ����(z-��) ����
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CDiffusedVertex pVertices[36];
	int i = 0;

	//���� ���� �����ʹ� �ﰢ�� ����Ʈ�̹Ƿ� 36���� ���� �����͸� �غ��Ѵ�.
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(1, 0, 0, 0));
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(1, 0, 0, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(1, 0, 0, 0));
	//�� ����(Top) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(0, 0, 1, 0));
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(0, 0, 1, 0));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(0, 0, 1, 0));
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//�� ����(Left) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//�� ����(Right) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//���� ���� �並 �����Ѵ�. 
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

CBillboardMesh::CBillboardMesh(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList, 
	CBillboardVertex* pGeometryBillboardVertices,
	UINT nGeometryBillboardVertices)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = nGeometryBillboardVertices;
	m_nStride = sizeof(CBillboardVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pGeometryBillboardVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

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
	bool isVertical)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];
	 
	float widthHalf = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;

	if (isVertical)
	{
		pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(0.0f, 0.0f));
		pVertices[1] = CTexturedVertex(XMFLOAT3( widthHalf,  heightHalf, fDepth), XMFLOAT2(1.0f, 0.0f));
		pVertices[2] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(1.0f, 1.0f));
		pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(0.0f, 0.0f));
		pVertices[4] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(1.0f, 1.0f));
		pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, -heightHalf, fDepth), XMFLOAT2(0.0f, 1.0f));
	}
	else	// horizon
	{
		pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf, 0.0, heightHalf), XMFLOAT2(0.0f, 0.0f));
		pVertices[1] = CTexturedVertex(XMFLOAT3(widthHalf, 0.0, heightHalf), XMFLOAT2(1.0f, 0.0f));
		pVertices[2] = CTexturedVertex(XMFLOAT3(widthHalf, 0.0, -heightHalf), XMFLOAT2(1.0f, 1.0f));
		pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf, 0.0, heightHalf), XMFLOAT2(0.0f, 0.0f));
		pVertices[4] = CTexturedVertex(XMFLOAT3(widthHalf, 0.0, -heightHalf), XMFLOAT2(1.0f, 1.0f));
		pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, 0.0, -heightHalf), XMFLOAT2(0.0f, 1.0f));
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

//////////////////////////////////////////////////////////////////////////////
//

CMeshFbx::CMeshFbx(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, char* pstrFbxFileName) : CMesh(pd3dDevice, pd3dCommandList)
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

	cout << "-�޽� �ε�:" << pstrFbxFileName << endl;

	LoadMesh(m_pfbxScene->GetRootNode(), temp);

	m_nVertices = fbxmesh.vertics;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	cout << "-�޽� �ε� �� ||| [����]: " << m_nVertices << "�� " << endl;

	CTexturedVertex* pVertices = new CTexturedVertex[fbxmesh.vertex.size()];
	copy(fbxmesh.vertex.begin(), fbxmesh.vertex.end(), pVertices);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//���� ���� �並 �����Ѵ�. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CMeshFbx::~CMeshFbx()
{

}

void CMeshFbx::LoadMesh(FbxNode* node, Meshinfo* info)
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

		cout << "[�޽��߰�] ���� ��� ���� ��: " << v << "  ���� ���� ��: " << vv << "  ��: " << vvv << endl;

		for (int pindex = 0; pindex < nPolygons; pindex++) {
			for (int vindex = 0; vindex < 3; vindex++) {
				int pvindex = pfbxMesh->GetPolygonVertex(pindex, vindex);
				int uvindex = pfbxMesh->GetTextureUVIndex(pindex, vindex, FbxLayerElement::eTextureDiffuse);

				FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
				FbxLayerElementUV* fbxLayerUV = pfbxMesh->GetLayer(0)->GetUVs();

				fbxUV = fbxLayerUV->GetDirectArray().GetAt(uvindex);

				float uv1 = fbxUV[0];
				float uv2 = 1.0f - fbxUV[1];

				
				info->vertex.push_back(
					CTexturedVertex(
						XMFLOAT3(
							pfbxMesh->GetControlPointAt(pvindex).mData[0],
							pfbxMesh->GetControlPointAt(pvindex).mData[2],
							pfbxMesh->GetControlPointAt(pvindex).mData[1]
						),
						XMFLOAT2(
							uv1,
							uv2
						)
					)
				);
				
				

				/*
				info->vertex.push_back(
					CDiffusedVertex(
						XMFLOAT3(
							pfbxMesh->GetControlPointAt(pvindex).mData[0],
							pfbxMesh->GetControlPointAt(pvindex).mData[2],
							pfbxMesh->GetControlPointAt(pvindex).mData[1]
						),
						XMFLOAT4(
							0.3,
							0.3,
							0.3,
							1.0f
						)
					)
				);
				*/
				

			}
		}

		info->vertics += nPolygons*3;
	}

	int nChilds = node->GetChildCount();
	cout << "����� ���ϵ� ��� ��: " << nChilds << endl;
	for (int i = 0; i < nChilds; i++) 
		LoadMesh(node->GetChild(i), info);
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

CTerrainMesh::CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int x_Index,int z_Index,int WidthBlock_Count,int DepthBlock_Count,int WidthBlock_Index,int DepthBlock_Index)

	:CMesh(pd3dDevice, pd3dCommandList)
{
	int xStart = x_Index * (WidthBlock_Count - 1);
	int zStart = z_Index * (DepthBlock_Count - 1);

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	m_nVertices = 25;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	
	for (int i = 0, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x+=2, i++)
		{
			if (i >= 25) break;
			// ������ ���̿� ������ ���� �����κ��� ���Ѵ�.
			float tempheight = OnGetHeight(x, z);

			pVertices[i].m_xmf3Position = XMFLOAT3(x/2 , tempheight , z/2);
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(1, 1);
			pVertices[i].m_xmf3Normal = XMFLOAT3(1, 0, z/9);
		
			
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

float CTerrainMesh::OnGetHeight(float x, float z)
{
	return 50.f * (z * sinf(0.7f * x) + x * cosf(0.7f * z));	
}

CTerrainWayMesh::CTerrainWayMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int WidthBlock_Count, int DepthBlock_Count)
	:CMesh(pd3dDevice, pd3dCommandList)
{
	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	//������ ����(����)�� ������ (nWidth * nLength)�̴�. 
	m_nVertices = m_nWidth * m_nDepth;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, z = zStart; z < (zStart + m_nDepth); z++)
	{
		for (int x = xStart; x < (xStart + m_nWidth); x++, i++)
		{
			// ������ ���̿� ������ ���� �����κ��� ���Ѵ�.
			float tempheight = OnGetHeight(x, z);
			pVertices[i].m_xmf3Position = XMFLOAT3(x , 20, z );
			pVertices[i].m_xmf3Normal = XMFLOAT3(x * 10, 10, z * 10);
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(float(x) / float(100.0f), float(z) / float(100.0f));


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


	m_nIndices = ((m_nWidth * 2) * (m_nDepth - 1)) + ((m_nDepth - 1) - 1);
	UINT* pnIndices = new UINT[m_nIndices];

	for (int j = 0, z = 0; z < m_nDepth - 1; z++)
	{
		if ((z % 2) == 0)
		{
			// Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) �ε����� ���� ������ ���ʿ��� ������ ����
			for (int x = 0; x < m_nWidth; x++)
			{
				// ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° �ε����� �߰�
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * m_nWidth));
				// �Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰�
				pnIndices[j++] = (UINT)(x + (z * m_nWidth));
				pnIndices[j++] = (UINT)((x + (z * m_nWidth)) + m_nWidth);
			}
		}
		else
		{
			// ¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) �ε����� ���� ������ �����ʿ��� ���� ����
			for (int x = m_nWidth - 1; x >= 0; x--)
			{
				// ���� �ٲ� ������(x == (nwidth-1)) ù ��° �ε����� �߰�
				if (x == (m_nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * m_nWidth));
				// �Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰�
				pnIndices[j++] = (UINT)(x + (z * m_nWidth));
				pnIndices[j++] = (UINT)((x + (z * m_nWidth)) + m_nWidth);
			}
		}
	}
	m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList,
		pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

CTerrainWayMesh::~CTerrainWayMesh()
{
}

float CTerrainWayMesh::OnGetHeight(float x, float z)
{
	return x+z;
}

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
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(xEnd,   0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd,   0.7f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(xStart, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(xEnd,   0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(xStart, 0.7f));

///////////////////////////////////////////////////////////////////////////////////////Top
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.5f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f ));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.5f, 0.9f ));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.9f ));
///////////////////////////////////////////////////////////////////////////////////////Back 
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(xStart, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(xEnd,   0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd,   0.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(xStart, 0.7f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(xEnd,   0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(xStart, 0.0f));
///////////////////////////////////////////////////////////////////////////////////////Bottom
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.5f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f ));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.78f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.5f, 0.9f ));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.9f ));
///////////////////////////////////////////////////////////////////////////////////////Left
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f,   1.0f ));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f,   0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.74f,  0.91f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f,   1.0f ));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.74f,  0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.74f,  1.0f )); 
///////////////////////////////////////////////////////////////////////////////////////Right
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f,  1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(0.0f,  0.91f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(0.74f, 0.91f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f,  1.0f));
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

CTerrainSinMesh::CTerrainSinMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int x_Index, int z_Index, int WidthBlock_Count, int DepthBlock_Count, int WidthBlock_Index, int DepthBlock_Index)
	:CMesh(pd3dDevice, pd3dCommandList)
{
	int xStart = x_Index * (WidthBlock_Count - 1);
	int zStart = z_Index * (DepthBlock_Count - 1);

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	m_nVertices = 25;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x += 2, i++)
		{
			if (i >= 25) break;
			// ������ ���̿� ������ ���� �����κ��� ���Ѵ�.
			float tempheight = OnGetSinHeight(x, z);

			pVertices[i].m_xmf3Position = XMFLOAT3(x / 2, tempheight, z / 2);
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(1, 1);
			pVertices[i].m_xmf3Normal = XMFLOAT3(1, 0, z / 9);


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

CTerrainSinMesh::~CTerrainSinMesh()
{
}

float CTerrainSinMesh::OnGetSinHeight(float x, float z)
{
	return 100.0f * sinf(60 * x) + 100.0f * cosf(60 * x);
}

CTerrainCosMesh::CTerrainCosMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int x_Index, int z_Index, int WidthBlock_Count, int DepthBlock_Count, int WidthBlock_Index, int DepthBlock_Index)
	:CMesh(pd3dDevice, pd3dCommandList)
{
	int xStart = x_Index * (WidthBlock_Count - 1);
	int zStart = z_Index * (DepthBlock_Count - 1);

	m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_nWidth = WidthBlock_Count + 1;
	m_nDepth = DepthBlock_Count + 1;

	m_nVertices = 25;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;

	CTexturedVertex* pVertices = new CTexturedVertex[m_nVertices];

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, z = (zStart + m_nDepth - 1); z >= zStart; z -= 2)
	{
		for (int x = xStart; x < (xStart + m_nWidth - 1); x += 2, i++)
		{
			if (i >= 25) break;
			// ������ ���̿� ������ ���� �����κ��� ���Ѵ�.
			float tempheight = OnGetCosHeight(x, z);

			pVertices[i].m_xmf3Position = XMFLOAT3(x / 2, 100.0f * sinf(i * 100 + i * 100), z / 2);
			pVertices[i].m_xmf2TexCoord = XMFLOAT2(1, 1);
			pVertices[i].m_xmf3Normal = XMFLOAT3(1, 0, z / 9);


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

CTerrainCosMesh::~CTerrainCosMesh()
{
}

float CTerrainCosMesh::OnGetCosHeight(float x, float z)
{
	return 0.0f;
}
