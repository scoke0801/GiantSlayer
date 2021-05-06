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
		ID3D12RootSignature* pd3dRootSignature,
		CShader* pShader);

	virtual ~CBridge(); 

public:
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

public:
	virtual void SetPosition(XMFLOAT3 pos) override;
	void RotateAll(const XMFLOAT3& axis, float angle);

	void UpdateColliders() override;
	bool CollisionCheck(Collider* pCollider) override;
private:
	int AddObject(CMesh* pMesh, CShader* pShader, UINT textureIndex);  
};

