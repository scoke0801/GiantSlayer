#pragma once
#include "GameObject.h"
class CBridge :  public CGameObject
{
public: 
	CBridge(OBJECT_ID id);
	~CBridge() {}

public:
	void SetPosition(XMFLOAT3 pos) override;
	void Rotate(const XMFLOAT3& axis, float angle) override;

	bool CollisionCheck(const BoundingBox& aabb) override;

	void UpdateColliders() override;

private:
	vector<CGameObject*> m_Plates; 
	vector<CGameObject*> m_Objects;
};

class CSign : public CGameObject
{  
public: 
	CSign(OBJECT_ID id);
	~CSign();

	void SetPosition(XMFLOAT3 pos) override;
	void Rotate(const XMFLOAT3& axis, float angle) override;

	bool CollisionCheck(const BoundingBox& aabb) override;

	void UpdateColliders() override;

private:
	CGameObject* m_Pillar;          // ±âµÕ
	CGameObject* m_Board;           // Ç¥ÁöÆÇ
};


class CDoor : public CGameObject
{
public:
	CDoor(bool isLeft);
	~CDoor();

public:
	void Update(float fTimeElapsed) override; 

private:
	bool m_IsLeft;

	float m_fAngle = 0.0f;
	float m_IsOpening = true;

	float m_Height;
};

class CDoorWall : public CGameObject
{
public:
	CDoorWall(OBJECT_ID id);
	~CDoorWall();

public:
	void Update(float fTimeElapsed) override;
	
	void SetPosition(XMFLOAT3 pos) override;
	void Rotate(const XMFLOAT3& axis, float angle) override;

	bool CollisionCheck(const BoundingBox& aabb) override;

	void UpdateColliders() override;

private:
	CDoor* m_LeftDoor;
	CDoor* m_RightDoor;
	vector<CGameObject*>  m_Walls;
};