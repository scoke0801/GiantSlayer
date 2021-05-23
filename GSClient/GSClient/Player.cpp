#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "Terrain.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Type = OBJ_TYPE::Player;

	m_HP = 100;
	m_SP = 100;
}

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	LoadFile(pstrFbxFileName);

	finTransform = new XMFLOAT4X4[100];

	cout << indices.size() << " " << vertices.size() << " " << skeleton.size() << endl;
	 
	SetMesh(new CAnimatedMesh(pd3dDevice, pd3dCommandList, vertices, indices, skeleton));
	//SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	SetShader(CShaderHandler::GetInstance().GetData("FbxAnimated"));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_time = 0;
	t = 0;
	endTime = 0;
	for (int i = 0; i < skeleton.size(); i++) {
		float t = animations[curAnim].bone[i].animFrame.back().frameTime;
		if (t > endTime)
			endTime = t;
	}

	cout << endTime << endl;	
	
	m_Type = OBJ_TYPE::Player;

	m_HP = 100;
	m_SP = 100;
}

CPlayer::~CPlayer()
{

}

void CPlayer::Update(float fTimeElapsed)
{
	if (false == m_IsCanAttack) {
		m_AttackWaitingTime -= fTimeElapsed;
	
		if (m_AttackWaitingTime < 0.0f){
			m_AttackWaitingTime = 0.0f;
			m_IsCanAttack = true;
		}
	}
	
	// 피격
	if (m_AttackedDelay > 0.0f) {
		m_AttackedDelay = max(m_AttackedDelay - fTimeElapsed, 0.0f);
	}

	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 6000.0f;
	float Friction = (m_MovingType == PlayerMoveType::Run) ? 360.0f : 50.0f;

	XMFLOAT3 vel = Vector3::Multifly(m_xmf3Velocity, fTimeElapsed);
	 
	Move(vel);

	if (false == m_isOnGround) {
		float y;
		if (m_JumpTime > 0.5f) {
			y = -PLAYER_JUMP_HEIGHT * fTimeElapsed;
		}
		else {
			y = PLAYER_JUMP_HEIGHT * fTimeElapsed;
		}
		Move({ 0,y,0 });
		m_JumpTime += fTimeElapsed;
		if (m_JumpTime > TO_JUMP_TIME) {
			m_JumpTime = 0.0f;
			m_isOnGround = true;
		}
	}

	UpdateCamera(); 

	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed); 
	if (fDeceleration > fLength) fDeceleration = fLength; 
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	 
	Animate(fTimeElapsed);
}

void CPlayer::UpdateCamera()
{
	if (m_Camera != nullptr) {
		m_Camera->Update(m_xmf3Position);
		m_Camera->LookAt(m_Camera->GetPosition3f(), m_xmf3Position, GetUp());
		m_Camera->UpdateViewMatrix(); 
	}
}

void CPlayer::FixCameraByTerrain(CTerrain* pTerrain)
{
	if (m_Camera == nullptr) {
		return;
	}
	XMFLOAT3 xmf3CameraPosition = m_Camera->GetPosition3f();

	/*
	높이 맵에서 카메라의 현재 위치 (x, z)에 대한 지형의 높이(y 값)를 구한다.
	이 값이 카메라의 위치 벡터의 y-값 보다 크면 카메라가 지형의 아래에 있게 된다. 
	이렇게 되면 다음 그림의 왼쪽과 같이
	지형이 그려지지 않는 경우가 발생한다(카메라가 지형 안에 있으므로 삼각형의 와인딩 순서가 바뀐다).
	이러한 경우가 발생하지 않도록 카메라의 위치 벡터의 y-값의 최소값은 (지형의 높이 + 5)로 설정한다. 
	카메라의 위치 벡터의 y-값의 최소값은 지형의 모든 위치에서
	카메라가 지형 아래에 위치하지 않도록 설정해야 한다.*/

	float offsetHeight = m_Camera->GetOffset().y;
	//float fHeight = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z) + 5.0f;	
	float fHeight = pTerrain->GetDetailHeight(xmf3CameraPosition.x, xmf3CameraPosition.z) + 5.0f;
	if (fHeight - 1500.0f > m_xmf3Position.y) {
		fHeight = m_xmf3Position.y + 1500.0f;
	}
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_Camera->SetPosition(xmf3CameraPosition); 
		m_Camera->LookAt(m_xmf3Position, GetUp());
	}
}

