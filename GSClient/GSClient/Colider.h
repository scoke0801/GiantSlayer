#pragma once

// BoundingBox, BoundingOrientedBox ,BoundingSphere 
// 객체들을 위한 interface 클래스  
class Colider
{
public:
	Colider() {}
	virtual ~Colider() {}

	virtual BoundingBox& GetBox() = 0;
	virtual BoundingOrientedBox& GetOrientedBox() = 0;
	virtual BoundingSphere& GetSphere() = 0;

protected:
	BoundingBox	m_BoundingBox;
	BoundingOrientedBox	m_BoundingOrientedBox;
	BoundingSphere	m_BoundingSphere;
};

class ColiderBox : public Colider {
public:
	// Center of the box.
	// Distance from the center to each side.
	ColiderBox(XMFLOAT3 center, XMFLOAT3 extents);

public:
	BoundingBox& GetBox() override { return m_BoundingBox; }
	BoundingOrientedBox& GetOrientedBox() override { assert(!"ColiderBox instance only use BoundingBox"); return m_BoundingOrientedBox;}
	BoundingSphere& GetSphere() override { assert(!"ColiderBox instance only use BoundingBox"); return m_BoundingSphere; } 
};
 
class ColiderOriBox : public Colider {
public:
	// Center of the box.
	// Distance from the center to each side.
	// Unit quaternion representing rotation (box -> world).
	ColiderOriBox(XMFLOAT3 center, XMFLOAT3 extents, XMFLOAT4 orientation);
		
public:
	BoundingBox& GetBox() override { assert(!"ColiderOriBox instance only use BoundingBox"); return m_BoundingBox;}
	BoundingOrientedBox& GetOrientedBox() override { return m_BoundingOrientedBox; }
	BoundingSphere& GetSphere() override { assert(!"ColiderOriBox instance only use BoundingBox"); return m_BoundingSphere; }
};

class ColiderSphere : public Colider {
public:    
	// Center of the sphere.
	// Radius of the sphere.
	ColiderSphere(XMFLOAT3 center, float radius);

public:
	BoundingBox& GetBox() override { assert(!"ColiderSphere instance only use BoundingBox"); return m_BoundingBox;}
	BoundingOrientedBox& GetOrientedBox() override { assert(!"ColiderSphere instance only use BoundingBox"); return m_BoundingOrientedBox;}
	BoundingSphere& GetSphere() override { return m_BoundingSphere; }  
};