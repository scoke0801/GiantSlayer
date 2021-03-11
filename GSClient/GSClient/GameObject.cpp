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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

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
}

CBox::~CBox()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//CAnimationCurve::CAnimationCurve(int nKeys)
//{
//	m_nKeys = nKeys;
//	m_pfKeyTimes = new float[nKeys];
//	m_pfKeyValues = new float[nKeys];
//}
//
//CAnimationCurve::~CAnimationCurve()
//{
//	if (m_pfKeyTimes) delete[] m_pfKeyTimes;
//	if (m_pfKeyValues) delete[] m_pfKeyValues;
//}
//
//float CAnimationCurve::GetValueByLinearInterpolation(float fPosition)
//{
//	for (int k = 0; k < (m_nKeys - 1); k++)
//	{
//		if ((m_pfKeyTimes[k] <= fPosition) && (fPosition < m_pfKeyTimes[k + 1]))
//		{
//			float t = (fPosition - m_pfKeyTimes[k]) / (m_pfKeyTimes[k + 1] - m_pfKeyTimes[k]);
//			return(m_pfKeyValues[k] * (1.0f - t) + m_pfKeyValues[k + 1] * t);
//		}
//	}
//	return(m_pfKeyValues[m_nKeys - 1]);
//}
//
//CAnimationLayer::CAnimationLayer()
//{
//}
//
//CAnimationLayer::~CAnimationLayer()
//{
//	for (int i = 0; i < m_nAnimatedBoneFrames; i++)
//	{
//		for (int j = 0; j < 9; j++)
//		{
//			if (m_ppAnimationCurves[i][j]) delete m_ppAnimationCurves[i][j];
//		}
//	}
//	if (m_ppAnimationCurves) delete[] m_ppAnimationCurves;
//
//	if (m_ppAnimatedBoneFrameCaches) delete[] m_ppAnimatedBoneFrameCaches;
//}
//
//void CAnimationLayer::LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE* pInFile)
//{
//	int nAnimationKeys = ::ReadIntegerFromFile(pInFile);
//
//	m_ppAnimationCurves[nBoneFrame][nCurve] = new CAnimationCurve(nAnimationKeys);
//
//	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyTimes, sizeof(float), nAnimationKeys, pInFile);
//	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyValues, sizeof(float), nAnimationKeys, pInFile);
//}
//
//XMFLOAT4X4 CAnimationLayer::GetSRT(int nBoneFrame, float fPosition, float fTrackWeight)
//{
//	CGameObject* pBoneFrame = m_ppAnimatedBoneFrameCaches[nBoneFrame];
//	XMFLOAT3 xmf3S = pBoneFrame->m_xmf3Scale;
//	XMFLOAT3 xmf3R = pBoneFrame->m_xmf3Rotation;
//	XMFLOAT3 xmf3T = pBoneFrame->m_xmf3Translation;
//
//	if (m_ppAnimationCurves[nBoneFrame][0]) xmf3T.x = m_ppAnimationCurves[nBoneFrame][0]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][1]) xmf3T.y = m_ppAnimationCurves[nBoneFrame][1]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][2]) xmf3T.z = m_ppAnimationCurves[nBoneFrame][2]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][3]) xmf3R.x = m_ppAnimationCurves[nBoneFrame][3]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][4]) xmf3R.y = m_ppAnimationCurves[nBoneFrame][4]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][5]) xmf3R.z = m_ppAnimationCurves[nBoneFrame][5]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][6]) xmf3S.x = m_ppAnimationCurves[nBoneFrame][6]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][7]) xmf3S.y = m_ppAnimationCurves[nBoneFrame][7]->GetValueByLinearInterpolation(fPosition);
//	if (m_ppAnimationCurves[nBoneFrame][8]) xmf3S.z = m_ppAnimationCurves[nBoneFrame][8]->GetValueByLinearInterpolation(fPosition);
//
//	float fWeight = m_fWeight * fTrackWeight;
//	XMMATRIX S = XMMatrixScaling(xmf3S.x * fWeight, xmf3S.y * fWeight, xmf3S.z * fWeight);
//	//	XMMATRIX R = XMMatrixRotationRollPitchYaw(xmf3R.x * fWeight, xmf3R.y * fWeight, xmf3R.z * fWeight);
//	XMMATRIX R = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(xmf3R.x * fWeight), XMMatrixRotationY(xmf3R.y * fWeight)), XMMatrixRotationZ(xmf3R.z * fWeight));
//	XMMATRIX T = XMMatrixTranslation(xmf3T.x * fWeight, xmf3T.y * fWeight, xmf3T.z * fWeight);
//
//	XMFLOAT4X4 xmf4x4Transform;
//	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixMultiply(XMMatrixMultiply(S, R), T));
//
//	return(xmf4x4Transform);
//}
//
//CAnimationSet::CAnimationSet(float fStartTime, float fEndTime, char* pstrName)
//{
//	m_fStartTime = fStartTime;
//	m_fEndTime = fEndTime;
//	m_fLength = fEndTime - fStartTime;
//
//	strcpy_s(m_pstrAnimationSetName, 64, pstrName);
//}
//
//CAnimationSet::~CAnimationSet()
//{
//	if (m_pAnimationLayers) delete[] m_pAnimationLayers;
//
//	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
//	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
//}
//
//void* CAnimationSet::GetCallbackData()
//{
//	for (int i = 0; i < m_nCallbackKeys; i++)
//	{
//		if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON)) return(m_pCallbackKeys[i].m_pCallbackData);
//	}
//	return(NULL);
//}
//
//void CAnimationSet::SetPosition(float fTrackPosition)
//{
//	m_fPosition = fTrackPosition;
//	switch (m_nType)
//	{
//	case ANIMATION_TYPE_LOOP:
//	{
//		m_fPosition = m_fStartTime + ::fmod(fTrackPosition, m_fLength);
//		break;
//	}
//	case ANIMATION_TYPE_ONCE:
//		break;
//	case ANIMATION_TYPE_PINGPONG:
//		break;
//	}
//
//	if (m_pAnimationCallbackHandler)
//	{
//		void* pCallbackData = GetCallbackData();
//		if (pCallbackData) m_pAnimationCallbackHandler->HandleCallback(pCallbackData);
//	}
//}
//
//void CAnimationSet::Animate(float fTrackPosition, float fTrackWeight)
//{
//	SetPosition(fTrackPosition);
//
//	//for (int i = 0; i < m_nAnimationLayers; i++)
//	//{
//	//	for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++)
//	//	{
//	//		m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Zero();
//	//	}
//	//}
//
//	for (int i = 0; i < m_nAnimationLayers; i++)
//	{
//		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++)
//		{
//			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = m_pAnimationLayers[i].GetSRT(j, m_fPosition, fTrackWeight);
//			//			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Add(xmf4x4Transform, m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent);
//		}
//	}
//}
//
//void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
//{
//	m_nCallbackKeys = nCallbackKeys;
//	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
//}
//
//void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void* pData)
//{
//	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
//	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
//}
//
//void CAnimationSet::SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler)
//{
//	m_pAnimationCallbackHandler = pCallbackHandler;
//}
//
//CAnimationSets::CAnimationSets(int nAnimationSets)
//{
//	m_nAnimationSets = nAnimationSets;
//	m_ppAnimationSets = new CAnimationSet * [nAnimationSets];
//	for (int i = 0; i < m_nAnimationSets; i++) m_ppAnimationSets[i] = NULL;
//}
//
//CAnimationSets::~CAnimationSets()
//{
//	for (int i = 0; i < m_nAnimationSets; i++) if (m_ppAnimationSets[i]) delete m_ppAnimationSets[i];
//	if (m_ppAnimationSets) delete[] m_ppAnimationSets;
//}
//
//void CAnimationSets::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
//{
//	m_ppAnimationSets[nAnimationSet]->m_nCallbackKeys = nCallbackKeys;
//	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
//}
//
//void CAnimationSets::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void* pData)
//{
//	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
//	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
//}
//
//void CAnimationSets::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler)
//{
//	m_ppAnimationSets[nAnimationSet]->SetAnimationCallbackHandler(pCallbackHandler);
//}

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

	cout << "애니메이션 스택 사이즈: " << m_nAnimationStacks << endl;

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
	m_pfbxCurrentTimes[nAnimationStack].SetSecondDouble(fPosition);
}

