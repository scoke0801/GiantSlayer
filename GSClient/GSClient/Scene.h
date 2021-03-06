#pragma once
#include "GameObject.h"
#include "d3dx12.h"
#include "UploadBuffer.h"
#include "Mesh.h" 

#define GET_PSO(key) *m_PSOs[key].get() 

class CScene
{
protected:
	unordered_map<string, unique_ptr<CTexture>> m_Textures;
	//unordered_map<string, unique_ptr<CMaterial>> m_Materials; 

	unordered_map<string, unique_ptr<ID3D12PipelineState*>> m_PSOs;
	
	unordered_map<string, unique_ptr<CShader*>> m_pShaders;
	 
protected:
	// about FBX
	FbxManager* m_pfbxManager = nullptr;
	FbxScene* m_pfbxScene = nullptr;
	FbxIOSettings* m_pfbxIOs = nullptr;
	FbxImporter* m_pfbxImporter = nullptr;

public:
	CScene();
	virtual ~CScene();

	// just proto, fill with d3d obj
	virtual void Update(double elapsedTime) {} 
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) {} 	
	virtual void DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void DrawUI(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV) {} 
	virtual void DrawMirror(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV) {}

public:
	// about Keyboard process
	virtual void ProcessInput() {}

	// about Mouse process
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

public:
	virtual void Communicate(SOCKET& sock) {}

public: 
	virtual void SendDataToNextScene(void* context) {}	
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) {}
	virtual void ReleaseUploadBuffers() {}
	
	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) {}
	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}

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
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) override {}
};
