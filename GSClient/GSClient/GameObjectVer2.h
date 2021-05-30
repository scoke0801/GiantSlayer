#pragma once
#include "GameObject.h"
#include "Mesh.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define ANIMATION_TYPE_ONCE			0
#define ANIMATION_TYPE_LOOP			1
#define ANIMATION_TYPE_PINGPONG		2

struct CALLBACKKEY
{
	float  							m_fTime = 0.0f;
	void* m_pCallbackData = NULL;
};

//#define _WITH_ANIMATION_SRT
#define _WITH_ANIMATION_INTERPOLATION

class CAnimationSet
{
public:
	CAnimationSet();
	~CAnimationSet();

public:
	char							m_pstrName[64];

	float							m_fLength = 0.0f;
	int								m_nFramesPerSecond = 0; //m_fTicksPerSecond

	int								m_nAnimationBoneFrames = 0;

	int								m_nKeyFrameTransforms = 0;
	float* m_pfKeyFrameTransformTimes = NULL;
	XMFLOAT4X4** m_ppxmf4x4KeyFrameTransforms = NULL;

#ifdef _WITH_ANIMATION_SRT
	int								m_nKeyFrameScales = 0;
	float* m_pfKeyFrameScaleTimes = NULL;
	XMFLOAT3** m_ppxmf3KeyFrameScales = NULL;
	int								m_nKeyFrameRotations = 0;
	float* m_pfKeyFrameRotationTimes = NULL;
	XMFLOAT4** m_ppxmf4KeyFrameRotations = NULL;
	int								m_nKeyFrameTranslations = 0;
	float* m_pfKeyFrameTranslationTimes = NULL;
	XMFLOAT3** m_ppxmf3KeyFrameTranslations = NULL;
#endif

	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	int								m_nCurrentKey = -1;

	int 							m_nCallbackKeys = 0;
	CALLBACKKEY* m_pCallbackKeys = NULL;

public:
	float GetPosition(float fPosition);
	XMFLOAT4X4 GetSRT(int nFrame, float fPosition);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void* pData);

	void* GetCallback(float fPosition) { return(NULL); }
};

class CAnimationTrack
{
public:
	CAnimationTrack() { }
	~CAnimationTrack() { }

public:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	CAnimationSet* m_pAnimationSet = NULL;
};

class CAnimationCallbackHandler
{
public:
	virtual void HandleCallback(void* pCallbackData) { }
};

class CAnimationController
{
public:
	CAnimationController(int nAnimationTracks = 1);
	~CAnimationController();

public:
	float 							m_fTime = 0.0f;

	int								m_nAnimationSets = 0;
	CAnimationSet* m_pAnimationSets = NULL;

	int								m_nAnimationSet = 0;

	int								m_nAnimationBoneFrames = 0;
	CGameObjectVer2** m_ppAnimationBoneFrameCaches = NULL;

	int 							m_nAnimationTracks = 0;
	CAnimationTrack* m_pAnimationTracks = NULL;

	int  				 			m_nAnimationTrack = 0;

	CGameObjectVer2* m_pRootFrame = NULL;

public:
	void SetAnimationSet(int nAnimationSet);

	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void* pData);

	void AdvanceTime(float fElapsedTime, CAnimationCallbackHandler* pCallbackHandler);

	int GetAnimationSet() { return m_nAnimationSet; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMaterial;
class CShader;

class CGameObjectVer2 : public CGameObject
{
public:
	CGameObjectVer2();
	CGameObjectVer2(int nMaterials);
	virtual ~CGameObjectVer2();

public:
	char			m_pstrFrameName[64];

	XMFLOAT4X4		m_xmf4x4ToParent;

	CGameObjectVer2* m_pParent = NULL;
	CGameObjectVer2* m_pChild = NULL;
	CGameObjectVer2* m_pSibling = NULL;

	int				m_nMaterials = 0;
	CMaterial**		m_ppMaterials = NULL;

	CAnimationController* m_pAnimationController = NULL;

public:
	//void SetShader(CShader* pShader) override;
	//void SetShader(int nMaterial, CShader* pShader);
	//void SetMaterial(int nMaterial, CMaterial* pMaterial);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void OnPrepareAnimate() { }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	void Animate(float fTimeElapsed) override;
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL) override;

	void SetChild(CGameObjectVer2* pChild, bool bReferenceUpdate = false);

public:
	CGameObjectVer2* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }

	CGameObjectVer2* FindFrame(char* pstrFrameName);

	CTexture* FindReplicatedTexture(_TCHAR* pstrTextureName);

	CGameObjectVer2* GetRootSkinnedGameObject();

public:
	void SetAnimationSet(int nAnimationSet);
	void CacheSkinningBoneFrames(CGameObjectVer2* pRootFrame);

	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObjectVer2* pParent, FILE* pInFile, CShader* pShader);
	void LoadAnimationFromFile(FILE* pInFile);

	static CGameObjectVer2* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObjectVer2* pParent, FILE* pInFile, CShader* pShader);
	static CGameObjectVer2* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader, bool bHasAnimation);
	static void PrintFrameInfo(CGameObjectVer2* pGameObject, CGameObjectVer2* pParent);

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class ExportedObject : public CGameObjectVer2
{
public:
	ExportedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~ExportedObject();

public:
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed);
};