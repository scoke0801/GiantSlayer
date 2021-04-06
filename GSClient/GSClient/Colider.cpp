#include "stdafx.h"
#include "Colider.h"

ColiderBox::ColiderBox(const XMFLOAT3& center, const XMFLOAT3& extents)
{
	m_BoundingBox.Center = center;
	m_BoundingBox.Extents = extents;
	m_Type = ColliderType::Box;
}

void ColiderBox::Update(const XMFLOAT3& pos)
{
	m_BoundingBox.Center.x = pos.x;
	m_BoundingBox.Center.y = pos.y;
	m_BoundingBox.Center.z = pos.z;
}

ColiderOriBox::ColiderOriBox(const XMFLOAT3& center, const XMFLOAT3& extents, const XMFLOAT4& orientation)
{
	m_BoundingOrientedBox.Center = center;
	m_BoundingOrientedBox.Extents = extents;
	m_BoundingOrientedBox.Orientation = orientation;
	m_Type = ColliderType::OrientedBox;
}

void ColiderOriBox::Update(const XMFLOAT3& pos)
{
	m_BoundingOrientedBox.Center.x = pos.x;
	m_BoundingOrientedBox.Center.y = pos.y;
	m_BoundingOrientedBox.Center.z = pos.z;
}

ColiderSphere::ColiderSphere(const XMFLOAT3& center, float radius)
{
	m_BoundingSphere.Center = center;
	m_BoundingSphere.Radius = radius;
	m_Type = ColliderType::Sphere;
}

void ColiderSphere::Update(const XMFLOAT3& pos)
{
	m_BoundingSphere.Center.x = pos.x;
	m_BoundingSphere.Center.y = pos.y;
	m_BoundingSphere.Center.z = pos.z;
}
