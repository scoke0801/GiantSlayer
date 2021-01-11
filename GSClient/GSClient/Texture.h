#pragma once
class CTexture
{
public:
	std::string m_Name;
	std::wstring m_FileName;

	ID3D12Resource* m_pd3dResource = nullptr;
	ID3D12Resource* m_pd3dUploadHeap = nullptr;
};

