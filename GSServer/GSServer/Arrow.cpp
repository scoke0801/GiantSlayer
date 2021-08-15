#include "stdafx.h"
#include "Arrow.h" 
#include "GameRoom.h"

void CArrow::Update(float fTimeElapsed)
{
	if (false == m_isUsing) {
		SetPosition(Vector3::Add(m_xmf3Position, Vector3::Multifly(m_xmf3Velocity, TEST_ARROW_SPEED * fTimeElapsed)));
		
		m_ElapsedTime += fTimeElapsed;
		if (m_ElapsedTime > ARROW_LIFE_TIME) {
			m_ElapsedTime = 0.0f;
			m_isDrawable = true;
			int idx;
			if(m_IsPlayerArrow){
				idx = (int)OBJECT_LAYER::PlayerArrow;
			}
			else {
				idx = (int)OBJECT_LAYER::MonsterArrow;
			}
			m_ConnectedRoom->RecyleObject(this, idx);
			//m_ConnectedParticle = nullptr;
		}

		// 
		if (m_xmf3Position.y >= m_startPos.y + 50) {
			falldown = true;
		}
		if (!falldown) {
			m_xmf3Position = Vector3::Add(m_xmf3Position, { 0.0f, 3.0f, 0.0f });
			//Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), -10.0f * fTimeElapsed);
		}
		else {
			//Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 20.0f * fTimeElapsed);
		}

		m_xmf3Velocity = Vector3::Subtract(m_xmf3Velocity, Vector3::Multifly(m_xmf3Velocity, AIR_RESISTANCE));
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, { 0.0f, GRAVITY * m_ElapsedTime, 0.0f });
	}
}

void CArrow::SetTargetPosition(const XMFLOAT3& targetPos)
{	
	XMFLOAT3 dirVector = Vector3::Normalize(Vector3::Subtract(targetPos, m_xmf3Position));

	m_xmf3TargetPosition = targetPos;  
	m_xmf3Velocity = dirVector; 
}

void CArrow::SetTargetVector(const XMFLOAT3& playerLookAt)
{
	XMFLOAT3 dirVector = Vector3::Normalize(playerLookAt);
	XMFLOAT3 targetPos = Vector3::Multifly(dirVector, 150000);
	m_xmf3Velocity = dirVector;
	LookAt(m_xmf3Position, targetPos, XMFLOAT3(0, 1, 0));
} 