void CAnimationController::AdvanceTime(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;

	FbxTime fbxElapsedTime;
	fbxElapsedTime.SetSecondDouble(fTimeElapsed);

	m_pfbxCurrentTimes[m_nAnimationStack] += fbxElapsedTime;
	if (m_pfbxCurrentTimes[m_nAnimationStack] > m_pfbxStopTimes[m_nAnimationStack]) m_pfbxCurrentTimes[m_nAnimationStack] = m_pfbxStartTimes[m_nAnimationStack];
}

CFbxObject::CFbxObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CFbxObject::CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	FbxScene* m_pfbxScene = FbxScene::Create(pfbxSdkManager, "");
	m_pfbxScene = LoadFbxSceneFromFile(pd3dDevice, pd3dCommandList, pfbxSdkManager, pstrFbxFileName);

	CMeshFbx* pFbxtestMesh = new CMeshFbx(pd3dDevice, pd3dCommandList, pfbxSdkManager, pstrFbxFileName);

	SetMesh(pFbxtestMesh);

	m_pAnimationController = new CAnimationController(m_pfbxScene);

	FbxLoader(pfbxSdkManager, pstrFbxFileName);
}

CFbxObject::~CFbxObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
	if (m_pAnimationController) delete m_pAnimationController;
}

void CFbxObject::Animate(float fTimeElapsed)
{
	if (m_pAnimationController)
	{
		m_pAnimationController->AdvanceTime(fTimeElapsed);
		FbxTime fbxCurrentTime = m_pAnimationController->GetCurrentTime();
		//::AnimateFbxNodeHierarchy(m_pfbxScene->GetRootNode(), fbxCurrentTime);
	}
}

void CFbxObject::TestAnimate(float fTimeElapsed)
{
	
}

void CFbxObject::Update(float fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	static float Friction = 50.0f;

	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = MaxVelocityXZ * fTimeElapsed;
	if (fLength > MaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = MaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > MaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity);

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

	Animate(fTimeElapsed);
}

void CFbxObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
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
	
	CTexturedRectMesh* pSkyBoxMesh_Front = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Back = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Left = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Right = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, 10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Top = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f, +10.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Bottom = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f, -10.0f, +0.0f);

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