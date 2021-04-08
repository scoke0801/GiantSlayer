#pragma once
#include "GameObject.h"
class CBridge :  public CGameObject
{
public: 
	CBridge(OBJECT_ID id) {}
	~CBridge() {}
};

class CSign : public CGameObject
{ 
	CGameObject* m_Pillar;          // ±âµÕ
	CGameObject* m_Board;           // Ç¥ÁöÆÇ

public: 
	CSign(OBJECT_ID id);
	~CSign();

	void SetPosition(const XMFLOAT3& pos) override;
	void Rotate(const XMFLOAT3& axis, float angle) override;

	bool CollisionCheck(const BoundingBox& aabb) override;
};


class CDoor : public CGameObject
{
private:
	bool m_IsLeft;

	float m_fAngle = 0.0f;
	float m_IsOpening = true;

	float m_Height;
public:
	CDoor(bool isLeft);
	~CDoor();

public:
	void Update(float fTimeElapsed) override; 
};

class CDoorWall : public CGameObject
{
	CDoor* m_LeftDoor;
	CDoor* m_RightDoor;
	vector<CGameObject*>  m_Walls;

public:
	CDoorWall(OBJECT_ID id);
	~CDoorWall();

public:
	void Update(float fTimeElapsed) override;
	
	void SetPosition(const XMFLOAT3& pos) override;
	void Rotate(const XMFLOAT3& axis, float angle) override;

	bool CollisionCheck(const BoundingBox& aabb) override;
};