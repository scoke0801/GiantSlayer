#include "stdafx.h"
#include "Texture.h"

void MakeTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CTexture* pTexture, const string& toRegName, const wchar_t* fileName)
{
	pTexture->m_Name = toRegName;
	pTexture->m_FileName = fileName;
	pTexture->m_pd3dResource = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList,
		pTexture->m_FileName.c_str(),
		&pTexture->m_pd3dUploadHeap,
		D3D12_RESOURCE_STATE_GENERIC_READ);
}
