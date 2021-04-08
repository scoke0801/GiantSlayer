#pragma once 
 
//class CCamera;
 
#define OBJECT_MAX_VELOCITY 120.0f
#define PLAYER_RUN_VELOCITY 250.0f
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

enum class OBJ_DIRECTION
{
	Front = 0,
	Back,
	Left,
	Right
};
string ConvertToObjectName(const OBJ_NAME& name);
 

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
	XMFLOAT3			m_xmf3Velocity;
	   
	OBJ_NAME			m_Name;

	//CCamera*			m_Camera = nullptr;

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
	virtual void Update(double fTimeElapsed); 

public:
	virtual void Move(XMFLOAT3 shift);
	void Move();

	//void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT3 pxmf3Axis, float fAngle);
	//void Rotate(float x, float y, float z);

	void Scale(float x, float y, float z);

public:
	XMFLOAT3 GetPosition() { return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }
	string GetObjectName() const { return ConvertToObjectName(m_Name); }
	XMFLOAT3 GetVelocity() const { return m_xmf3Velocity; }
	XMFLOAT4X4 GetWorldTransform() const { return m_xmf4x4World; }
	 
	virtual void SetPosition(const XMFLOAT3& pos);
	virtual void SetVelocity(const XMFLOAT3& vel);
	virtual void SetVelocity(OBJ_DIRECTION direction);

	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents);
	void SetObjectName(const OBJ_NAME& name) { m_Name = name; }

public:
	// about collision
	virtual bool CollisionCheck(const BoundingBox& pCollider);
	virtual bool CollisionCheck(CGameObject* other);

	void FixCollision();

	virtual void UpdateColliders();

	void AddColider(const BoundingBox& boundingBox) { m_BoundingBox.push_back(boundingBox); AddAABB(boundingBox); }
	void AddAABB(const BoundingBox& boundingBox);

	int GetColliderCount() const { return m_BoundingBox.size(); }

	vector<BoundingBox>& GetColliders() { return m_BoundingBox; }
	vector<BoundingBox>& GetAABB() { return m_AABB; }

public:
	DirectX::XMFLOAT3 GetRight()const;
	DirectX::XMFLOAT3 GetUp()const;
	DirectX::XMFLOAT3 GetLook()const; 
};
 