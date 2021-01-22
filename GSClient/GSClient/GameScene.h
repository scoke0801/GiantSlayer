#pragma once

#include "GameTimer.h"
#include "Shader.h"
#include "Scene.h"
#include "Mesh.h"

class CShader;
class CGameObject;
class CCamera;

class CGameScene : public CScene
{
public:
	CGameScene();
	~CGameScene();
	
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void ReleaseObjects();

	void AnimateObjects(float fTimeElapsed);
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;

	virtual void ReleaseUploadBuffers() override;

	//그래픽 루트 시그너쳐를 생성한다.

private:
	void CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override;

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CGameObject		** m_ppObjects = NULL;
	int m_nObjects = 0;
	CShader			** m_ppShaders = NULL;
	
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

};

class CGameScene2 : public CScene
{
protected:
	CGameObject				** m_ppObjects = NULL;
	int						m_nObjects = 0;

	ID3D12RootSignature		* m_pd3dGraphicsRootSignature = NULL;

	CCamera					** m_Cameras;
	CCamera					* m_CurrentCamera = nullptr;

private:
	POINT					m_LastMousePos;

	ID3D12DescriptorHeap	* m_pd3dSrvDescriptorHeap = nullptr;

public:
	CGameScene2();
	~CGameScene2();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height); 

	void ReleaseObjects();

public:
	virtual void Update(double elapsedTime) override;
	void AnimateObjects(float fTimeElapsed);

	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;

public:
	virtual void ProcessInput();

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

public:
	virtual void ReleaseUploadBuffers() override;

	//그래픽 루트 시그너쳐를 생성한다.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override { return(m_pd3dGraphicsRootSignature); }

};