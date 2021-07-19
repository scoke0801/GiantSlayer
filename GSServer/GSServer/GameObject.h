#pragma once 

//class CCamera;

#define OBJECT_MAX_VELOCITY 120.0f
#define PLAYER_RUN_VELOCITY 250.0f * 4
#define PLAYER_WALK_VELOCITY 80.0f 

enum class OBJ_NAME
{
	None = 0,
	Terrain = 1,
	Box = 2,
	Player = 3,
	SkyBox = 4,
	Bridge = 5,
	DoorWall = 6,
	Sign = 7,
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

enum class ObjectState {
	Wait,		// 상태와 상태 사이의 대기상태
	Idle,		// 평소 상태
	Patrol,		// 탐색 상태
	Trace,		// 추격
	Attack,		// 공격
	Attacked,	// 피격
	Die,		// 사망
	RunAway,	// 도망
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
private:
	int					m_nReferences = 0;

protected:
	XMFLOAT4X4			m_xmf4x4World;

	// frame update loop, update 갱신 후의 좌표
	XMFLOAT3			m_xmf3Position = XMFLOAT3{ 0,0,0 };
	// frame update loop, update 갱신 전의 좌표
	XMFLOAT3			m_xmf3PrevPosition = XMFLOAT3{ 0,0,0 };
	XMFLOAT3			m_xmf3Velocity = XMFLOAT3{ 0,0,0 };
	XMFLOAT3			m_xmf3Size = XMFLOAT3{ 0,0,0 };

	OBJ_NAME			m_Name;
	int					m_HP = 0;
	int					m_SP = 0;

	float				m_HeightFromTerrain = 0.0f;

	COLLISION_HANDLE_TYPE m_CollisionHandleType = COLLISION_HANDLE_TYPE::Stop;

	// 재활용 처리를 위한 변수
	bool				m_isUsing = true;

	SECTOR_POSITION		m_ExistingSector = SECTOR_POSITION::SECTOR_1;

protected:// 충돌처리 관련 변수
	vector<BoundingBox>	m_BoundingBox;
	vector<BoundingBox>	m_AABB;

public:
	CGameObject();
	virtual ~CGameObject();

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	virtual void Update(float fTimeElapsed);

public:
	virtual void Move(XMFLOAT3 shift);
	void Move();

	virtual void Rotate(const XMFLOAT3& pxmf3Axis, float fAngle);

	virtual void Scale(float x, float y, float z, bool setSize = true);

	virtual void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

public:
	XMFLOAT3 GetPosition() { return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }
	string GetObjectName() const { return ConvertToObjectName(m_Name); }
	XMFLOAT3 GetVelocity() const { return m_xmf3Velocity; }
	XMFLOAT4X4 GetWorldTransform() const { return m_xmf4x4World; }

	virtual void SetPosition(XMFLOAT3 pos);
	virtual void SetVelocity(const XMFLOAT3& vel);
	virtual void SetVelocity(OBJ_DIRECTION direction);

	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
	void SetObjectName(const OBJ_NAME& name) { m_Name = name; }

	virtual void SetIsUsable(bool drawable) { m_isUsing = drawable; }
	bool IsUsable() const { return m_isUsing; }

public: // about sectoring for updates
	// 플레이어가 위치한 영역과, 객체들이 위치한 영역을 비교하여
	// 인접한 영역에 있는지 확인한다. 
	bool IsInNearSector(bool* playerSector) const;
	bool IsInSameSector(bool* playerSector) const;
	bool IsInSameSector(SECTOR_POSITION sectorPos) const;

	SECTOR_POSITION GetExistingSector() const { return m_ExistingSector; }
	void SetExistingSector(SECTOR_POSITION sectorPos) { m_ExistingSector = sectorPos; }

public:
	// about collision
	virtual bool CollisionCheck(const BoundingBox& pCollider);
	virtual bool CollisionCheck(CGameObject* other);

	void FixCollision(); 
	virtual void FixCollision(CGameObject* pCollideObject);

	virtual void UpdateColliders();

	void AddBoundingBox(const BoundingBox& boundingBox) { m_BoundingBox.push_back(boundingBox); AddAABB(boundingBox); }
	void AddAABB(const BoundingBox& boundingBox);

	int GetColliderCount() const { return m_BoundingBox.size(); }
	 
	vector<BoundingBox>& GetColliders() { return m_BoundingBox; }
	vector<BoundingBox>& GetAABB() { return m_AABB; }

	void SetCollisionHandleType(COLLISION_HANDLE_TYPE type) { m_CollisionHandleType = type; }
	COLLISION_HANDLE_TYPE GetCollisionHandleType() const { return m_CollisionHandleType; }
	
	void SetSize(const XMFLOAT3& size) { m_xmf3Size = size; }
	XMFLOAT3 GetSize()const { return m_xmf3Size; }

	 virtual void FixPositionByTerrain(int heightsMap[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1]);

	virtual void ChangeState(ObjectState stateInfo, void* pData) {}
	virtual ObjectState GetStateInfo() const { return ObjectState::Wait; }
	virtual BYTE GetAnimationType() const { return 0;}

	void SetHP(UINT HP) { m_HP = HP; }
	UINT GetHP() const { return m_HP; }
public:
	DirectX::XMFLOAT3 GetRight()const;
	DirectX::XMFLOAT3 GetUp()const;
	DirectX::XMFLOAT3 GetLook()const;
};
