#pragma once
#include "GameObject.h"

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
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) {}

public:
	virtual void SendDataToNextScene(void* context) {}	
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
};



class CNullScene : public CScene
{
public :
	CNullScene();
	~CNullScene();

	virtual void Update(double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;

	//virtual void Update(double elapsedTime) override;
	//virtual void Draw() override;

public:
	CGameObject testbox1;
	CGameObject testbox2;

	virtual void SendDataToNextScene(void* context) override {}
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override {}
};