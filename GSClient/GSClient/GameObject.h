#pragma once
#include "Mesh.h"
#include "FbxSceneContext.h"
#include "FbxLoader.h"

class CShader;
class CCamera;
 
#define OBJECT_MAX_VELOCITY 120.0f
#define PLAYER_RUN_VELOCITY 250.0f
#define PLAYER_WALK_VELOCITY 80.0f 

enum class OBJ_NAME
{
	None = 0,
	Terrain = 1,
	Box = 2,
	Player = 3, 
	SkyBox = 10,
	Bridge = 11,
	Wall = 12,
	Door = 13
};

enum class OBJ_DIRECTION
{
	Front = 0,
	Back,
	Left,
	Right
};
string ConvertToObjectName(const OBJ_NAME& name);

//struct CB_GAMEOBJECT_INFO
//{
//	XMFLOAT4X4						m_xmf4x4World;
//	UINT							m_nObjectID;
//	UINT							m_nMaterialID;
//};

class CGameObject
{
private:
	int			m_nReferences = 0;
	 
protected:
	XMFLOAT4X4	m_xmf4x4World;

	XMFLOAT3	m_xmf3Position;
	//XMFLOAT3	m_xmf3Right = { 1.0f, 0.0f, 0.0f };
	//XMFLOAT3	m_xmf3Up = { 0.0f, 1.0f, 0.0f };
	//XMFLOAT3	m_xmf3Look = { 0.0f, 0.0f, 1.0f };
	XMFLOAT3	m_xmf3Velocity;

	XMFLOAT3	m_xmf3Scale;
	XMFLOAT3	m_xmf3Rotation;
	XMFLOAT3	m_xmf3Translation;

	CMesh*		m_pMesh = NULL;
	CShader*	m_pShader = NULL;

	UINT		m_nTextureIndex = 0x00;
	 
//CMaterial*	m_Material = nullptr;
//int			m_MaterialParameterIndex = 2;

	OBJ_NAME	m_Name; 

protected:
	CCamera*	m_Camera = nullptr;

public:

public:
	CGameObject();
	virtual ~CGameObject();

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
	 
	virtual void ReleaseUploadBuffers(); 
public:
	virtual void Animate(float fTimeElapsed);
	virtual void Update(double fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	virtual void Move(XMFLOAT3 shift);
	void Move();

	//void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT3 pxmf3Axis, float fAngle);
	//void Rotate(float x, float y, float z);

	bool CollisionCheck() { return false; };

public:
	XMFLOAT3 GetPosition() { return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }
	string GetObjectName() const { return ConvertToObjectName(m_Name); }
	XMFLOAT3 GetVelocity() const { return m_xmf3Velocity; }
	XMFLOAT4X4 GetWorldTransform() const { return m_xmf4x4World; }

	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void SetPosition(XMFLOAT3 pos);
	void SetVelocity(XMFLOAT3 vel); 
	virtual void SetVelocity(OBJ_DIRECTION direction);

	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
	void SetTextureIndex(UINT index) { m_nTextureIndex = index; }
	//void SetMaterial(CMaterial* pMaterial, int rootParameterIndex) { m_Material = pMaterial; m_MaterialParameterIndex = rootParameterIndex; }
	void SetObjectName(const OBJ_NAME& name) { m_Name = name; }

	// Set / Get connected Camera
	void SetCamera(CCamera* camera) { m_Camera = camera; }
	CCamera* GetCamera() const { return m_Camera; }

public:
	DirectX::XMFLOAT3 GetRight()const;
	DirectX::XMFLOAT3 GetUp()const;
	DirectX::XMFLOAT3 GetLook()const; 
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed);
};

class CBox : public CGameObject
{
public:
	CBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float width, float height, float depth);
	virtual ~CBox();

