#pragma once
#include "Scene.h"
  

class CShader;
class CGameObject;
class CCamera;
class CPlayer;

class CSceneJH : public CScene
{
protected:
	CGameObject**				m_ppObjects = NULL;
	int							m_nObjects = 0;

	CPlayer*					m_Player = nullptr;

	CSkyBox*					m_Skybox;

	ID3D12RootSignature*		m_pd3dGraphicsRootSignature = NULL;

	CCamera**					m_Cameras;
	CCamera*					m_CurrentCamera = nullptr;
private:
	POINT						m_LastMousePos;

	ID3D12DescriptorHeap*		m_pd3dSrvDescriptorHeap = nullptr;

private:	// about Meterail
	MATERIALS*					m_pMaterials = NULL;

	ID3D12Resource*				m_pd3dcbMaterials = NULL;
	MATERIAL*					m_pcbMappedMaterials = NULL;

private:	// about Lights
	LIGHTS*						m_pLights = NULL;

	ID3D12Resource*				m_pd3dcbLights = NULL;
	LIGHTS*						m_pcbMappedLights = NULL;

public:
	CSceneJH();
	~CSceneJH();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) override;

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;

	void ReleaseObjects();

public:
	virtual void Update(double elapsedTime) override;
	void AnimateObjects(float fTimeElapsed);

	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;	
	virtual void DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList) override;

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

private:
	// input startIndex, output endIndex 
	int BuildBridges(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		int startIndex, CShader* pShader);
	int BuildDoorWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int startIndex, CShader* pShader);
};
