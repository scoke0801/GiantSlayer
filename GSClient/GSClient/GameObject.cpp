#include "stdafx.h"
#include "GameObject.h"

CGameObject::CGameObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };
}

CGameObject::~CGameObject()
{
}

void CGameObject::Update()
{
	Move();
}

void CGameObject::Draw()
{
	cout << m_xmf3Position.x << "	" << m_xmf3Position.y << endl;
}

void CGameObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3Position = pos;
}

void CGameObject::SetVelocity(XMFLOAT3 pos)
{
	m_xmf3Velocity = pos;
}

void CGameObject::Move()
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity);
}
