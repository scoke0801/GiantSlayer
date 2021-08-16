#pragma once

#include "Mesh.h" 
#include "Colider.h"  
#include "FbxLoader.h" 

class CShader;
class CCamera;
class CTerrain;
class CSkill;

#define OBJECT_MAX_VELOCITY 120.0f 
#define PLAYER_RUN_VELOCITY 650.0f
#define PLAYER_WALK_VELOCITY 80.0f  

enum class OBJ_TYPE
{
	Object = 0,
	Player,
	Enemy,
	Mummy,
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
enum class Laser_TYPE
{
	Laser1=0,
	Laser2 = 1,
	Laser3 = 2,
	Laser4 = 3,
	Laser5 = 4,
};
enum class Chess_Type
{
	King=0,
	Knight=1,
	Pawn=2,
	Rook=3,
};

enum class COLLISION_HANDLE_TYPE : int {
	NotCollide = 0,
	Stop,
	On,
	Attacked
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

enum class ObjectState {
	Wait,		// ���¿� ���� ������ ������
	Idle,		// ��� ����
	Patrol,		// Ž�� ����
	Trace,		// �߰�
	Attack,		// ����
	Attacked,	// �ǰ�
	Die,		// ���
	RunAway,	// ����
	Mummy_1_Die_Anger,// �̶� �Ѹ��� �׾����� �ٸ� �̶���� ���ֻ��� 
	Mummy_2_Die_Anger,// �̶� �θ��� �׾����� �ٸ� �̶���� ���ֻ��� 
	BossSkill_1,
	BossSkill_2,
	BossSkill_3,
	BossSkill_4,
	BossSkill_5,
	BossBorn
};

enum class SECTOR_POSITION {
	SECTOR_1,
	SECTOR_2,
	SECTOR_3,
	SECTOR_4,
	SECTOR_5,
};

class CGameObject
{
public:
	XMFLOAT4X4			m_xmf4x4ToParent;
private:
	int					m_nReferences = 0;

public:
	XMFLOAT4X4			m_xmf4x4World;

protected:	// ��ǥ ���� ����
	SECTOR_POSITION		m_ExistingSector = SECTOR_POSITION::SECTOR_1;

	// frame update loop, update ���� ���� ��ǥ
	XMFLOAT3			m_xmf3Position = XMFLOAT3{ 0,0,0 };
	// frame update loop, update ���� ���� ��ǥ
	XMFLOAT3			m_xmf3PrevPosition = XMFLOAT3{ 0,0,0 };

	XMFLOAT3			m_xmf3Velocity = XMFLOAT3{ 0,0,0 };
	XMFLOAT3			m_xmf3Size = XMFLOAT3{ 0,0,0 };
	 
	float				m_HeightFromTerrain = 0.0f;

protected:// �浹ó�� ���� ����
	vector<Collider*>	m_Colliders;
	vector<Collider*>	m_AABB; 

	COLLISION_HANDLE_TYPE m_CollisionHandleType = COLLISION_HANDLE_TYPE::Stop;
	 
protected: // ������ ���� ����
	CMesh*				m_pMesh = NULL;
	vector<CMesh*>		m_BoundingObjectMeshes;

	CShader*			m_pShader = NULL;

	UINT				m_nTextureIndex = 0x00;

	MATERIAL*			m_Material;

	CCamera*			m_Camera = nullptr;

	Laser_TYPE m_LaserType = Laser_TYPE::Laser1;
	Chess_Type m_ChessType = Chess_Type::King;

	UINT				m_Laser = 0;
	//EnemyAttackType m_EnemyAttackType = EnemyAttackType::Mummy1;

	bool				m_isDrawable = true;
protected:	// ��ü ���� �Ӽ� ����
	int					m_HP = 0;
	int					m_SP = 0;
	float				m_ATK = 0;
	float				m_DEF = 0;

