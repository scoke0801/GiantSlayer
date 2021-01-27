#include "stdafx.h"
#include "Material.h"

CMaterial::CMaterial()
{
	m_xmf4DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_xmf3FresnelR0 = { 0.01f, 0.01f, 0.01f };
	m_Roughness = 0.25f;
	m_xmf4x4MatTransform = Matrix4x4::Identity();
}

CMaterial::CMaterial(const string& Name)
{
	CMaterial();
	m_Name = Name;
}

void CMaterial::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(MaterialInfo) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbMaterial = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterial->Map(0, NULL, (void**)&m_pcbMappedMaterial);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pd3dcbMaterial)
	{
		m_pd3dcbMaterial->Unmap(0, NULL);
		m_pd3dcbMaterial->Release();
	}
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex)
{ 
	::memcpy(&m_pcbMappedMaterial->DiffuseAlbedo, &m_xmf4DiffuseAlbedo, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedMaterial->FresnelR0, &m_xmf3FresnelR0, sizeof(XMFLOAT3));
	::memcpy(&m_pcbMappedMaterial->Roughness, &m_Roughness, sizeof(float));
	
	XMMATRIX matTransform = XMLoadFloat4x4(&m_xmf4x4MatTransform);
	XMStoreFloat4x4(&m_pcbMappedMaterial->MatTransform, XMMatrixTranspose(matTransform));
	::memcpy(&m_pcbMappedMaterial->MatTransform, &matTransform, sizeof(XMFLOAT4X4));
	 
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbMaterial->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, d3dGpuVirtualAddress);
}
