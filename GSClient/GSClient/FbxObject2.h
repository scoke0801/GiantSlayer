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

struct Bone2
{
	FbxAMatrix offset;
	int pIndex = -1;
	string name;
};

struct ControlPoint2
{
	XMFLOAT3 pos;
	vector<int> indices;
	vector<double> weights;
};

struct Vertex2
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;

	vector<int> indices;
	vector<double> weights;
};

struct Keyframe2
{
	float frameTime;
	FbxAMatrix aniMatrix;

	bool operator == (const Keyframe2& rhs) {
		if (aniMatrix != rhs.aniMatrix)
			return false;

		return true;
	}
};

struct AnimBones
{
	vector<Keyframe2> keyframes;

	float GetAnimLength() {
		return keyframes.back().frameTime;
	}
};

struct Animation2
{
	float animLength;
	vector<AnimBones> animBones;
};

class MeshInfo
{
public:
	MeshInfo() { }
	~MeshInfo() { };

public:
	int numCP = 0;
	int	numPG = 0;
	int	numDC = 0;
	bool hasAnimation = false;

	int* indexCP = NULL;

	unordered_map<int, ControlPoint2> mControlPoint;
	vector<Vertex2> vertices;
	CMesh* pMesh = NULL;
	CShader* pShader = NULL;

	vector<Bone2> skeleton;
	vector<Animation2> animations;
};

class CFbxObject2 : public CGameObject
{
public:
	CFbxObject2();
	CFbxObject2(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName);
	virtual ~CFbxObject2();

public:

	vector<MeshInfo> renderinfo;
	vector<MeshInfo> vMesh;
	vector<Bone2> skeleton;
	 
	bool hasAnimation = false;
	float m_AnimationTime = 0.0f;

public: 
	CAnimationController* m_pAnimationController = NULL;

public:
	void LoadScene(char* pstrFbxFileName, FbxManager* pfbxSdkManager);
	void LoadFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pNode);

	void ExploreFbxHierarchy(FbxNode* inRootNode);
	void LoadControlPoints(FbxNode* inNode, MeshInfo* tempMesh);
	void LoadAnimations(FbxNode* inNode, MeshInfo* tempMesh);
	void LoadMesh(FbxNode* inNode, MeshInfo* tempMesh);

	void LoadSkeletonHierarchy(FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);

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
