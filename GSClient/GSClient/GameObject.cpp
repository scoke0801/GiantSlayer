#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

string ConvertToObjectName(const OBJ_NAME& name)
{
	switch (name)
	{
	case OBJ_NAME::None:
		return "None";
	case OBJ_NAME::Box:
		return "Box";
	case OBJ_NAME::Terrain:
		return "Terrain";
	default:
		assert(!"UnDefinedObjectName");
	}
}

CGameObject::CGameObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();

	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();

}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();

	m_pMesh = pMesh;

	if (m_pMesh) m_pMesh->AddRef();
} 
void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, 
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObjInfo);
}
void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{ 
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World))); 
	::memcpy(&m_pcbMappedGameObjInfo->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4)); 
	::memcpy(&m_pcbMappedGameObjInfo->m_Material, m_Material, sizeof(MATERIAL));
	::memcpy(&m_pcbMappedGameObjInfo->m_nTextureIndex, &m_nTextureIndex, sizeof(UINT)); 
} 
void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{

}

void CGameObject::Update(double fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	static float Friction = 50.0f;	

	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ =  MaxVelocityXZ * fTimeElapsed;
	if (fLength > MaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY =  MaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > MaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity); 

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = ( Friction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true)); 
}

void CGameObject::OnPrepareRender()
{

}

void CGameObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);

}

void CGameObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3Position = pos;

	m_xmf4x4World._41 = pos.x;
	m_xmf4x4World._42 = pos.y;
	m_xmf4x4World._43 = pos.z;
}

void CGameObject::SetVelocity(XMFLOAT3 vel)
{
	m_xmf3Velocity = vel;
}

void CGameObject::SetVelocity(OBJ_DIRECTION direction)
{
	XMFLOAT3 look = GetLook();
	XMFLOAT3 right = GetRight(); 
	
	switch (direction)
	{
	case OBJ_DIRECTION::Front: 
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(look, PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Back:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(look, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Left:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(Vector3::Multifly(right, -1), PLAYER_RUN_VELOCITY));
		break;
	case OBJ_DIRECTION::Right:
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multifly(right, PLAYER_RUN_VELOCITY));
		break;
	default:
		assert("잘못된 방향으로 이동할 수 없어요~");
		break;
	} 
	
}

void CGameObject::SetBoundingBox(XMFLOAT3 center, XMFLOAT3 extents)
{	
	
}
  
XMFLOAT3 CGameObject::GetRight()const
{
	return XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
}


XMFLOAT3 CGameObject::GetUp()const
{
	return XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
}

XMFLOAT3 CGameObject::GetLook()const
{
	return XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
}

void CGameObject::Move(XMFLOAT3 shift)
{
	SetPosition(Vector3::Add(m_xmf3Position, shift));
}

void CGameObject::Move()
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity);
}

void CGameObject::Rotate(XMFLOAT3 pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Scale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World); 
}

//void CGameObject::Rotate(float x, float y, float z)
//{
//	static float pitch, yaw, roll;
//	if (x != 0.0f)
//	{
//		pitch += x;
//		if (pitch > +89.0f) { x -= (pitch - 89.0f); pitch = +89.0f; }
//		if (pitch < -89.0f) { x -= (pitch + 89.0f); pitch = -89.0f; }
//	}
//	if (y != 0.0f)
//	{
//		yaw += y;
//		if (yaw > 360.0f) yaw -= 360.0f;
//		if (yaw < 0.0f) yaw += 360.0f;
//	}
//	if (z != 0.0f)
//	{
//		roll += z;
//		if (roll > +20.0f) { z -= (roll - 20.0f); roll = +20.0f; }
//		if (roll < -20.0f) { z -= (roll + 20.0f); roll = -20.0f; }
//	}
//	//m_pCamera->Rotate(x, y, z);
//	if (y != 0.0f)
//	{
//		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
//		XMFLOAT3 xmf3Look = Vector3::TransformNormal(GetLook(), xmmtxRotate);
//		XMFLOAT3 xmf3Right = Vector3::TransformNormal(GetRight(), xmmtxRotate);
//	}
//	XMFLOAT3 xmf3Look = GetLook();
//	XMFLOAT3 xmf3Right = GetRight();
//	XMFLOAT3 xmf3Up = GetUp();
//	m_xmf4x4World._11 = xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = xmf3Right.z;
//	m_xmf4x4World._21 = xmf3Up.x;    m_xmf4x4World._22 = xmf3Up.y;	  m_xmf4x4World._23 = xmf3Up.z;
//	m_xmf4x4World._31 = xmf3Look.x;  m_xmf4x4World._32 = xmf3Look.y;  m_xmf4x4World._33 = xmf3Look.z;
//}

