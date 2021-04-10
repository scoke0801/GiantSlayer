#pragma once

enum class ColliderType : short { None, Box, OrientedBox, Sphere };
// BoundingBox, BoundingOrientedBox ,BoundingSphere 
// 객체들을 위한 interface 클래스   

class Collider
{
public:
	Collider() {}
	virtual ~Collider() {}

	virtual BoundingBox& GetBox() = 0;
	virtual BoundingOrientedBox& GetOrientedBox() = 0;
	virtual BoundingSphere& GetSphere() = 0;

	virtual void Update(const XMFLOAT3& pos) = 0;
	virtual void Update(const XMFLOAT4X4& world) = 0;

	ColliderType GetType() const { return m_Type; }

protected:
	BoundingBox			m_BoundingBox;
	BoundingOrientedBox	m_BoundingOrientedBox;
	BoundingSphere		m_BoundingSphere; 
	
	ColliderType	m_Type;
};

class ColliderBox : public Collider {
public:
	// Center of the box.
	// Distance from the center to each side.
	ColliderBox(const XMFLOAT3& center, const XMFLOAT3& extents);

public:
	BoundingBox& GetBox() override { return m_BoundingBox; }
	BoundingOrientedBox& GetOrientedBox() override { assert(!"ColiderBox instance only use BoundingBox"); return m_BoundingOrientedBox;}
	BoundingSphere& GetSphere() override { assert(!"ColiderBox instance only use BoundingBox"); return m_BoundingSphere; } 

	void Update(const XMFLOAT3& pos) override; 
	void Update(const XMFLOAT4X4& world) override;
};
 
class ColliderOriBox : public Collider {
public:
	// Center of the box.
	// Distance from the center to each side.
	// Unit quaternion representing rotation (box -> world).
	ColliderOriBox(const XMFLOAT3& center, const XMFLOAT3& extents, const XMFLOAT4& orientation);
		
public:
	BoundingBox& GetBox() override { assert(!"ColiderOriBox instance only use BoundingBox"); return m_BoundingBox;}
	BoundingOrientedBox& GetOrientedBox() override { return m_BoundingOrientedBox; }
	BoundingSphere& GetSphere() override { assert(!"ColiderOriBox instance only use BoundingBox"); return m_BoundingSphere; }

	void Update(const XMFLOAT3& pos) override; 
	void Update(const XMFLOAT4X4& world) override;
};

class ColliderSphere : public Collider {
public:    
	// Center of the sphere.
	// Radius of the sphere.
	ColliderSphere(const XMFLOAT3& center, float radius);
	
public:
	BoundingBox& GetBox() override { assert(!"ColiderSphere instance only use BoundingBox"); return m_BoundingBox;}
	BoundingOrientedBox& GetOrientedBox() override { assert(!"ColiderSphere instance only use BoundingBox"); return m_BoundingOrientedBox;}
	BoundingSphere& GetSphere() override { return m_BoundingSphere; }  

	void Update(const XMFLOAT3& pos) override;
	void Update(const XMFLOAT4X4& world) override;
};