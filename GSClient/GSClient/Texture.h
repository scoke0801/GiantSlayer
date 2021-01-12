#pragma once
class CTexture
{
public:
	std::string m_Name;
	std::wstring m_FileName;

	ID3D12Resource* m_pd3dResource = nullptr;
	ID3D12Resource* m_pd3dUploadHeap = nullptr;
};

void MakeTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CTexture* pTexture,
	const string& toRegName, const wchar_t* fileName);

