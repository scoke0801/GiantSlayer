#include "stdafx.h"
#include "Magic.h"

CMagic::CMagic()
{

}

CMagic::~CMagic()
{

}

void CMagic::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

}

void CMagic::Update(float fTimeElapsed)
{

}

void CMagic::SetTarget(const XMFLOAT3& targetPos)
{
	XMFLOAT3 dirVector = Vector3::Normalize(targetPos);
	//XMFLOAT3 targetPos = Vector3::Multifly(dirVector, 150000);
	m_xmf3Velocity = dirVector;
	LookAt(m_xmf3Position, targetPos, XMFLOAT3(0, 1, 0));
}

void CMagic::TrackingTarget(CGameObject target)
{

}
