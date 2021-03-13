#pragma once
#include "Mesh.h"
#include "FbxSceneContext.h"

class CShader;
class CCamera;
 
#define OBJECT_MAX_VELOCITY 120.0f
#define PLAYER_RUN_VELOCITY 250.0f
#define PLAYER_WALK_VELOCITY 80.0f 

enum class OBJ_TYPE
{
	Object = 0,
	Player,
	Enemy,
	Obstacle,
};
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

enum class Map_Configuration_Name
{
	First_Ground =0,
	Second_Ground,
	Third_Ground,
	Fourth_Ground,
	Boss_Ground,

	Plane_Pattern_Ground,
	Plane_Pattern_Ground_Vertical,

	Up_Pattern_Ground,
	Up_Pattern_Ground_Rotate,

	Down_Pattern_Ground,
	Down_Pattern_Ground_Rotate,

	Cliff_Pattern_Ground,
	Cliff_Pattern_Ground_Vertical,
	Cliff_Pattern_Ground_Rotate,
};

struct MapData
{
	Map_Configuration_Name Name;
	int Height;
};

enum class OBJ_DIRECTION
{
	Front = 0,
	Back,
	Left,
	Right
};
string ConvertToObjectName(const OBJ_NAME& name);

struct GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	MATERIAL						m_Material;
	UINT							m_nTextureIndex;
};

class CGameObject
{
private:
	int					m_nReferences = 0;
	 
protected:	// 좌표 관련 변수
	XMFLOAT4X4			m_xmf4x4World;

	XMFLOAT3			m_xmf3Position = XMFLOAT3{ 0,0,0 };
	XMFLOAT3			m_xmf3Velocity = XMFLOAT3{ 0,0,0 };
	XMFLOAT3			m_xmf3Size = XMFLOAT3{ 0,0,0 };

protected: // 렌더링 관련 변수
	CMesh*				m_pMesh = NULL;
	CShader*			m_pShader = NULL;
 
	UINT				m_nTextureIndex = 0x00;
	 
	MATERIAL*			m_Material;
	  
	CCamera*			m_Camera = nullptr;

protected:	// 객체 관련 속성 변수
	UINT				m_HP = 0;
	UINT				m_SP = 0;

	OBJ_NAME			m_Name;
	OBJ_TYPE			m_Type = OBJ_TYPE::Object;
	bool				m_isCollidable = true;

private:	// GPU 전달 데이터
	ID3D12Resource*		m_pd3dcbGameObject = NULL;
	GAMEOBJECT_INFO*	m_pcbMappedGameObjInfo = NULL;

public:
	CGameObject();
	virtual ~CGameObject();

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}

	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseUploadBuffers(); 

public:
	virtual void Animate(float fTimeElapsed);
	virtual void Update(double fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	virtual void Move(XMFLOAT3 shift);
	void Move();

	void Rotate(XMFLOAT3 pxmf3Axis, float fAngle);

	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	bool CollisionCheck() { return false; };

	void Scale(float x, float y, float z, bool setSize = true);

public:
	XMFLOAT3 GetPosition() { return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }
	string GetObjectName() const { return ConvertToObjectName(m_Name); }
	XMFLOAT3 GetVelocity() const { return m_xmf3Velocity; }
	XMFLOAT4X4 GetWorldTransform() const { return m_xmf4x4World; }

	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void SetPosition(XMFLOAT3 pos);
	virtual void SetPositionPlus(XMFLOAT3 pos);
	void SetVelocity(XMFLOAT3 vel); 
	virtual void SetVelocity(OBJ_DIRECTION direction);

	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
	void SetTextureIndex(UINT index) { m_nTextureIndex = index; }
	//void SetMaterial(CMaterial* pMaterial, int rootParameterIndex) { m_Material = pMaterial; m_MaterialParameterIndex = rootParameterIndex; }
	void SetObjectName(const OBJ_NAME& name) { m_Name = name; }

	// Set / Get connected Camera
	void SetCamera(CCamera* camera) { m_Camera = camera; }
	CCamera* GetCamera() const { return m_Camera; }

	void SetHP(UINT HP) { m_HP = HP; }
	UINT GetHP() const { return m_HP; }

	void SetSP(UINT SP) { m_SP = SP; }
	UINT GetSP() const { return m_SP; }
public:
	DirectX::XMFLOAT3 GetRight()const;
	DirectX::XMFLOAT3 GetUp()const;
	DirectX::XMFLOAT3 GetLook()const; 
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

class CFbxObject : public CGameObject
{
public:
	CFbxObject();
	CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		FbxManager* pfbxSdkManager, FbxScene* pfbxScene, char* pstrFbxFileName);
	virtual ~CFbxObject();

public:
	FbxScene*				m_pfbxScene = NULL;
	CAnimationController*	m_pAnimationController = NULL;

public:
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
protected:
	int						m_nObjects;
	CGameObject**			m_ppObjects;

	CSkyBox() {}
public:
	
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CShader* pShader);
	 
	virtual ~CSkyBox();

	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Rotate(XMFLOAT3 pxmf3Axis, float fAngle); 
};

class CSkyBoxSphere : public CSkyBox
{ 
public: 
	CSkyBoxSphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CShader* pShader, float radius, UINT32 sliceCount, UINT32 stackCount);
	virtual ~CSkyBoxSphere();
};
 
class CBillboard
{
public:
	CBillboard(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CBillboard();

	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
private:
	int						m_nBillboardObjects;
	vector<CGameObject*>    m_BillboardObjects;

};
