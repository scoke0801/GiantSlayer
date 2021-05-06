#pragma once
#include "GameObject.h"
#include "Mesh.h"

class CAnimationController
{
public:
	CAnimationController(FbxScene* pfbxScene);
	~CAnimationController();

public:
	float 					m_fTime = 0.0f;

	int 					m_nAnimationStacks = 0;
	FbxAnimStack**			m_ppfbxAnimationStacks = NULL;

	int 					m_nAnimationStack = 0;

	FbxTime*				m_pfbxStartTimes = NULL;
	FbxTime*				m_pfbxStopTimes = NULL;

	FbxTime*				m_pfbxCurrentTimes = NULL;

public:
	void SetAnimationStack(FbxScene* pfbxScene, int nAnimationStack);

	void AdvanceTime(float fElapsedTime);
	FbxTime GetCurrentTime() { return(m_pfbxCurrentTimes[m_nAnimationStack]); }

	void SetPosition(int nAnimationStack, float fPosition);
};

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;

	int indices[4];
	XMFLOAT3 weights;
};

class CFbxObject : public CGameObject
{
public:
	CFbxObject();
	CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName);
	virtual ~CFbxObject();

public:
	vector<Vertex> vertices;
	vector<int> indices;

	int m_time;

public:
	//void LoadFile(char* pstrFbxFileName);
	void LoadFile();
	void LoadFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pNode);

	void AnimateFbxMesh(FbxNode* pNode, FbxTime& fbxCurrentTime);
	void DrawFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld);
	
	void Animate(float fTimeElapsed) override;
	void Update(float fTimeElapsed) override;
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	static void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	static void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, FbxAMatrix* pfbxf4x4World);

	virtual void ReleaseUploadBuffers();

	void SetAnimationStack(int nAnimationStack) override;
	void LoadFbxModelFromFile(char* pstrFbxFileName);
};
