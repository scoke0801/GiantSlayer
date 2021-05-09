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
	CGameObject* m_Pillar;          // 기둥
	CGameObject* m_Board;           // 표지판
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


// Puzzle Plate
class CPlate :public CGameObject
{
private:
	vector<CGameObject*> m_Objects;
	 
public:
	CPlate(OBJECT_ID id);
	~CPlate();

public:
	void SetPosition(XMFLOAT3 pos) override;
	void Rotate(const XMFLOAT3& axis, float angle) override;
	 
	bool CollisionCheck(const BoundingBox& aabb) override;

	void UpdateColliders() override;
};

class CPuzzle : public CGameObject
{
private:
	vector<CGameObject*> m_Objects;

public:
	CPuzzle(OBJECT_ID id);
	~CPuzzle();

public:
	void SetPosition(XMFLOAT3 pos) override;
	 
	bool CollisionCheck(const BoundingBox& aabb) override;

	void UpdateColliders() override;
};

class CPuzzleBox : public CGameObject
{
	// 밀기 용으로 사용하기 위해 별도 정의
public:
	CPuzzleBox(OBJECT_ID id);
	~CPuzzleBox();
};