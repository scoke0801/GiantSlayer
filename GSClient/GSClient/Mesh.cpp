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

	//�޽��� ���� ���� �並 �������Ѵ�(����������(�Է� ������)�� �۵��ϰ� �Ѵ�).
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
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

CCubeMeshTextured::CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth)
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

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

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

CPlaneMeshTextured::CPlaneMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float fWidth, float fHeight, float fDepth)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	 
	CTexturedVertex pVertices[6];
	 
	float widthHalf = fWidth * 0.5f;
	float heightHalf = fHeight * 0.5f;

	pVertices[0] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(0.0f, 0.0f));
	pVertices[1] = CTexturedVertex(XMFLOAT3( widthHalf,  heightHalf, fDepth), XMFLOAT2(1.0f, 0.0f));
	pVertices[2] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(1.0f, 1.0f));
	pVertices[3] = CTexturedVertex(XMFLOAT3(-widthHalf,  heightHalf, fDepth), XMFLOAT2(0.0f, 0.0f));
	pVertices[4] = CTexturedVertex(XMFLOAT3( widthHalf, -heightHalf, fDepth), XMFLOAT2(1.0f, 1.0f));
	pVertices[5] = CTexturedVertex(XMFLOAT3(-widthHalf, -heightHalf, fDepth), XMFLOAT2(0.0f, 1.0f));

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
	//FbxScene* m_pfbxScene = FbxScene::Create(pfbxSdkManager, "");
	//m_pfbxScene = LoadFbxScene(pd3dDevice, pd3dCommandList, pfbxSdkManager, pstrFbxFileName);
	//
	//FbxMesh* pfbxMesh = m_pfbxScene->GetRootNode()->GetChild(0)->GetMesh();
	////�� ���, ��Ʈ���->�޽����->��

	//FbxNode* pfbxNode = m_pfbxScene->GetRootNode();

	//FbxNodeAttribute* pfbxNodeAttribute = pfbxNode->GetNodeAttribute();

	//m_nVertices = pfbxMesh->GetControlPointsCount();
	//m_nPolygons = pfbxMesh->GetPolygonCount();
	//m_nStride = sizeof(CDiffusedVertex);
	//m_nOffset = 0;
	//m_nSlot = 0;
	//m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//cout << "-�޽� �ε�:" << pstrFbxFileName << " [����]: " << m_nVertices << "�� " << 
	//	"[������]: " << m_nPolygons << "��" << endl;

	////CDiffusedVertex* pVertices = new CDiffusedVertex[m_nVertices];
	//vector<CDiffusedVertex> Vertices;

	//for (int pindex = 0; pindex < m_nPolygons; pindex++) {
	//	//pVertices[i].m_xmf3Position.x = pfbxMesh->GetControlPointAt(pindex).mData[0];
	//	//pVertices[i].m_xmf3Position.y = pfbxMesh->GetControlPointAt(pindex).mData[1];
	//	//pVertices[i].m_xmf3Position.z = pfbxMesh->GetControlPointAt(pindex).mData[2];
	//	/*pVertices[pindex] = CDiffusedVertex(XMFLOAT3(pfbxMesh->GetControlPointAt(pindex).mData[0],
	//												 pfbxMesh->GetControlPointAt(pindex).mData[1],
	//												 pfbxMesh->GetControlPointAt(pindex).mData[2]),
	//												 XMFLOAT4(0.5, 0.5, 0.5, 0));*/
	//	for (int vindex = 0; vindex < 3; vindex++) {
	//		int pvindex = pfbxMesh->GetPolygonVertex(pindex, vindex);

	//		Vertices.push_back(CDiffusedVertex(XMFLOAT3(
	//			pfbxMesh->GetControlPointAt(pvindex).mData[0],
	//			pfbxMesh->GetControlPointAt(pvindex).mData[2],
	//			pfbxMesh->GetControlPointAt(pvindex).mData[1]),
	//			XMFLOAT4(0.5, 0.5, 0.5, 0)));
	//		//cout << pindex << " " << pfbxMesh->GetControlPointAt(pindex).mData[0] << " " << pfbxMesh->GetControlPointAt(pindex).mData[1] << " " << pfbxMesh->GetControlPointAt(pindex).mData[2] << endl;
	//	}
	//}

	//CDiffusedVertex* pVertices = new CDiffusedVertex[Vertices.size()];
	//copy(Vertices.begin(), Vertices.end(), pVertices);

	//m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
	//	m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
	//	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	////���� ���� �並 �����Ѵ�. 
	//m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	//m_d3dVertexBufferView.StrideInBytes = m_nStride;
	//m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CMeshFbx::~CMeshFbx()
{

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