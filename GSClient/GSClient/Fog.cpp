#include "stdafx.h"
#include "Fog.h"

CFog::CFog()
{
}

CFog::~CFog()
{
}

void CFog::Update(float elapsedTime)
{
	//UpdateLights(elapsedTime);
}

void CFog::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
}

//void CFog::UpdateLights(float elapsedTime)
//{
//	for (auto light : m_Lights)
//	{
//		if (light->m_nType == SPOT_LIGHT)
//		{
//			light->m_xmf3Position = GetPosition3f();
//			light->m_xmf3Direction = GetLook3f();
//		}
//	}
//}


void CFog::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_FOG_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbFog = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbFog->Map(0, NULL, (void**)&m_pcbFog);
}

void CFog::ReleaseShaderVariables()
{
	if (m_pd3dcbFog)
	{
		m_pd3dcbFog->Unmap(0, NULL);
		m_pd3dcbFog->Release();
	}
}

void CFog::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, int rootParameterIndex)
{
	::memcpy(&m_pcbFog->m_FogColor, &m_FogColor, sizeof(XMFLOAT4X4));
	::memcpy(&m_pcbFog->m_FogStart, &m_xmf3StartPosition, sizeof(XMFLOAT4X4));
	::memcpy(&m_pcbFog->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbFog->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, d3dGpuVirtualAddress);
}

