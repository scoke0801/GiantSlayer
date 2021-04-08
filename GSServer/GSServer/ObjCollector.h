#pragma once
#include "GameObject.h"

class CObjCollector :  public CGameObject
{
public:
	CObjCollector(OBJECT_ID id);
	virtual ~CObjCollector();

public:
	virtual void SetPosition(const XMFLOAT3& pos);
	  
	virtual void SetVelocity(OBJ_DIRECTION direction); 

private:
	void InitAsBridge();
	void InitAsSign();
	void InitAsDoorWall();

private:
	vector<CGameObject*> m_Objects;
};

