#pragma once
#include "d3dx12.h"
#include "UploadBuffer.h"
#include "Mesh.h"
#include "Shader.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

#define GET_PSO(key) *m_PSOs[key].get() 

class CScene
{
protected:
	unordered_map<string, unique_ptr<CTexture>> m_Textures;
	unordered_map<string, unique_ptr<CMaterial>> m_Materials;

	unordered_map<string, unique_ptr<ID3D12PipelineState*>> m_PSOs;
	unordered_map<string, unique_ptr<CShader*>> m_pShaders;


	unordered_map<string, unique_ptr<ID3D12PipelineState*>> m_PSOs;
	
	unordered_map<string, unique_ptr<CShader*>> m_pShaders;
	 
public:
	CScene();
	virtual ~CScene();

	// just proto, fill with d3d obj
	virtual void Update(double elapsedTime) {} 
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) {}  
	 
public:
	// about Keyboard process
	virtual void ProcessInput() {}

	// about Mouse process
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

public: 
	virtual void SendDataToNextScene(void* context) {}	
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {} 
	virtual void ReleaseUploadBuffers() {}

	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) {}


	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) { return NULL; }
	virtual ID3D12RootSignature* GetGraphicsRootSignature() { return NULL; } 

protected:
	template<class SceneName>
	void ChangeScene(void* pContext = nullptr) { CFramework::GetInstance().ChangeScene<SceneName>(pContext); }
};  

class CNullScene : public CScene
{
public:
	CNullScene();
	~CNullScene();

	virtual void Update(double elapsedTime) override; 
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override; 

public:
	// about Keyboard process
	virtual void ProcessInput();

	// about Mouse process
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

public:
	virtual void SendDataToNextScene(void* context) override {}
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override {}
};
