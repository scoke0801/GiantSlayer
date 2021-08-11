#pragma once
#include "Scene.h"

class CShader;
class CGameObject;
class CCamera;
class CTerrain;
class CPlayer;
class UI;
class HelpTextUI;
class CTerrain;
class CParticle;
class CLightCamera;
class CEnemy;
class CMummy;
class CMummyLaser;
class CFbxObject2;
class CBoss;
class CEffectHandler;
class Font;

class CTitleScene : public CScene
{  
private: 
	array<CMesh*, (int)FBX_MESH_TYPE::COUNT> m_LoadedFbxMesh;

	array<vector<CGameObject*>, (int)OBJECT_LAYER::Count> m_ObjectLayers;
	 
	vector<UI*>					m_UIs; 
	HelpTextUI*					m_HelpTextUI; 

	ID3D12RootSignature*		m_pd3dGraphicsRootSignature = NULL;
	 
	CCamera**					m_Cameras;
	CCamera*					m_CurrentCamera = nullptr;  

private:
	POINT						m_LastMousePos;

	ID3D12DescriptorHeap* m_pd3dSrvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = nullptr;
	 
	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dDsvCPUDesciptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dDsvGPUDesciptorStartHandle;

private:	// about Meterail
	MATERIALS*					m_pMaterials = NULL;

	ID3D12Resource*				m_pd3dcbMaterials = NULL;
	MATERIAL*					m_pcbMappedMaterials = NULL;

private:	// about Lights
	LIGHTS*						m_pLights = NULL;

	ID3D12Resource*				m_pd3dcbLights = NULL;
	LIGHTS*						m_pcbMappedLights = NULL;
	 

	ID3D12Resource*				m_pd3dFontTexture = NULL;

private:	// about SceneInfo
	ID3D12Resource*				m_pd3dcbSceneInfo = NULL;
	CB_GAMESCENE_FRAME_DATA*	m_pcbMappedSceneFrameData = NULL;
	chrono::steady_clock::time_point m_CreatedTime;

private: // for server mouse input process
	vector<POINTF>				m_MousePositions;
	vector<MOUSE_INPUT_TYPE>    m_MouseInputTypes;
	MOUSE_INPUT_TYPE			m_prevMouseInputType;

private:
	CSoundManager*				m_SoundManager;

private:
	bool						m_IsOnRoomSelect = false;
	int							m_nWeaponSelect = 1;

	UI*							m_Select;
	UI*							m_PrevBtn, *m_NextBtn;
	UI* m_RoomBoard;

	vector<UI*>					m_Weapons;
	vector<UI*>					m_RoomInfos;
	vector<UI*>					m_ToSelectWeapon;
public:
	CTitleScene();
	~CTitleScene();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) override;

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	 
	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;

	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height); 

	void ReleaseObjects();
public:
	void Update(float elapsedTime) override;
	void UpdateForMultiplay(float elapsedTime)override;
	void AnimateObjects(float fTimeElapsed);
	void ProcessWindowKeyboard(WPARAM wParam, bool isKeyUp);

	void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;
	void DrawUI(ID3D12GraphicsCommandList* pd3dCommandList) override; 
	void DrawFont(ID3D12GraphicsCommandList* pd3dCommandList) override;

public:
	void Communicate(SOCKET& sock) override;

	void ProcessPacket(unsigned char* p_buf) override;

	void LoginToServer()  override;
	void LogoutToServer() override;

	void DeletePlayer(int playerId) override {}
	void AddPlayer(int palyerId) override {}
public:
	void ProcessInput() override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y)	override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
public:
	virtual void ReleaseUploadBuffers() override;

	//그래픽 루트 시그너쳐를 생성한다.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override { return(m_pd3dGraphicsRootSignature); }

public:
	void BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	 
private:
	void SendMouseInputPacket();
	void RecvMouseProcessPacket();
};
