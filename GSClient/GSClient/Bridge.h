#pragma once
#include "GameObject.h"
#include <DirectXCollision.h>
class CBridge : public CGameObject
{
private:
	vector<CMesh*> m_Meshes;
	vector<CBox*> m_Handles;

	vector<CGameObject*> m_Objects;

public:
	CBridge(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dRootSignature);

	virtual ~CBridge(); 

public:
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

public:
	virtual void SetPosition(XMFLOAT3 pos) override;

private:
	void AddObject(CMesh* pMesh, CShader* pShader, UINT textureIndex);
};

