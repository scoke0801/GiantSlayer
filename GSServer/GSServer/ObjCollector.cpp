#include "stdafx.h"
#include "ObjCollector.h"

CObjCollector::CObjCollector(OBJECT_ID id)
{ 
	switch (id)
	{ 
	case OBJECT_ID::BRIDEGE_SEC2_SEC3_1: 
	case OBJECT_ID::BRIDEGE_SEC2_SEC3_2: 
	case OBJECT_ID::BRIDEGE_SEC2_SEC3_3:
		break;
	case OBJECT_ID::SIGN_SCROLL: 
	case OBJECT_ID::SIGN_PUZZLE: 
	case OBJECT_ID::SIGN_MEDUSA: 
	case OBJECT_ID::SIGN_BOSS:
		break;
	case OBJECT_ID::DOOR_WALL_SEC1: 
	case OBJECT_ID::DOOR_WALL_SEC2: 
	case OBJECT_ID::DOOR_WALL_SEC3: 
	case OBJECT_ID::DOOR_WALL_SEC4: 
	case OBJECT_ID::DOOR_WALL_SEC5:
		break;
	case OBJECT_ID::MONSTER_MELEE_1: 
	case OBJECT_ID::MONSTER_MELEE_2: 
	case OBJECT_ID::MONSTER_MELEE_3: 
	case OBJECT_ID::MONSTER_MELEE_4: 
	case OBJECT_ID::MONSTER_MELEE_5: 
	case OBJECT_ID::MONSTER_MELEE_6: 
	case OBJECT_ID::MONSTER_MELEE_7: 
	case OBJECT_ID::MONSTER_MELEE_8: 
	case OBJECT_ID::MONSTER_MELEE_9: 
	case OBJECT_ID::MONSTER_MELEE_10: 
		break;
	case OBJECT_ID::MONSTER_RANGE_1: 
	case OBJECT_ID::MONSTER_RANGE_2: 
	case OBJECT_ID::MONSTER_RANGE_3: 
	case OBJECT_ID::MONSTER_RANGE_4: 
	case OBJECT_ID::MONSTER_RANGE_5: 
	case OBJECT_ID::MONSTER_RANGE_6:  
	case OBJECT_ID::MONSTER_RANGE_7: 
	case OBJECT_ID::MONSTER_RANGE_8: 
	case OBJECT_ID::MONSTER_RANGE_9: 
	case OBJECT_ID::MONSTER_RANGE_10:
		break;
	case OBJECT_ID::BOSS:
		break;
	default:
		break;
	}
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

void CObjCollector::InitAsBridge()
{
}

void CObjCollector::InitAsSign()
{
}

void CObjCollector::InitAsDoorWall()
{
}
