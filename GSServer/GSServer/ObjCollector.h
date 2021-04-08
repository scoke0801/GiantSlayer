#pragma once
#include "GameObject.h"

class CObjCollector :  public CGameObject
{
public:
	CObjCollector(OBJ_NAME name);
	virtual ~CObjCollector();

public:
	virtual void SetPosition(const XMFLOAT3& pos);
	  
	virtual void SetVelocity(OBJ_DIRECTION direction);

private:
	vector<CGameObject*> m_Objects;
};

