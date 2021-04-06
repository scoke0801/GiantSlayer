#include "stdafx.h"
#include "Colider.h"

ColiderBox::ColiderBox(XMFLOAT3 center, XMFLOAT3 extents)
{
	m_BoundingBox.Center = center;
	m_BoundingBox.Extents = extents;
}

ColiderOriBox::ColiderOriBox(XMFLOAT3 center, XMFLOAT3 extents, XMFLOAT4 orientation)
{
	m_BoundingOrientedBox.Center = center;
	m_BoundingOrientedBox.Extents = extents;
	m_BoundingOrientedBox.Orientation = orientation;
}

ColiderSphere::ColiderSphere(XMFLOAT3 center, float radius)
{
	m_BoundingSphere.Center = center;
	m_BoundingSphere.Radius = radius;
}
