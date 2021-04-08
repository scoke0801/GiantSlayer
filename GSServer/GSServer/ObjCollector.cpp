#include "stdafx.h"
#include "ObjCollector.h"

CObjCollector::CObjCollector(OBJ_NAME name)
{

}

CObjCollector::~CObjCollector()
{
}

void CObjCollector::SetPosition(const XMFLOAT3& pos)
{
	for (int i = 0; i < m_Objects.size(); ++i) {
		m_Objects[i]->SetPosition(pos);
	}
}

void CObjCollector::SetVelocity(OBJ_DIRECTION direction)
{
	for (int i = 0; i < m_Objects.size(); ++i) {
		//m_Objects[i]->SetVelocity();
	}
}