void CPlayer::FixPositionByTerrain(CTerrain* pTerrain)
{
	if (m_isOnGround) {
		m_xmf3Position.y = pTerrain->GetDetailHeight(m_xmf3Position.x, m_xmf3Position.z);
	}
}

void CPlayer::SetVelocity(OBJ_DIRECTION direction)
{ 	
	//XMFLOAT3 look = GetLook();//m_Camera->GetLook3f();
	//XMFLOAT3 right = GetRight();//m_Camera->GetRight3f();
	XMFLOAT3 look = m_Camera->GetLook3f();
	XMFLOAT3 right = m_Camera->GetRight3f();

	look.y = right.y = 0.0f;
	look = Vector3::Normalize(look);
	right = Vector3::Normalize(right);

	XMFLOAT3 xmf3Dir = Vector3::Normalize(m_xmf3Velocity);
	switch (direction)
	{
	case OBJ_DIRECTION::Front:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(look, PLAYER_RUN_VELOCITY));
		xmf3Dir.z += 500;
		break;
	case OBJ_DIRECTION::Back:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(look, -1), PLAYER_RUN_VELOCITY));
		xmf3Dir.z -= 500;
		break;
	case OBJ_DIRECTION::Left:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(right, -1), PLAYER_RUN_VELOCITY));
		xmf3Dir.x -= 500;
		break;
	case OBJ_DIRECTION::Right:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(right, PLAYER_RUN_VELOCITY));
		xmf3Dir.x += 500;
		break;
	default:
		assert("잘못된 방향으로 이동할 수 없어요~");
		break;
	}	
	XMFLOAT3 playerLookAt = Vector3::Normalize(GetLook());
	float angle = Vector3::GetAngle(xmf3Dir, playerLookAt);
	//cout << "각도 : " << angle << "\n"; 
	
	LookAt(m_xmf3Position, xmf3Dir, { 0,1,0 }); 
	float speed = m_MovingType == (PlayerMoveType::Run) ? PLAYER_RUN_VELOCITY : PLAYER_WALK_VELOCITY;
	if (m_xmf3Velocity.x >  speed) m_xmf3Velocity.x =  speed;
	if (m_xmf3Velocity.y >  speed) m_xmf3Velocity.y =  speed;
	if (m_xmf3Velocity.z >  speed) m_xmf3Velocity.z =  speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
} 
void CPlayer::SetVelocity(XMFLOAT3 dir)
{
	dir.y = 0;
	XMFLOAT3 normalizedDir = Vector3::Normalize(dir);  
	XMFLOAT3 targetPosition = Vector3::Multifly(normalizedDir, 150000.0f);
	LookAt(m_xmf3Position, targetPosition, XMFLOAT3{ 0,1,0 }); 
	Rotate({ 0,1,0 }, 180.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, m_xmf3Velocity.y, 0.0f);
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, 
		Vector3::Multifly(normalizedDir, PLAYER_RUN_VELOCITY));

	float speed = m_MovingType == (PlayerMoveType::Run) ? PLAYER_RUN_VELOCITY : PLAYER_WALK_VELOCITY;

	if (m_xmf3Velocity.x > speed) m_xmf3Velocity.x = speed;
	if (m_xmf3Velocity.y > speed) m_xmf3Velocity.y = speed;
	if (m_xmf3Velocity.z > speed) m_xmf3Velocity.z = speed;
	if (m_xmf3Velocity.x < -speed) m_xmf3Velocity.x = -speed;
	if (m_xmf3Velocity.y < -speed) m_xmf3Velocity.y = -speed;
	if (m_xmf3Velocity.z < -speed) m_xmf3Velocity.z = -speed;
}

void CPlayer::Jump()
{
	if (false == m_isOnGround) {
		return;
	}
	// 일반 사람의 점프 높이는 대략 30 ~ 40cm 
	m_isOnGround = false;
	m_JumpTime = 0.0f;
}

bool CPlayer::Attacked(CGameObject* pObject)
{
	if (m_AttackedDelay > 0.0f) {
		return false;
	}
	m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	m_AttackedDelay += 1.5f;
	m_HP -= 5;
	if (m_HP <= 5) {
		m_HP = 0;
	}
	return true;
}
 