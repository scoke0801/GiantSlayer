#pragma once
#include "GameObject.h"
class CBridge : public CGameObject
{
private:
	vector<CMesh*> m_Meshes;
	vector<CBox*> m_Handles;

public:
	CBridge(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float width, float height, float depth);
	virtual ~CBridge(); 

public:
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
};