CRotatingObject::CRotatingObject()
{
	m_Name = OBJ_NAME::None;
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{

}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}
  
CBox::CBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float width, float height, float depth)
{
	
	m_Name = OBJ_NAME::Box;
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		width, height, depth);

	SetMesh(pCubeMeshTex);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CBox::~CBox()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CAnimationController::CAnimationController(FbxScene* pfbxScene)
{
	FbxArray<FbxString*> fbxAnimationStackNames;
	pfbxScene->FillAnimStackNameArray(fbxAnimationStackNames);

	m_nAnimationStacks = fbxAnimationStackNames.Size();

	m_ppfbxAnimationStacks = new FbxAnimStack * [m_nAnimationStacks];
	m_pfbxStartTimes = new FbxTime[m_nAnimationStacks];
	m_pfbxStopTimes = new FbxTime[m_nAnimationStacks];
	m_pfbxCurrentTimes = new FbxTime[m_nAnimationStacks];

	for (int i = 0; i < m_nAnimationStacks; i++)
	{
		FbxString* pfbxStackName = fbxAnimationStackNames[i];
		FbxAnimStack* pfbxAnimationStack = pfbxScene->FindMember<FbxAnimStack>(pfbxStackName->Buffer());
		m_ppfbxAnimationStacks[i] = pfbxAnimationStack;

		FbxTakeInfo* pfbxTakeInfo = pfbxScene->GetTakeInfo(*pfbxStackName);
		FbxTime fbxStartTime, fbxStopTime;
		if (pfbxTakeInfo)
		{
			fbxStartTime = pfbxTakeInfo->mLocalTimeSpan.GetStart();
			fbxStopTime = pfbxTakeInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			FbxTimeSpan fbxTimeLineTimeSpan;
			pfbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(fbxTimeLineTimeSpan);
			fbxStartTime = fbxTimeLineTimeSpan.GetStart();
			fbxStopTime = fbxTimeLineTimeSpan.GetStop();
		}

		m_pfbxStartTimes[i] = fbxStartTime;
		m_pfbxStopTimes[i] = fbxStopTime;
		m_pfbxCurrentTimes[i] = FbxTime(0);
	}

	FbxArrayDelete(fbxAnimationStackNames);
}

CAnimationController::~CAnimationController()
{
	if (m_ppfbxAnimationStacks) delete[] m_ppfbxAnimationStacks;
	if (m_pfbxStartTimes) delete[] m_pfbxStartTimes;
	if (m_pfbxStopTimes) delete[] m_pfbxStopTimes;
	if (m_pfbxCurrentTimes) delete[] m_pfbxCurrentTimes;
}

void CAnimationController::SetAnimationStack(FbxScene* pfbxScene, int nAnimationStack)
{
	m_nAnimationStack = nAnimationStack;
	pfbxScene->SetCurrentAnimationStack(m_ppfbxAnimationStacks[nAnimationStack]);
}

void CAnimationController::SetPosition(int nAnimationStack, float fPosition)
{
	m_pfbxCurrentTimes[nAnimationStack].SetSecondDouble(fPosition);;
}

void CAnimationController::AdvanceTime(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;

	FbxTime fbxElapsedTime;
	fbxElapsedTime.SetSecondDouble(fTimeElapsed);

	m_pfbxCurrentTimes[m_nAnimationStack] += fbxElapsedTime;
	if (m_pfbxCurrentTimes[m_nAnimationStack] > m_pfbxStopTimes[m_nAnimationStack]) m_pfbxCurrentTimes[m_nAnimationStack] = m_pfbxStartTimes[m_nAnimationStack];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CFbxObject::CFbxObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
}

CFbxObject::CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, FbxScene* pfbxScene, char* pstrFbxFileName)
{
	m_pfbxScene = pfbxScene;
	if (!m_pfbxScene)
	{
		m_pfbxScene = ::LoadFbxSceneFromFile(pd3dDevice, pd3dCommandList, pfbxSdkManager, pstrFbxFileName);
		::CreateMeshFromFbxNodeHierarchy(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pfbxScene->GetRootNode());
	}
	else
	{
		cout << "씬 없음..." << endl;
	}
	m_pAnimationController = new CAnimationController(m_pfbxScene);
}

