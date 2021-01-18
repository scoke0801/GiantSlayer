#pragma once
#include "GameObject.h"
#include "d3dx12.h"
#include "UploadBuffer.h"
#include "Mesh.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


class CScene
{
protected:
	unordered_map<string, unique_ptr<CTexture>> m_Textures;
	unordered_map<string, unique_ptr<CMaterial>> m_Materials;

public:
	CScene();
	virtual ~CScene();

	// just proto, fill with d3d obj
	virtual void Update(double elapsedTime) {}
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {}

public:

	virtual void SendDataToNextScene(void* context) {}	
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void ReleaseUploadBuffers() {}

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) { return NULL; }
	virtual ID3D12RootSignature* GetGraphicsRootSignature() { return NULL; }

	virtual CHeightMapTerrain* GetTerrain() { return NULL; }

};


class CNullScene : public CScene
{
public:
	CNullScene();
	~CNullScene();

	virtual void Update(double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

public:

	CGameObject testbox1;
	CGameObject testbox2;

	virtual void SendDataToNextScene(void* context) override {}
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override {}
};