private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed) {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define SKINNED_ANIMATION_BONES		128
//
//#define ANIMATION_TYPE_ONCE			0
//#define ANIMATION_TYPE_LOOP			1
//#define ANIMATION_TYPE_PINGPONG		2
//
//#define ANIMATION_CALLBACK_EPSILON	0.015f
//
//struct CALLBACKKEY
//{
//	float  							m_fTime = 0.0f;
//	void* m_pCallbackData = NULL;
//};
//
//#define _WITH_ANIMATION_INTERPOLATION
//
//class CAnimationCallbackHandler
//{
//public:
//	CAnimationCallbackHandler() { }
//	~CAnimationCallbackHandler() { }
//
//public:
//	virtual void HandleCallback(void* pCallbackData) { }
//};
//
//class CAnimationCurve
//{
//public:
//	CAnimationCurve(int nKeys);
//	~CAnimationCurve();
//
//public:
//	int								m_nKeys = 0;
//
//	float* m_pfKeyTimes = NULL;
//	float* m_pfKeyValues = NULL;
//
//public:
//	float GetValueByLinearInterpolation(float fPosition);
//};

//class CAnimationLayer
//{
//public:
//	CAnimationLayer();
//	~CAnimationLayer();
//
//public:
//	float							m_fWeight = 1.0f;
//
//	int								m_nAnimatedBoneFrames = 0;
//	CGameObject** m_ppAnimatedBoneFrameCaches = NULL; //[m_nAnimatedBoneFrames]
//
//	CAnimationCurve* (*m_ppAnimationCurves)[9] = NULL;
//
//public:
//	void LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE* pInFile);
//
//	XMFLOAT4X4 GetSRT(int nBoneFrame, float fPosition, float fTrackWeight);
//};
//
//class CAnimationSet
//{
//public:
//	CAnimationSet(float fStartTime, float fEndTime, char* pstrName);
//	~CAnimationSet();
//
//public:
//	char							m_pstrAnimationSetName[64];
//
//	int								m_nAnimationLayers = 0;
//	CAnimationLayer* m_pAnimationLayers = NULL;
//
//	float							m_fStartTime = 0.0f;
//	float							m_fEndTime = 0.0f;
//	float							m_fLength = 0.0f;
//
//	float 							m_fPosition = 0.0f;
//	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong
//
//	int 							m_nCallbackKeys = 0;
//	CALLBACKKEY* m_pCallbackKeys = NULL;
//
//	CAnimationCallbackHandler* m_pAnimationCallbackHandler = NULL;
//
//public:
//	void SetPosition(float fTrackPosition);
//
//	void Animate(float fTrackPosition, float fTrackWeight);
//
//	void SetCallbackKeys(int nCallbackKeys);
//	void SetCallbackKey(int nKeyIndex, float fTime, void* pData);
//	void SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler);
//
//	void* GetCallbackData();
//};
//
//class CAnimationSets
//{
//private:
//	int								m_nReferences = 0;
//
//public:
//	void AddRef() { m_nReferences++; }
//	void Release() { if (--m_nReferences <= 0) delete this; }
//
//public:
//	CAnimationSets(int nAnimationSets);
//	~CAnimationSets();
//
//public:
//	int								m_nAnimationSets = 0;
//	CAnimationSet** m_ppAnimationSets = NULL;
//
//public:
//	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
//	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void* pData);
//	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler);
//};

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

	int 							m_nAnimationSet = 0;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	void SetPosition(float fPosition) { m_fPosition = fPosition; }
};

//////////////////////////////////////////////////////////////////////////////
//

class CAnimationController
{
public:
	CAnimationController(FbxScene* pfbxScene);
	~CAnimationController();

public:
	float 					m_fTime = 0.0f;

	int 					m_nAnimationStacks = 0;
	FbxAnimStack** m_ppfbxAnimationStacks = NULL;

	int 					m_nAnimationStack = 0;

	FbxTime* m_pfbxStartTimes = NULL;
	FbxTime* m_pfbxStopTimes = NULL;

	FbxTime* m_pfbxCurrentTimes = NULL;

public:
	void SetAnimationStack(FbxScene* pfbxScene, int nAnimationStack);

	void AdvanceTime(float fElapsedTime);
	FbxTime GetCurrentTime() { return(m_pfbxCurrentTimes[m_nAnimationStack]); }

	void SetPosition(int nAnimationStack, float fPosition);
};

class CFbxObject : public CGameObject
{
public:
	CFbxObject();
	CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
			FbxManager* pfbxSdkManager, char* pstrFbxFileName);
	virtual ~CFbxObject();

public:
	FbxScene*				m_pfbxScene = NULL;
	FbxLoader*				m_pfbxLoader = NULL;
	CAnimationController*	m_pAnimationController = NULL;

public:
	void LoadSkeletonHierarchy(FbxNode* inRootNode);
	void LoadSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex);

	virtual void Animate(float fTimeElapsed);
	virtual void TestAnimate(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	
	static void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	static void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, FbxAMatrix* pfbxf4x4World);

	virtual void ReleaseUploadBuffers();

	void SetAnimationStack(int nAnimationStack) { m_pAnimationController->SetAnimationStack(m_pfbxScene, nAnimationStack); }
};

//////////////////////////////////////////////////////////////////////////////
//

class CSkyBox  
{
private:
	int						m_nObjects;
	CGameObject**			m_ppObjects;

public:
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CShader* pShader);
	virtual ~CSkyBox();

	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};