CFbxObject::~CFbxObject()
{
	if (m_pfbxScene) ::ReleaseMeshFromFbxNodeHierarchy(m_pfbxScene->GetRootNode());
	if (m_pfbxScene) m_pfbxScene->Destroy();

	if (m_pAnimationController) delete m_pAnimationController;
}

void CFbxObject::Update(float fTimeElapsed)
{
	if (m_pAnimationController)
	{
		m_pAnimationController->AdvanceTime(fTimeElapsed);
		FbxTime fbxCurrentTime = m_pAnimationController->GetCurrentTime();
		::AnimateFbxNodeHierarchy(m_pfbxScene->GetRootNode(), fbxCurrentTime);
	}
}

void CFbxObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	FbxAMatrix fbxf4x4World = ::XmFloat4x4MatrixToFbxMatrix(m_xmf4x4World);
	if (m_pfbxScene) ::RenderFbxNodeHierarchy(pd3dCommandList, m_pfbxScene->GetRootNode(), m_pAnimationController->GetCurrentTime(), fbxf4x4World);
}

void CFbxObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CFbxObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, FbxAMatrix* pfbxf4x4World)
{
	XMFLOAT4X4 xmf4x4World = ::FbxMatrixToXmFloat4x4Matrix(pfbxf4x4World);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CFbxObject::ReleaseUploadBuffers()
{
	if (m_pfbxScene) ::ReleaseUploadBufferFromFbxNodeHierarchy(m_pfbxScene->GetRootNode());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	m_nObjects = 6;
	m_ppObjects = new CGameObject * [m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CGameObject* pObject = new CGameObject();

		m_ppObjects[i] = pObject;
	}	
	
	CTexturedRectMesh* pSkyBoxMesh_Front = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,  10.0f, 10.0f,  0.0f,   0.0f,   0.0f,   5.0f);
	CTexturedRectMesh* pSkyBoxMesh_Back = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,   10.0f, 10.0f,  0.0f,   0.0f,   0.0f,  -5.0f);
	CTexturedRectMesh* pSkyBoxMesh_Left = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,    0.0f, 10.0f, 10.0f,  -5.0f,   0.0f,   0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Right = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,   0.0f, 10.0f, 10.0f,   5.0f,   0.0f,   0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Top = new CTexturedRectMesh(pd3dDevice, pd3dCommandList,    10.0f, 00.0f, 10.0f,   0.0f,   5.0f,   0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Bottom = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 10.0f, 00.0f, 10.0f,   0.0f,  -5.0f,   0.0f);

	// 스카이박스
	m_ppObjects[0]->SetMesh(pSkyBoxMesh_Front);
	m_ppObjects[0]->SetTextureIndex(0x02);
	m_ppObjects[0]->SetShader(pShader);

	m_ppObjects[1]->SetMesh(pSkyBoxMesh_Back);
	m_ppObjects[1]->SetTextureIndex(0x04);
	m_ppObjects[1]->SetShader(pShader);

	m_ppObjects[2]->SetMesh(pSkyBoxMesh_Left);
	m_ppObjects[2]->SetTextureIndex(0x08);
	m_ppObjects[2]->SetShader(pShader);

	m_ppObjects[3]->SetMesh(pSkyBoxMesh_Right);
	m_ppObjects[3]->SetTextureIndex(0x10);
	m_ppObjects[3]->SetShader(pShader);

	m_ppObjects[4]->SetMesh(pSkyBoxMesh_Top);
	m_ppObjects[4]->SetTextureIndex(0x20);
	m_ppObjects[4]->SetShader(pShader);

	m_ppObjects[5]->SetMesh(pSkyBoxMesh_Bottom);
	m_ppObjects[5]->SetTextureIndex(0x40);
	m_ppObjects[5]->SetShader(pShader); 
}

CSkyBox::~CSkyBox()
{
}
  
void CSkyBox::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition3f();

	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->SetPosition(xmf3CameraPos);
 
	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Draw(pd3dCommandList, pCamera); 
}

void CSkyBox::Rotate(XMFLOAT3 pxmf3Axis, float fAngle)
{
	//for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[4]->Rotate(pxmf3Axis, fAngle);
}

