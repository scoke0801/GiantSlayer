#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer()
{
	m_pCamera = NULL;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CPlayer::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}
void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. 
		//‘↓’를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_FORWARD)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다.
		//‘←’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_RIGHT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		//‘Page Up’을 누르면 로컬 y-축 방향으로 이동한다.
		//‘Page Down’을 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_UP)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN)
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다.
		Move(xmf3Shift, bUpdateVelocity);
	}
}
void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	//bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다. 
	if (bUpdateVelocity)
	{
		//플레이어의 속도 벡터를 xmf3Shift 벡터만큼 변경한다.
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		//플레이어의 위치가 변경되었으므로 카메라의 위치도 xmf3Shift 벡터만큼 이동한다. 
		//if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}
}
void CPlayer::Move(float fxOffset, float fyOffset, float fzOffset)
{
}

void CPlayer::Rotate(float x, float y, float z)
{
	
}

void CPlayer::Update(float fTimeElapsed)
{
	/*플레이어의 속도 벡터를 중력 벡터와 더한다.
	중력 벡터에 fTimeElapsed를 곱하는 것은 중력을 시간에 비례하도록
	적용한다는 의미이다.*/
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity,
		Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	/*플레이어의 속도 벡터의 XZ-성분의 크기를 구한다.
	이것이 XZ-평면의 최대 속력보다 크면 속도 벡터의 x와 z-방향
	성분을 조정한다.*/
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x +
		m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}

	/*플레이어의 속도 벡터의 y-성분의 크기를 구한다. 이것이 y-축 방향의 최대 속력보다 크면 속도 벡터의 y-방향 성
	분을 조정한다.*/
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라도 이동될 것이다).
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);

	/*플레이어의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 플레이어의 새로운 위치가 유효한 위치가 아닐 수도
	있고 또는 플레이어의 충돌 검사 등을 수행할 필요가 있다. 이러한 상황에서 플레이어의 위치를 유효한 위치로 다시
	변경할 수 있다.*/
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	//DWORD nCameraMode = m_pCamera->GetMode();

	//플레이어의 위치가 변경되었으므로 3인칭 카메라를 갱신한다. 
	//if (nCameraMode == THIRD_PERSON_CAMERA)
	//	m_pCamera->Update(m_xmf3Position, fTimeElapsed);

	//카메라의 위치가 변경될 때 추가로 수행할 작업을 수행한다.
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다.
	//if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	//카메라의 카메라 변환 행렬을 다시 생성한다. 
	//m_pCamera->RegenerateViewMatrix();

	/*플레이어의 속도 벡터가 마찰력 때문에 감속이 되어야 한다면 감속 벡터를 생성한다. 속도 벡터의 반대 방향 벡터를
	구하고 단위 벡터로 만든다. 마찰 계수를 시간에 비례하도록 하여 마찰력을 구한다. 단위 벡터에 마찰력을 곱하여 감
	속 벡터를 구한다. 속도 벡터에 감속 벡터를 더하여 속도 벡터를 줄인다. 마찰력이 속력보다 크면 속력은 0이 될 것이
	다.*/
	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity,
		Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	return(m_pCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x;
	m_xmf4x4World._12 = m_xmf3Right.y;
	m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x;
	m_xmf4x4World._22 = m_xmf3Up.y;
	m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x;
	m_xmf4x4World._32 = m_xmf3Look.y;
	m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	CGameObject::Draw(pd3dCommandList, pCamera);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

CCubePlayer::CCubePlayer(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

CCubePlayer::CCubePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int
	nMeshes) : CPlayer()
{
	//m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	//CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	/*플레이어의 위치를 지형의 가운데(y-축 좌표는 지형의 높이보다 1500 높게)로
	설정한다. 플레이어 위치 벡터의 y좌표가 지형의 높이보다 크고 중력이 작용하도록 플레이어를 설정하였으므로 플레이어는 점차적으로 하강하게 된다.*/

	SetPosition(XMFLOAT3(10, 10, 10));

	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		10.0f, 10.0f, 10.0f);
	SetMesh(pCubeMesh);

	//플레이어를 렌더링할 셰이더를 생성한다.
	CPlayerShader* pShader = new CPlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
CCubePlayer::~CCubePlayer()
{
}
CCamera* CCubePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	return(m_pCamera);
}
void CCubePlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	
}
void CCubePlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	
}