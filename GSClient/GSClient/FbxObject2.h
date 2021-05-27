#pragma once
#include "GameObject.h"
#include "Mesh.h"

struct FbxSubMesh
{
	int nControlPoint;
	int nPolygon;
	int nDeformer;
	XMFLOAT4X4 gTransform;

	vector<int> mIndex;
	vector<CTexturedVertex> mVertex;

	//vector<double> mClusterWeight;
	//vector<XMFLOAT3> mClusterDef;
	XMFLOAT3* mCP;

	double* mClusterWeight;
	XMFLOAT3* mClusterDef;
};

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

class MeshInfo
{
public:
	MeshInfo() { }
	~MeshInfo() { };

public:
	int numCP, numPG, numDC;

	int* indexCP;
	XMFLOAT3* mControlPoint;

	CMesh* pMesh;
	CShader* pShader;
	bool animation = false;
};

class CFbxObject2 : public CGameObject
{
public:
	CFbxObject2();
	CFbxObject2(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName);
	virtual ~CFbxObject2();

public:
	vector<MeshInfo> vMesh;
	 
	float m_AnimationTime = 0.0f;

public: 
	CAnimationController* m_pAnimationController = NULL;

public:
	void LoadScene(char* pstrFbxFileName, FbxManager* pfbxSdkManager);
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

class CFbxObjectFileLoadVer : public CGameObject
{
public:
	CFbxObjectFileLoadVer();
	CFbxObjectFileLoadVer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFbxFileName);
	virtual ~CFbxObjectFileLoadVer();

public:
	vector<FbxSubMesh>			mFbxMesh;
	vector<MeshInfo>			mMesh;

	int							vertexnum;
	int							m_time;

public:
	void LoadFbxModelFromFile(char* pstrFbxFileName);

	void AnimateFbxMesh(FbxNode* pNode, FbxTime& fbxCurrentTime);
	void DrawFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList);

	void Animate(float fTimeElapsed) override;
	void Update(float fTimeElapsed) override;
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	static void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	static void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void ReleaseUploadBuffers();

	void SetAnimationStack(int nAnimationStack) override;

};