CSkyBoxSphere::CSkyBoxSphere(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	CShader* pShader,
	float radius, UINT32 sliceCount, UINT32 stackCount)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject * [m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CGameObject* pObject = new CGameObject();

		m_ppObjects[i] = pObject;
	} 

	// 스카이박스  
	CSphereMesh* pSphereMesh = new CSphereMesh(pd3dDevice, pd3dCommandList,
		radius, sliceCount, stackCount);
	m_ppObjects[0]->SetMesh(pSphereMesh); 
	m_ppObjects[0]->SetTextureIndex(0x02);
	m_ppObjects[0]->SetShader(pShader);
}

CSkyBoxSphere::~CSkyBoxSphere()
{
}
 
CTerrain::CTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nWidth, int nLength, int nBlockWidth, int nBlockLength,CShader* pShader)
{
	m_nWidth = nWidth;			// 257
	m_nLength = nLength;		// 257

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;	// 32
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;	// 32

	m_nObjects = cxBlocks * czBlocks;

	m_ppObjects = new CGameObject * [m_nObjects];

	for (int i = 0; i < m_nObjects; i++)
	{
		vector<CGameObject> element(m_nObjects);
		m_VectorObjects.push_back(element);
	}
	 
	CTerrainMesh* pTerrainPlaneMesh = new CTerrainMesh(pd3dDevice, pd3dCommandList, 
		0, 0, nBlockWidth, nBlockLength, cxBlocks, czBlocks, MapMeshHeightType::Plane);
	CTerrainMesh* pTerrainUpRidgeMesh = new CTerrainMesh(pd3dDevice, pd3dCommandList,
		0, 0, nBlockWidth, nBlockLength, cxBlocks, czBlocks, MapMeshHeightType::UpRidge);
	CTerrainMesh* pTerrainDownRidgeMesh = new CTerrainMesh(pd3dDevice, pd3dCommandList,
		0, 0, nBlockWidth, nBlockLength, cxBlocks, czBlocks, MapMeshHeightType::DownRidge);

	// 첫 섬 지형
	for (int j = 0; j <18 ; j++)
	{
		for (int i = 0; i < 18; i++)
		{ 
			CGameObject* pObject = new CGameObject();
			pObject->SetTextureIndex(0x01);
			pObject->SetShader(pShader);

			//m_VectorObjects[i][j].SetTextureIndex(0x01);
			//m_VectorObjects[i][j].SetShader(pShader);
			if (j < 3 && i < 3)
			{
				//m_VectorObjects[i][j].SetMesh(pTerrainDownRidgeMesh);
				//m_VectorObjects[i][j].Scale(400.0f, 1.0f, 700.0f);
				//m_VectorObjects[i][j].SetPosition(XMFLOAT3(i * 1600.0f, -100.0f, j * 2800.0f));

				pObject->SetMesh(pTerrainDownRidgeMesh);
				pObject->Scale(400.0f, 1.0f, 700.0f);
				pObject->SetPosition(XMFLOAT3(i * 1600.0f, -100.0f, j * 2800.0f));
			}
			if (j < 6 && i < 6 && j>=3 && i>=3)
			{
				//m_VectorObjects[i][j].SetMesh(pTerrainUpRidgeMesh);
				//m_VectorObjects[i][j].Scale(400.0f, 1.0f, 700.0f);
				//m_VectorObjects[i][j].SetPosition(XMFLOAT3((i-3) * 1600.0f, -500.0f, j * 2800.0f+2500.0f));

				pObject->SetMesh(pTerrainUpRidgeMesh);
				pObject->Scale(400.0f, 1.0f, 700.0f);
				pObject->SetPosition(XMFLOAT3((i - 3) * 1600.0f, -500.0f, j * 2800.0f + 2500.0f));
			}

			/*if (j < 9 && i < 9 && j >= 6 && i >= 6)
			{
				m_VectorObjects[i][j].SetMesh(pTerrainMesh);
				m_VectorObjects[i][j].Scale(400.0f, 1.0f, 700.0f);
				m_VectorObjects[i][j].SetPosition(XMFLOAT3((i - 3) * 1600.0f+1000.0f, -300.0f, (j-6) * 2800.0f));

				pObject->SetMesh(pTerrainMesh);
				pObject->Scale(400.0f, 1.0f, 700.0f);
				pObject->SetPosition(XMFLOAT3((i - 3) * 1600.0f+1000.0f, -300.0f, (j-6) * 2800.0f));
			}
			if (j < 12 && i < 12 && j >= 9 && i >= 9)
			{
				m_VectorObjects[i][j].SetMesh(pTerrainMesh);
				m_VectorObjects[i][j].Scale(400.0f, 1.0f, 700.0f);
				m_VectorObjects[i][j].SetPosition(XMFLOAT3((i - 6) * 1600.0f + 1000.0f, -300.0f, (j - 6) * 2800.0f+2500.0f));

				pObject->SetMesh(pTerrainMesh);
				pObject->Scale(400.0f, 1.0f, 700.0f);
				pObject->SetPosition(XMFLOAT3((i - 6) * 1600.0f + 1000.0f, -300.0f, (j - 6) * 2800.0f+2500.0f));
			}
			if (j < 15 && i < 15 && j >= 12 && i >= 12)
			{
				m_VectorObjects[i][j].SetMesh(pTerrainMesh);
				m_VectorObjects[i][j].Scale(400.0f, 1.0f, 500.0f);
				m_VectorObjects[i][j].SetPosition(XMFLOAT3((i - 6) * 1600.0f + 1500.0f, -300.0f, (j - 12) * 2000.0f));

				pObject->SetMesh(pTerrainMesh);
				pObject->Scale(400.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3((i - 6) * 1600.0f + 1500.0f, -300.0f, (j - 12) * 2000.0f));
			}
			if (j < 16 && i < 16 && j >= 15 && i >= 15)
			{
				m_VectorObjects[i][j].SetMesh(pTerrainMesh);
				m_VectorObjects[i][j].Scale(200.0f, 1.0f, 2000.0f);
				m_VectorObjects[i][j].SetPosition(XMFLOAT3((i - 6) * 1600.0f + 1800.0f, -300.0f, (j - 15) * 2000.0f));

				pObject->SetMesh(pTerrainMesh);
				pObject->Scale(200.0f, 1.0f, 2000.0f);
				pObject->SetPosition(XMFLOAT3((i - 6) * 1600.0f + 1800.0f, -300.0f, (j - 15) * 2000.0f));
			}
			if (j < 19 && i < 19 && j >= 16 && i >= 16)
			{
				m_VectorObjects[i][j].SetMesh(pTerrainMesh);
				m_VectorObjects[i][j].Scale(400.0f, 1.0f, 500.0f);
				m_VectorObjects[i][j].SetPosition(XMFLOAT3((i - 7.5) * 1600.0f + 1000.0f, -300.0f, (j - 9) * 2000.0f+1000.0f));

				pObject->SetMesh(pTerrainMesh);
				pObject->Scale(400.0f, 1.0f, 500.0f);
				pObject->SetPosition(XMFLOAT3((i - 7.5) * 1600.0f + 1000.0f, -300.0f, (j - 9) * 2000.0f+1000.0f));
			}*/ 
			m_Objects.push_back(std::move(pObject));
		}
	} 
	CGameObject* pObject = new CGameObject();
	pObject->Scale(5000.0f, 1.0f, 5000.0f);
	pObject->SetTextureIndex(0x01);
	pObject->SetMesh(pTerrainPlaneMesh);
	pObject->SetShader(pShader);
	pObject->SetPosition(XMFLOAT3(0.0f, -5000.0f, 0.0f));
	m_Objects.push_back(std::move(pObject));
	//m_VectorObjects[19][19].Scale(5000.0f, 1.0f, 5000.0f);
	//m_VectorObjects[19][19].SetTextureIndex(0x01);
	//m_VectorObjects[19][19].SetMesh(pTerrainPlaneMesh);
	//m_VectorObjects[19][19].SetShader(pShader);
	//m_VectorObjects[19][19].SetPosition(XMFLOAT3(0.0f, -5000.0f, 0.0f)); 
}

CTerrain::~CTerrain()
{
}

void CTerrain::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto pObj : m_Objects)
	{
		pObj->Draw(pd3dCommandList, pCamera);
	}
	//for (int j = 0; j < 32; ++j)
	//{
	//	for (int i = 0; i < 32; ++i)
	//	{
	//		m_VectorObjects[i][j].Draw(pd3dCommandList, pCamera);
	//	}
	//}
}
