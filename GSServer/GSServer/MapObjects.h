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
	bool CollisionCheck(CGameObject* other) override;

	void UpdateColliders() override;

private:
	vector<CGameObject*> m_Plates; 
	vector<CGameObject*> m_Objects; 

	// �迭 ��� �߿��� �浹�� ��ü
	CGameObject* m_CollideObject = nullptr;
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
	CGameObject* m_Pillar;          // ���
	CGameObject* m_Board;           // ǥ����
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
	float m_IsOnOpening = true;

	bool m_IsOnAnimating = false;

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
	CGameObject* m_CollideObject;

public:
	CPlate(OBJECT_ID id);
	~CPlate();

public:
	void SetPosition(XMFLOAT3 pos) override;
	void Rotate(const XMFLOAT3& axis, float angle) override;
	 
	bool CollisionCheck(const BoundingBox& aabb) override;

	bool CollisionCheck(CGameObject* other) override;

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
	// �б� ������ ����ϱ� ���� ���� ����
public:
	CPuzzleBox(OBJECT_ID id);
	~CPuzzleBox();
};