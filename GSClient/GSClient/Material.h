#pragma once
class CMaterial
{
public:
	CMaterial();
	CMaterial(const string& Name);
	// Unique material name for lookup.
	std::string m_Name;

	// Index into constant buffer corresponding to this material.
	int m_MatCBIndex = -1;

	// Index into SRV heap for diffuse texture.
	int m_DiffuseSrvHeapIndex = -1;

	// Index into SRV heap for normal texture.
	int m_NormalSrvHeapIndex = -1;

	// Dirty flag indicating the material has changed and we need to update the constant buffer.
	// Because we have a material constant buffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify a material we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	// int NumFramesDirty = gNumFrameResources;

	// Material constant buffer data used for shading.
	XMFLOAT4	m_xmf4DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3	m_xmf3FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float		m_Roughness = 0.25f;
	XMFLOAT4X4	m_xmf4x4MatTransform;  
};

