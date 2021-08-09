#pragma once
#include "GameObject.h"
#include "d3dx12.h"
#include "UploadBuffer.h"
#include "Mesh.h" 

#include "Communicates.h"

#define GET_PSO(key) *m_PSOs[key].get() 

enum class FBX_MESH_TYPE : UINT {
	DryForestRock,
	DesertRock,
	Arrow,
	FireBall,
	Mummylaser,
	Human,
	DeadTree_01,
	DeadTree_02,
	DryTree_01,
	DryTree_02,
	Stump,
	Bush_1,
	Boss,
	Player,
	Enemy_01,
	Enemy_02,
	Mummy,
	GreenTree,
	Pawn,
	Knight,
	Rook,
	King,
	Laser,
	COUNT
};

// 편의를 위해 Layer에서 몇 가지 객체 유형은 제외하고 별도로 관리
enum class OBJECT_LAYER : int {
	Skybox,
	TerrainWater,
	Puzzle,
	PuzzleBox,
	ChessPuzzle,
	PlayerChessPuzzle,
	Obstacle,
	TerrainBoundary,
	MirrorBox,
	Bridge,
	Enemy,
	Mummy,
	Boss,
	Arrow,
	PlayerArrow,
	MonsterArrow,
	Mummylaser,
	Mummylaser2,
	Mummylaser3,
	FireBall,
	Billboard,
	Fog,
	Count,
};

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

	bool m_IsFocusOn = true;
	//protected:
	//	GAME_STATE	m_GameState;

public:
	CScene();
	virtual ~CScene();

	virtual void Update(float elapsedTime) {}
	virtual void UpdateForMultiplay(float elapsedTime) {}

	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void DrawPlayer(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void DrawPlayer_Shadow(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void DrawUI(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void FadeInOut(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void DrawMinimap(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV) {}
	virtual void DrawMirror(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dRTV) {}
	virtual void DrawShadow(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void DrawFont(ID3D12GraphicsCommandList* pd3dCommandList) {}
public:
	// about Keyboard process
	virtual void ProcessInput() {}
	virtual void ProcessWindowKeyboard(WPARAM wParam, bool isKeyUp) {}

	// about Mouse process
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

public:
	virtual void Communicate(SOCKET& sock) {}

	virtual void ProcessPacket(unsigned char* p_buf) {}
	virtual void DoRecv();

	virtual void LoginToServer() {}
	virtual void LogoutToServer() {}

	virtual void DeletePlayer(int playerId) {}
	virtual void AddPlayer(int palyerId) {}
public: 
	virtual void SendDataToNextScene(void* context) {}	
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) {}
	virtual void ReleaseUploadBuffers() {}
	
	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) {}
	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void BuildSceneFrameData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void BuildFbxManager() {}

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) { return NULL; }
	virtual ID3D12RootSignature* GetGraphicsRootSignature() { return NULL; } 

public:
	void SetFocus(bool focusData) { m_IsFocusOn = focusData; }
protected:
	template<class SceneName>
	void ChangeScene(void* pContext = nullptr) { CFramework::GetInstance().ChangeScene<SceneName>(pContext); }
};  

class CNullScene : public CScene
{
public:
	CNullScene();
	~CNullScene();

	virtual void Update(float elapsedTime) override;
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