	OBJ_NAME			m_Name;
	OBJ_TYPE			m_Type = OBJ_TYPE::Object;
	bool				m_isCollidable = true;
	bool				isCanDamaged = true;
	CSkill*				m_Skill;


	//ObjectState			m_StateType = ObjectState::Idle;

public:
	FbxScene*			m_pfbxScene = NULL;

private:	// GPU ���� ������
	ID3D12Resource*		m_pd3dcbGameObject = NULL;
	GAMEOBJECT_INFO*	m_pcbMappedGameObjInfo = NULL;

public:
	CGameObject();
	virtual ~CGameObject();

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseUploadBuffers();

public:
	// �÷��̾ ��ġ�� ������, ��ü���� ��ġ�� ������ ���Ͽ�
	// ������ ������ �ִ��� Ȯ���Ѵ�. 
	bool IsInNearSector(bool* playerSector) const;
	bool IsInSameSector(bool* playerSector) const; 
	bool IsInSameSector(SECTOR_POSITION sectorPos) const;

	SECTOR_POSITION GetExistingSector() const { return m_ExistingSector; }
	void SetExistingSector(SECTOR_POSITION sectorPos) { m_ExistingSector = sectorPos; }
	void SetExistingSector();

	virtual void Animate(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void UpdateOnServer(float fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera); 

	void DrawForBoundingObj(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
 
	virtual void Draw_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera); 
	virtual void SetTargetVector(const XMFLOAT3& playerLookAt);

	

public:
	virtual void Move(XMFLOAT3 shift);
	void Move();

	virtual void Rotate(XMFLOAT3 pxmf3Axis, float fAngle);

	virtual void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	virtual void Scale(float x, float y, float z, bool setSize = true);

public:
	// about collision
	virtual bool CollisionCheck(Collider* pCollider);
	virtual bool CollisionCheck(CGameObject* other);

	void FixCollision();
	virtual void FixCollision(CGameObject* pCollideObject);

	virtual void UpdateColliders();

	void AddColider(Collider* pCollider) { m_Colliders.push_back(pCollider); AddAABB(pCollider); }
	void AddAABB(Collider* pCollider);

	int GetColliderCount() const { return m_Colliders.size(); }
	virtual vector<Collider*>& GetColliders() { return m_Colliders; }
	virtual vector<Collider*>& GetAABB() { return m_AABB; }

	virtual void FixPositionByTerrain(CTerrain* pTerrain);
	 
	virtual void ChangeState(ObjectState stateInfo, void* pData) {}

	
public:
	// about bounding box 
	void BuildBoundigBoxMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth, float fHeight, float fDepth,
		const XMFLOAT3& shift);
	void BuildBoundigSphereMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		PulledModel pulledModel,
		float radius, UINT32 sliceCount, UINT32 stackCount,
		const XMFLOAT3& shift);
	//�޽��� �߽ɿ��� ���۵��� �ʰ� 
	// ���� Ȥ�� �����ʿ��� �����ϴ� ��� ex) �� ��ü
	void BuildBoundigBoxMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		PulledModel pulledModel,
		float fWidth, float fHeight, float fDepth,
		const XMFLOAT3& shift); 
public:
	XMFLOAT3 GetPositionToParent() { return(XMFLOAT3(m_xmf4x4ToParent._41, m_xmf4x4ToParent._42, m_xmf4x4ToParent._43)); }
	XMFLOAT3 GetPosition() { return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }
	string GetObjectName() const { return ConvertToObjectName(m_Name); }
	XMFLOAT3 GetVelocity() const { return m_xmf3Velocity; }
	XMFLOAT4X4 GetWorldTransform() const { return m_xmf4x4World; }

	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL) {}
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void SetPosition(XMFLOAT3 pos);
	virtual void SetPositionPlus(XMFLOAT3 pos);
	//void SetVelocity(XMFLOAT3 vel); 
	virtual void SetVelocity(OBJ_DIRECTION direction);
	virtual void SetVelocity(XMFLOAT3 dir);

	virtual void SetLaserType(Laser_TYPE laser);

	virtual void SetLaser(UINT laser);
	
	UINT GetLaser() const { return m_Laser; };

	Laser_TYPE GetLaserType() const { return m_LaserType; };

	void LookAtDirection(XMFLOAT3 dir, void* pContext);
	void LookAtDirections(XMFLOAT3 dir);
	void SetVelocityToZero() { m_xmf3Velocity = XMFLOAT3(0, 0, 0); }

	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
	void SetTextureIndex(UINT index) { m_nTextureIndex = index; }
	//void SetMaterial(CMaterial* pMaterial, int rootParameterIndex) { m_Material = pMaterial; m_MaterialParameterIndex = rootParameterIndex; }
	void SetObjectName(const OBJ_NAME& name) { m_Name = name; }

	// Set / Get connected Camera
	void SetCamera(CCamera* camera) { m_Camera = camera; }
	CCamera* GetCamera() const { return m_Camera; }

	void SetChess(Chess_Type chess) { m_ChessType = chess; }
	Chess_Type GetChess() const { return m_ChessType; }

	virtual void SetAnimationStack(int nAnimationStack) { }

	void SetHP(UINT HP) { m_HP = HP; }
	UINT GetHP() const { return m_HP; }
	void TakeDamage(float dmg) { 
		if (dmg < m_DEF)
			return;
		m_HP -= (dmg - m_DEF);
		if (m_HP <= 0) m_HP = 0;
	}

	void SetSP(UINT SP) { m_SP = SP; }
	UINT GetSP() const { return m_SP; }

	void SetATK(UINT ATK) { m_ATK = ATK; }
	UINT GetATK() const { return m_ATK; }

	void SetDEF(UINT DEF) { m_DEF = DEF; }
	UINT GetDEF() const { return m_DEF; }

	void SetSize(const XMFLOAT3& size) { m_xmf3Size = size; }
	XMFLOAT3 GetSize()const { return m_xmf3Size; }
	 
	virtual void SetDrawable(bool drawable) { m_isDrawable = drawable; }
	bool IsDrawable() const { return m_isDrawable; }

	bool IsCanDamaged() { return isCanDamaged; }
	void SetCanDamaged(bool param) { isCanDamaged = param; }

	void SetHegithFromTerrain(float height) { m_HeightFromTerrain = height; }
	float GetHeightFromTerrain() const { return m_HeightFromTerrain; }

	void SetCollisionHandleType(COLLISION_HANDLE_TYPE type) { m_CollisionHandleType = type; }
	COLLISION_HANDLE_TYPE GetCollisionHandleType() const { return m_CollisionHandleType; }
	
	virtual ObjectState GetStateInfo() const { return ObjectState::Wait; }
public:
	DirectX::XMFLOAT3 GetRight()const;
	DirectX::XMFLOAT3 GetUp()const;
	DirectX::XMFLOAT3 GetLook()const; 

	DirectX::XMFLOAT3 GetReflectLook_0()const;
	DirectX::XMFLOAT3 GetReflectLook_1()const;
	DirectX::XMFLOAT3 GetReflectLook_2()const;
	DirectX::XMFLOAT3 GetReflectLook_P()const;

	void InverseDirection() {
		//m_xmf4x4World._21 *= -1;
		//m_xmf4x4World._22 *= -1;
		//m_xmf4x4World._23 *= -1;

		m_xmf3Velocity = Vector3::Multifly(m_xmf3Velocity, -1);
	};
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
	bool m_SelectedBox;
	bool m_GripBox;


public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	void SetSelectBox(bool SelectedBox) { m_SelectedBox = SelectedBox; }
	bool GetSelectBox() { return m_SelectedBox; }

	void SetGripBox(bool GripBox) { m_GripBox = GripBox; }
	bool GetGriptBox() { return m_GripBox; }

	virtual void Animate(float fTimeElapsed) {}
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

	void Update(float timeElapsed);
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
