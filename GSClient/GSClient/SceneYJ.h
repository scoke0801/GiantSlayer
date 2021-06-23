#pragma once
#include "Scene.h"

class CShader;
class CGameObject;
class CCamera;
class CPlayer;
class UI;
class HelpTextUI;
class CTerrain;
class CParticle;
class CLightCamera;
class CEnemy;
class CFbxObject2;

class CSceneYJ : public CScene
{
private:
	CFbxObject2* pfbxTestObject;

private:
	bool						m_isPlayerSelected = true;
	bool						m_isPlayerBoxCollide = false;
	bool						m_isBoxDown = false;
	bool						m_PuzzleNumSelect[9] = { false };
private:
	//array<CFixedMesh*, (int)FBX_MESH_TYPE::COUNT> m_LoadedFbxMesh;
	array<CMesh*, (int)FBX_MESH_TYPE::COUNT> m_LoadedFbxMesh;

	array<vector<CGameObject*>, (int)OBJECT_LAYER::Count> m_ObjectLayers;

	CParticle* m_Particles;

	// 플레이어가 새 지역으로 이동 시 이전 지역으로 이동을 막기 위한 벽을 생성
	// 씬 생성 시 저장한 후, 게임 중 상황에 따라 처리
	unordered_map<int, CGameObject*> m_BlockingPlateToPreviousSector;

	CGameObject* m_Mirror[1] = { nullptr };
	CPlayer* m_Player = nullptr;

	int							m_CurrentPlayerNum = 0;
	//vector<CPlayer*>			m_Players[MAX_PLAYER];
	CPlayer* m_Players[MAX_PLAYER];
	CBox* m_PuzzleBox[8];

	vector<UI*>					m_UIs;
	vector<UI*>					m_HPGauges;
	vector<UI*>					m_SPGauges;
	HelpTextUI* m_HelpTextUI;

	CSkyBox* m_Skybox;
	CTerrain* m_Terrain;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	vector<CCamera*>			m_PlayerCameras;
	CCamera** m_Cameras;
	CCamera* m_CurrentCamera = nullptr;
	CCamera* m_MinimapCamera = nullptr;
	CCamera* m_MirrorCamera = nullptr;

	CLightCamera* m_pLightCamera = nullptr;

	short						m_DoorIdx = 0;
	int							m_PuzzleNum[4];
	bool						m_PuzzleBoxCount = false;

private:
	POINT						m_LastMousePos;

	ID3D12DescriptorHeap* m_pd3dSrvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dDsvShadowMapCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvShadowMapGPUHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dDsvCPUDesciptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dDsvGPUDesciptorStartHandle;

private:	// about Meterail
	MATERIALS* m_pMaterials = NULL;

	ID3D12Resource* m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;

private:	// about Lights
	LIGHTS* m_pLights = NULL;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

private:	// about Minimap
	ID3D12Resource* m_pd3dMinimapTex = NULL;
	UI* m_MinimapArrow;

private:
	ID3D12Resource* m_pd3dMirrorTex = NULL;

private:
	ID3D12Resource* m_pd3dShadowMap = NULL;

private:	// about SceneInfo
	ID3D12Resource* m_pd3dcbSceneInfo = NULL;
	CB_GAMESCENE_FRAME_DATA* m_pcbMappedSceneFrameData = NULL;
	chrono::steady_clock::time_point m_CreatedTime;

private: // for server mouse input process
	vector<POINTF>				m_MousePositions;
	vector<MOUSE_INPUT_TYPE>    m_MouseInputTypes;
	MOUSE_INPUT_TYPE			m_prevMouseInputType;

private:
	CSoundManager* m_SoundManager;
public:
	CSceneYJ();
	~CSceneYJ();

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
	void Update(float elapsedTime) override;
	void UpdateForMultiplay(float elapsedTime)override;
	void AnimateObjects(float fTimeElapsed);

	void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;
	void DrawUI(ID3D12GraphicsCommandList* pd3dCommandList) override;
	void DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList) override;
	void FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList) override;
	void DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV) override;
	void DrawMirror(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV) override;
	void DrawShadow(ID3D12GraphicsCommandList* pd3dCommandList) override;

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
	void ShotPlayerArrow();
	void ShotMonsterArrow(CEnemy* pEmeny, const XMFLOAT3& lookVector);
	void DeleteEnemy(CEnemy* pEmeny);

private:
	void BuildBridges(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	void BuildDoorWall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);
	void BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildPuzzles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildSigns(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildEnemys(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildMirror(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildParticles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildArrows(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildMinimapResource(ID3D12Device* pd3dDevice);
	void BuildMirrorResource(ID3D12Device* pd3dDevice);

	void BuildPlayers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadFbxMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	// 그림자
	void BuildShadowResource(ID3D12Device* pd3dDevice);

	void CreateLightCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nHeight);

private:
	void BuildMapSector1(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildMapSector2(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildMapSector3(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildMapSector4(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildMapSector5(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildBoundingRegions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void EnterNewSector(int sectorNum);

	void MakingFog();
	void MakingRain();

private:
	void SendMouseInputPacket();
	void RecvMouseProcessPacket();
};
