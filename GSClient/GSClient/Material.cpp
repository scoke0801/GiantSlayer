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
