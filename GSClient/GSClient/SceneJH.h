#pragma once
#include "Scene.h"
   
class CShader;
class CGameObject;
class CCamera;
class CPlayer;
class UI;
class HelpTextUI;
class CTerrain;

class CSceneJH : public CScene
{
private:
	bool						m_isPlayerSelected = true;

protected:
	vector<CGameObject*>		m_Objects; 
	vector<CGameObject*>		m_BillboardObjects;

	CPlayer*					m_Player = nullptr;

	vector<UI*>					m_UIs;
	vector<UI*>					m_HPGauges;
	vector<UI*>					m_SPGauges;
	HelpTextUI*					m_HelpTextUI;

	CSkyBox*					m_Skybox;
	CTerrain*					m_Terrain;

	ID3D12RootSignature*		m_pd3dGraphicsRootSignature = NULL;

	CCamera**					m_Cameras;
	CCamera*					m_CurrentCamera = nullptr;
	CCamera*					m_MinimapCamera = nullptr;
private:
	POINT						m_LastMousePos;

	ID3D12DescriptorHeap*		m_pd3dBasicSrvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap*		m_pd3dMonsterDescriptorHeap = nullptr;

private:	// about Meterail
	MATERIALS*					m_pMaterials = NULL;

	ID3D12Resource*				m_pd3dcbMaterials = NULL;
	MATERIAL*					m_pcbMappedMaterials = NULL;

private:	// about Lights
	LIGHTS*						m_pLights = NULL;

	ID3D12Resource*				m_pd3dcbLights = NULL;
	LIGHTS*						m_pcbMappedLights = NULL;

private:	// about Minimap
	ID3D12Resource*				m_pd3dMinimapTex = NULL;
	UI*							m_MinimapArrow;

private:	// about SceneInfo
	ID3D12Resource*				m_pd3dcbSceneInfo = NULL;
	CB_GAMESCENE_FRAME_DATA*	m_pcbMappedSceneFrameData = NULL;

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
	virtual void BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;

	void ReleaseObjects();

public:
	virtual void Update(double elapsedTime) override;
	void AnimateObjects(float fTimeElapsed);

	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;	
	virtual void DrawUI(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV) override;

public:
	virtual void Communicate(SOCKET& sock) override;

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
	void BuildBridges(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pShader = nullptr);
	void BuildDoorWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader = nullptr);

	void BuildEnemys(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildPuzzles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildBilboardObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	
	void BuildMinimapResource(ID3D12Device* pd3dDevice);
};
