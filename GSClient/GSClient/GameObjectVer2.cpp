#include "stdafx.h"
#include "GameObjectVer2.h"
#include "Material.h"
#include "Shader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSet::CAnimationSet()
{
}

CAnimationSet::~CAnimationSet()
{
	if (m_pfKeyFrameTransformTimes) delete[] m_pfKeyFrameTransformTimes;
	for (int j = 0; j < m_nKeyFrameTransforms; j++) if (m_ppxmf4x4KeyFrameTransforms[j]) delete[] m_ppxmf4x4KeyFrameTransforms[j];
	if (m_ppxmf4x4KeyFrameTransforms) delete[] m_ppxmf4x4KeyFrameTransforms;

	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
}

float CAnimationSet::GetPosition(float fPosition)
{
	float fGetPosition = fPosition;
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		fGetPosition = fPosition - int(fPosition / m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1]) * m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1];
		//			fGetPosition = fPosition - int(fPosition / m_fLength) * m_fLength;
#ifdef _WITH_ANIMATION_INTERPOLATION			
#else
		m_nCurrentKey++;
		if (m_nCurrentKey >= m_nKeyFrameTransforms) m_nCurrentKey = 0;
#endif
		break;
	}
	case ANIMATION_TYPE_ONCE:
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}
	return(fGetPosition);
}

XMFLOAT4X4 CAnimationSet::GetSRT(int nFrame, float fPosition)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
#ifdef _WITH_ANIMATION_INTERPOLATION
#ifdef _WITH_ANIMATION_SRT
	XMVECTOR S, R, T;
	for (int i = 0; i < (m_nKeyFrameTranslations - 1); i++)
	{
		if ((m_pfKeyFrameTranslationTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameTranslationTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameTranslationTimes[i]) / (m_pfKeyFrameTranslationTimes[i + 1] - m_pfKeyFrameTranslationTimes[i]);
			T = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i + 1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameScales - 1); i++)
	{
		if ((m_pfKeyFrameScaleTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameScaleTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameScaleTimes[i]) / (m_pfKeyFrameScaleTimes[i + 1] - m_pfKeyFrameScaleTimes[i]);
			S = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameScales[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameScales[i + 1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameRotations - 1); i++)
	{
		if ((m_pfKeyFrameRotationTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameRotationTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameRotationTimes[i]) / (m_pfKeyFrameRotationTimes[i + 1] - m_pfKeyFrameRotationTimes[i]);
			R = XMQuaternionSlerp(XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i][nFrame]), XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i + 1][nFrame]), t);
			break;
		}
	}

	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, NULL, R, T));
#else   
	for (int i = 0; i < (m_nKeyFrameTransforms - 1); i++)
	{
		if ((m_pfKeyFrameTransformTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameTransformTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameTransformTimes[i]) / (m_pfKeyFrameTransformTimes[i + 1] - m_pfKeyFrameTransformTimes[i]);
			XMVECTOR S0, R0, T0, S1, R1, T1;
			XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&m_ppxmf4x4KeyFrameTransforms[i][nFrame]));
			XMMatrixDecompose(&S1, &R1, &T1, XMLoadFloat4x4(&m_ppxmf4x4KeyFrameTransforms[i + 1][nFrame]));
			XMVECTOR S = XMVectorLerp(S0, S1, t);
			XMVECTOR T = XMVectorLerp(T0, T1, t);
			XMVECTOR R = XMQuaternionSlerp(R0, R1, t);
			XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
			break;
		}
	}
#endif
#else
	xmf4x4Transform = m_ppxmf4x4KeyFrameTransforms[m_nCurrentKey][nFrame];
#endif
	return(xmf4x4Transform);
}

void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void* pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController(int nAnimationTracks)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new CAnimationTrack[nAnimationTracks];
}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationSets) delete[] m_pAnimationSets;
	if (m_ppAnimationBoneFrameCaches) delete[] m_ppAnimationBoneFrameCaches;
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;
}

void CAnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_pAnimationSets[nAnimationSet].m_nCallbackKeys = nCallbackKeys;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void* pData)
{
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationController::SetAnimationSet(int nAnimationSet)
{
	if (m_pAnimationSets && (nAnimationSet < m_nAnimationSets))
	{
		m_fTime = 0.0f;
		m_pAnimationTracks[nAnimationSet].m_fPosition = 0.0f;
		m_nAnimationSet = nAnimationSet;
		m_pAnimationTracks[m_nAnimationTrack].m_pAnimationSet = &m_pAnimationSets[m_nAnimationSet];
		// 수정
		//m_pAnimationTracks[m_nAnimationTrack].m_pAnimationSet->m_fPosition = 0.0f;
	}
}

void CAnimationController::AdvanceTime(float fTimeElapsed, CAnimationCallbackHandler* pCallbackHandler)
{
	m_fTime += fTimeElapsed;

	if (m_pAnimationSets)
	{
		for (int i = 0; i < m_nAnimationTracks; i++)
		{
			if (m_pAnimationTracks[i].m_bEnable)
			{
				m_pAnimationTracks[i].m_fPosition += (fTimeElapsed * m_pAnimationTracks[i].m_fSpeed);

				CAnimationSet* pAnimationSet = m_pAnimationTracks[i].m_pAnimationSet;
				pAnimationSet->m_fPosition += (fTimeElapsed * pAnimationSet->m_fSpeed);

				if (pCallbackHandler)
				{
					void* pCallbackData = pAnimationSet->GetCallback(pAnimationSet->m_fPosition);
					if (pCallbackData) pCallbackHandler->HandleCallback(pCallbackData);
				}

				float fPositon = pAnimationSet->GetPosition(pAnimationSet->m_fPosition);
				for (int i = 0; i < m_nAnimationBoneFrames; i++)
				{
					m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent = pAnimationSet->GetSRT(i, fPositon);
					//cout << " TransformMatrix : ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._11 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._12 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._13 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._14 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._21 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._22 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._23 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._24 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._31 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._32 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._33 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._34 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._41 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._42 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._43 << " ";
					//cout << m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent._44 << "\n";
				}
			
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObjectVer2::CGameObjectVer2()
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObjectVer2::CGameObjectVer2(int nMaterials)
{
}

CGameObjectVer2::~CGameObjectVer2()
{
}
//
//void CGameObjectVer2::SetShader(CShader* pShader)
//{
//	m_nMaterials = 1;
//	m_ppMaterials = new CMaterial * [m_nMaterials];
//	m_ppMaterials[0] = new CMaterial(0);
//	m_ppMaterials[0]->SetShader(pShader);
//}
//
//void CGameObjectVer2::SetShader(int nMaterial, CShader* pShader)
//{
//	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
//}
//
void CGameObjectVer2::SetMaterial(int nMaterial, CMaterial* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void CGameObjectVer2::SetChild(CGameObjectVer2* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObjectVer2::SetShadertoAll(CShader* pShader)
{
	if (pShader == nullptr) {
		if (isSkinned == true) {
			SetShader(CShaderHandler::GetInstance().GetData("Skinned"));
			//cout << "MESH: SKINNED" << endl;
		}
		else {
			SetShader(CShaderHandler::GetInstance().GetData("Standard"));
			//cout << "MESH: STANDARD" << endl;
		}
	}
	if (m_pSibling) m_pSibling->SetShadertoAll();
	if (m_pChild) m_pChild->SetShadertoAll();
}

void CGameObjectVer2::SetTextureInedxToAll(UINT index)
{
	if (m_nTextureIndex == 0)
	{
		SetTextureIndex(index);
	}
	if (m_pSibling) m_pSibling->SetTextureInedxToAll(index);
	if (m_pChild) m_pChild->SetTextureInedxToAll(index);
}

void CGameObjectVer2::SetPosition(XMFLOAT3 pos)
{
	m_xmf3PrevPosition = m_xmf3Position;
	m_xmf3Position = pos;
	m_xmf4x4ToParent._41 = pos.x;
	m_xmf4x4ToParent._42 = pos.y;
	m_xmf4x4ToParent._43 = pos.z;

	UpdateTransform(NULL);
}

void CGameObjectVer2::Move(XMFLOAT3 shift)
{
	SetPosition(Vector3::Add(m_xmf3Position, shift));
}

void CGameObjectVer2::Scale(float x, float y, float z, bool setSize)
{
	if (setSize)
	{
		m_xmf3Size = { x,y,z };
	}
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObjectVer2::Rotate(XMFLOAT3 pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObjectVer2::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	XMFLOAT3 L = Vector3::Normalize(Vector3::Subtract(target, pos));
	XMFLOAT3 R = Vector3::Normalize(Vector3::CrossProduct(up, L));
	XMFLOAT3 U = Vector3::CrossProduct(L, R);

	float x = -(Vector3::DotProduct(pos, R));
	float y = -(Vector3::DotProduct(pos, U));
	float z = -(Vector3::DotProduct(pos, L));

	m_xmf4x4ToParent(0, 0) = R.x;
	m_xmf4x4ToParent(0, 1) = R.y;
	m_xmf4x4ToParent(0, 2) = R.z;

	m_xmf4x4ToParent(1, 0) = U.x;
	m_xmf4x4ToParent(1, 1) = U.y;
	m_xmf4x4ToParent(1, 2) = U.z;

	m_xmf4x4ToParent(2, 0) = L.x;
	m_xmf4x4ToParent(2, 1) = L.y;
	m_xmf4x4ToParent(2, 2) = L.z;
	Scale(m_xmf3Size.x, m_xmf3Size.y, m_xmf3Size.z, false);
}
 

void CGameObjectVer2::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObjectVer2::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial)
{
}

void CGameObjectVer2::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{  
	XMFLOAT4X4 xmf4x4World;
	int matIndex = 0;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_nTextureIndex, 16);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &matIndex, 17);
}

void CGameObjectVer2::ReleaseShaderVariables()
{
}

void CGameObjectVer2::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObjectVer2::UpdateColliders()
{
	for (int i = 0; i < m_Colliders.size(); ++i) {
		m_Colliders[i]->GetBox().Transform(m_AABB[i]->GetBox(), XMLoadFloat4x4(&m_xmf4x4ToParent));
	}
}

void CGameObjectVer2::Update(float fTimeElapsed)
{
	//if (!m_isDrawbale) return;
	//static float MaxVelocityXZ = 120.0f;
	//static float MaxVelocityY = 120.0f;
	//static float Friction = 50.0f;
	//
	////m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	//float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	//float fMaxVelocityXZ = MaxVelocityXZ * fTimeElapsed;
	//if (fLength > MaxVelocityXZ)
	//{
	//	m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
	//	m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	//}
	//float fMaxVelocityY = MaxVelocityY * fTimeElapsed;
	//fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	//if (fLength > MaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
	//
	//Move(m_xmf3Velocity);
	//
	//fLength = Vector3::Length(m_xmf3Velocity);
	//float fDeceleration = (Friction * fTimeElapsed);
	//if (fDeceleration > fLength) fDeceleration = fLength;
	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	
	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CGameObjectVer2::UpdateOnServer(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CGameObjectVer2::Animate(float fTimeElapsed)
{
	if (pause)
		return;

	if (m_pAnimationController) m_pAnimationController->AdvanceTime(fTimeElapsed, NULL);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void CGameObjectVer2::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_pMesh && IsDrawable())
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World); 

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_pShader)
					{
						//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
						//m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
						m_pShader->Render(pd3dCommandList, pCamera);
					}
					m_pMesh->Render(pd3dCommandList, i);
				}
			}
		}
	}
	if (gbBoundaryOn)
	{
		if (m_pShader)
		{
			m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4ToParent, m_nTextureIndex, 0);
			m_pShader->RenderBoundary(pd3dCommandList, pCamera);
			for (auto pBoundingMesh : m_BoundingObjectMeshes)
			{
				pBoundingMesh->Render(pd3dCommandList);
			}
		}
	}
	if (m_pSibling) m_pSibling->Draw(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Draw(pd3dCommandList, pCamera);
}

void CGameObjectVer2::Draw_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_pMesh)
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_pShader)
					{
						//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
						//m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
						m_pShader->Render_Shadow(pd3dCommandList, pCamera);
					}
					m_pMesh->Render(pd3dCommandList, i);
				}
			}
		}
	} 
	if (m_pSibling) m_pSibling->Draw(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Draw(pd3dCommandList, pCamera);
}

void CGameObjectVer2::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObjectVer2* CGameObjectVer2::FindFrame(char* pstrFrameName)
{
	CGameObjectVer2* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

CTexture* CGameObjectVer2::FindReplicatedTexture(_TCHAR* pstrTextureName)
{
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i])
		{
			for (int j = 0; j < m_ppMaterials[i]->m_nTextures; j++)
			{
				if (m_ppMaterials[i]->m_ppTextures[j])
				{
					if (!_tcsncmp(m_ppMaterials[i]->m_ppstrTextureNames[j], pstrTextureName, _tcslen(pstrTextureName))) return(m_ppMaterials[i]->m_ppTextures[j]);
				}
			}
		}
	}
	CTexture* pTexture = NULL;
	if (m_pSibling) if (pTexture = m_pSibling->FindReplicatedTexture(pstrTextureName)) return(pTexture);
	if (m_pChild) if (pTexture = m_pChild->FindReplicatedTexture(pstrTextureName)) return(pTexture);

	return(NULL);
}

CGameObjectVer2* CGameObjectVer2::GetRootSkinnedGameObject()
{
	if (m_pAnimationController) return(this);

	CGameObjectVer2* pRootSkinnedGameObject = NULL;
	if (m_pSibling) if (pRootSkinnedGameObject = m_pSibling->GetRootSkinnedGameObject()) return(pRootSkinnedGameObject);
	if (m_pChild) if (pRootSkinnedGameObject = m_pChild->GetRootSkinnedGameObject()) return(pRootSkinnedGameObject);

	return(NULL);
}

void CGameObjectVer2::SetAnimationSet(int nAnimationSet)
{
	m_AnimationSet = nAnimationSet;
	if (m_pAnimationController) m_pAnimationController->SetAnimationSet(nAnimationSet);

	if (m_pSibling) m_pSibling->SetAnimationSet(nAnimationSet);
	if (m_pChild) m_pChild->SetAnimationSet(nAnimationSet);
}

void CGameObjectVer2::CacheSkinningBoneFrames(CGameObjectVer2* pRootFrame)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT))
	{
		CSkinnedMesh* pSkinnedMesh = (CSkinnedMesh*)m_pMesh;
		for (int i = 0; i < pSkinnedMesh->m_nSkinningBones; i++)
		{
			pSkinnedMesh->m_ppSkinningBoneFrameCaches[i] = pRootFrame->FindFrame(pSkinnedMesh->m_ppstrSkinningBoneNames[i]);

			//cout << " i " << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._11 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._12 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._13 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._14 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._21 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._22 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._23 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._24 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._31 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._32 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._33 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._34 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._41 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._42 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._43 << " ";
			//cout << pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World._44 << "\n";
#ifdef _WITH_DEBUG_SKINNING_BONE
			TCHAR pstrDebug[256] = { 0 };
			TCHAR pwstrBoneCacheName[64] = { 0 };
			TCHAR pwstrSkinningBoneName[64] = { 0 };
			size_t nConverted = 0;
			mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, pSkinnedMesh->m_ppSkinningBoneFrameCaches[i]->m_pstrFrameName, _TRUNCATE);
			mbstowcs_s(&nConverted, pwstrSkinningBoneName, 64, pSkinnedMesh->m_ppstrSkinningBoneNames[i], _TRUNCATE);
			_stprintf_s(pstrDebug, 256, _T("SkinningBoneFrame:: Cache(%s) Bone(%s)\n"), pwstrBoneCacheName, pwstrSkinningBoneName);
			OutputDebugString(pstrDebug);
#endif
		}
	}
	if (m_pSibling) m_pSibling->CacheSkinningBoneFrames(pRootFrame);
	if (m_pChild) m_pChild->CacheSkinningBoneFrames(pRootFrame);
}

void CGameObjectVer2::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObjectVer2* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials = new CMaterial * [m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	CMaterial* pMaterial = NULL;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = new CMaterial(7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

			if (!pShader)
			{
				UINT nMeshType = GetMeshType();
				if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
				{
					if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
					{
						pMaterial->SetSkinnedAnimationShader();
					}
					else
					{
						pMaterial->SetStandardShader();
					}
				}
			}
			SetMaterial(nMaterial, pMaterial);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			pMaterial->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_ALBEDO_MAP, 3, pMaterial->m_ppstrTextureNames[0], &(pMaterial->m_ppTextures[0]), pParent, pInFile, pShader);
			SetTextureIndexFindByName(pMaterial->FindTextureName(MATERIAL_ALBEDO_MAP));
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_SPECULAR_MAP, 4, pMaterial->m_ppstrTextureNames[1], &(pMaterial->m_ppTextures[1]), pParent, pInFile, pShader);
			SetTextureIndexFindByName(pMaterial->FindTextureName(MATERIAL_SPECULAR_MAP));
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_NORMAL_MAP, 5, pMaterial->m_ppstrTextureNames[2], &(pMaterial->m_ppTextures[2]), pParent, pInFile, pShader);
			SetTextureIndexFindByName(pMaterial->FindTextureName(MATERIAL_NORMAL_MAP));
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_METALLIC_MAP, 6, pMaterial->m_ppstrTextureNames[3], &(pMaterial->m_ppTextures[3]), pParent, pInFile, pShader);
			SetTextureIndexFindByName(pMaterial->FindTextureName(MATERIAL_METALLIC_MAP));
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_EMISSION_MAP, 7, pMaterial->m_ppstrTextureNames[4], &(pMaterial->m_ppTextures[4]), pParent, pInFile, pShader);
			SetTextureIndexFindByName(pMaterial->FindTextureName(MATERIAL_EMISSION_MAP));
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_ALBEDO_MAP, 8, pMaterial->m_ppstrTextureNames[5], &(pMaterial->m_ppTextures[5]), pParent, pInFile, pShader);
			SetTextureIndexFindByName(pMaterial->FindTextureName(MATERIAL_DETAIL_ALBEDO_MAP));
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_NORMAL_MAP, 9, pMaterial->m_ppstrTextureNames[6], &(pMaterial->m_ppTextures[6]), pParent, pInFile, pShader);
			SetTextureIndexFindByName(pMaterial->FindTextureName(MATERIAL_DETAIL_NORMAL_MAP)); 
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
	
	int stop = 3;
}

void CGameObjectVer2::LoadAnimationFromFile(FILE* pInFile)
{
	//system("cls");
	char pstrToken[64] = { '\0' }; 
	BYTE nStrLength = 0;
	UINT nReads = 0;

	bool haveToRead = true;

	for (; ; )
	{
		if (haveToRead == true)
		{
			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		
			nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
			haveToRead = false;
		} 
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nReads = (UINT)::fread(&m_pAnimationController->m_nAnimationSets, sizeof(int), 1, pInFile);

			m_pAnimationController->m_pAnimationSets = new CAnimationSet[m_pAnimationController->m_nAnimationSets];
			cout << "애니메이션 개수: " << m_pAnimationController->m_nAnimationSets << endl;
			haveToRead = true;
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			haveToRead = true;
			nReads = (UINT)::fread(&m_pAnimationController->m_nAnimationBoneFrames, sizeof(int), 1, pInFile);
			m_pAnimationController->m_ppAnimationBoneFrameCaches = new CGameObjectVer2 * [m_pAnimationController->m_nAnimationBoneFrames];

			for (int i = 0; i < m_pAnimationController->m_nAnimationBoneFrames; i++)
			{
				nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
				nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
				pstrToken[nStrLength] = '\0';

				m_pAnimationController->m_ppAnimationBoneFrameCaches[i] = FindFrame(pstrToken);

#ifdef _WITH_DEBUG_SKINNING_BONE
				TCHAR pstrDebug[256] = { 0 };
				TCHAR pwstrAnimationBoneName[64] = { 0 };
				TCHAR pwstrBoneCacheName[64] = { 0 };
				size_t nConverted = 0;
				mbstowcs_s(&nConverted, pwstrAnimationBoneName, 64, pstrToken, _TRUNCATE);
				mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, m_pAnimationSets->m_ppAnimationBoneFrameCaches[i]->m_pstrFrameName, _TRUNCATE);
				_stprintf_s(pstrDebug, 256, _T("AnimationBoneFrame:: Cache(%s) AnimationBone(%s)\n"), pwstrBoneCacheName, pwstrAnimationBoneName);
				OutputDebugString(pstrDebug);
#endif
			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			haveToRead = true;
			int nAnimationSet = 0;
			nReads = (UINT)::fread(&nAnimationSet, sizeof(int), 1, pInFile);
			CAnimationSet* pAnimationSet = &m_pAnimationController->m_pAnimationSets[nAnimationSet];

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pAnimationSet->m_pstrName, sizeof(char), nStrLength, pInFile);
			pAnimationSet->m_pstrName[nStrLength] = '\0';

			nReads = (UINT)::fread(&pAnimationSet->m_fLength, sizeof(float), 1, pInFile);
			nReads = (UINT)::fread(&pAnimationSet->m_nFramesPerSecond, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&pAnimationSet->m_nKeyFrameTransforms, sizeof(int), 1, pInFile);  
			pAnimationSet->m_pfKeyFrameTransformTimes = new float[pAnimationSet->m_nKeyFrameTransforms];
			pAnimationSet->m_ppxmf4x4KeyFrameTransforms = new XMFLOAT4X4 * [pAnimationSet->m_nKeyFrameTransforms];
			for (int i = 0; i < pAnimationSet->m_nKeyFrameTransforms; i++) pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i] = new XMFLOAT4X4[m_pAnimationController->m_nAnimationBoneFrames];

			for (int i = 0; i < pAnimationSet->m_nKeyFrameTransforms; i++)
			{
				nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
				nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
				pstrToken[nStrLength] = '\0';

				//if (nReads == 1) {
				//	haveToRead = true;
				//	pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i] = pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i - 1];
				//	break;
				//}
				if (!strcmp(pstrToken, "<AnimationSet>:"))
				{
					haveToRead = false;
					// 더미 프레임 추가
					pAnimationSet->m_pfKeyFrameTransformTimes[i] = pAnimationSet->m_pfKeyFrameTransformTimes[i - 1] + 
						(pAnimationSet->m_pfKeyFrameTransformTimes[i - 1] - pAnimationSet->m_pfKeyFrameTransformTimes[i - 2]); // 트랜스폼 적용되는 시간
					pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i] = pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i - 1]; // 트랜스폼
					
					

					/*
					2안, 배열 크기를 m_nKeyFrameTransforms - 1 로 아예 재할당하는 방안, 깜박임 문제 있음
					XMFLOAT4X4* tempMatrix = new XMFLOAT4X4[pAnimationSet->m_nKeyFrameTransforms - 1];
					memcpy(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[0], tempMatrix, sizeof(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[0]) - sizeof(XMFLOAT4X4**));

					float* tempTimes = new float[pAnimationSet->m_nKeyFrameTransforms - 1];
					memcpy(pAnimationSet->m_pfKeyFrameTransformTimes, tempTimes, sizeof(pAnimationSet->m_pfKeyFrameTransformTimes) - sizeof(float*));

					pAnimationSet->m_nKeyFrameTransforms -= 1;
					pAnimationSet->m_ppxmf4x4KeyFrameTransforms[0] = tempMatrix;
					//pAnimationSet->m_pfKeyFrameTransformTimes = tempTimes;
					*/

					break;
				}
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKeyFrame = 0;
					nReads = (UINT)::fread(&nKeyFrame, sizeof(int), 1, pInFile); //i

					nReads = (UINT)::fread(&pAnimationSet->m_pfKeyFrameTransformTimes[i], sizeof(float), 1, pInFile);
					
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i], sizeof(float), 16 * m_pAnimationController->m_nAnimationBoneFrames, pInFile);
					
					//cout << " i : " << i << "\n";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_11 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_12 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_13 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_14 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_21 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_22 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_23 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_24 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_31 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_32 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_33 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_34 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_41 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_42 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_43 << " ";
					//cout << pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i]->_44 << "\n";
				}
				
			}
#ifdef _WITH_ANIMATION_SRT
			nReads = (UINT)::fread(&pAnimationSet->m_nKeyFrameTranslations, sizeof(int), 1, pInFile);
			pAnimationSet->m_pfKeyFrameTranslationTimes = new float[pAnimationSet->m_nKeyFrameTranslations];
			pAnimationSet->m_ppxmf3KeyFrameTranslations = new XMFLOAT3 * [pAnimationSet->m_nKeyFrameTranslations];
			for (int i = 0; i < pAnimationSet->m_nKeyFrameTranslations; i++) pAnimationSet->m_ppxmf3KeyFrameTranslations[i] = new XMFLOAT3[m_pAnimationController->m_nAnimationBoneFrames];
#endif
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			haveToRead = true;
			break;
		}
	}
}

CGameObjectVer2* CGameObjectVer2::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObjectVer2* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObjectVer2* pGameObject = NULL;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObjectVer2();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			pGameObject->m_pstrFrameName[nStrLength] = '\0';
			//cout << pGameObject->m_pstrFrameName << endl;
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(float), 16, pInFile);
			//cout << " TransformMatrix : ";
			//cout << pGameObject->m_xmf4x4ToParent._11 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._12 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._13 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._14 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._21 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._22 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._23 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._24 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._31 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._32 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._33 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._34 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._41 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._42 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._43 << " ";
			//cout << pGameObject->m_xmf4x4ToParent._44 << "\n";
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CStandardMesh* pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			CSkinnedMesh* pSkinnedMesh = new CSkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); //<Mesh>:
			pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pSkinnedMesh);
			pGameObject->isSkinned = true;
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObjectVer2* pChild = CGameObjectVer2::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

CGameObjectVer2* CGameObjectVer2::LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader, bool bHasAnimation)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CGameObjectVer2* pGameObject = CGameObjectVer2::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader);

	pGameObject->CacheSkinningBoneFrames(pGameObject);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, "Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObjectVer2::PrintFrameInfo(pGameObject, NULL);
#endif

	if (bHasAnimation)
	{
		pGameObject->m_pAnimationController = new CAnimationController(1);
		pGameObject->LoadAnimationFromFile(pInFile);
		pGameObject->m_pAnimationController->SetAnimationSet(0);
	}

	return(pGameObject);
}

void CGameObjectVer2::PrintFrameInfo(CGameObjectVer2* pGameObject, CGameObjectVer2* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObjectVer2::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObjectVer2::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void CGameObjectVer2::SetTextureIndexFindByName(string fileName)
{
	if (fileName == "clothingSet_01_tex") {
		SetTextureIndex(0x01);
	}
	else if (fileName == "girl_texture_01") {
		SetTextureIndex(0x02);
	}
	else if (fileName == "hair1") {
		SetTextureIndex(0x04);
	}
	else if (fileName == "sword1") {
		SetTextureIndex(0x08);
	}
	else if (fileName == "Body_D") {
		SetTextureIndex(0x10);
	} 
	else if (fileName == "Body_C") {
		SetTextureIndex(0x10);
	}
	else if (fileName == "Body_E") {
		SetTextureIndex(0x10);
	}
	else if (fileName == "Body_N") {
		SetTextureIndex(0x10);
	} 
	else if (fileName == "Skeleton_D") {
		SetTextureIndex(0x20);
	}
	else if (fileName == "DS_skeleton_standard") {
		SetTextureIndex(0x40);
	}
	else if (fileName == "DS_equipment_standard") {
		SetTextureIndex(0x40);
	}
	else if (fileName == "bow_texture") {
		SetTextureIndex(0x80);
	}
	else if (fileName == "Mat01_mummy_A") {
		SetTextureIndex(0x100);
	}
	int stop = 3;
}

void CGameObjectVer2::SetDrawableRecursively(char* name, bool draw)
{
	if (!strcmp(m_pstrFrameName, name)) SetDrawable(draw);

	if (m_pSibling) m_pSibling->SetDrawableRecursively(name, draw);
	if (m_pChild) m_pChild->SetDrawableRecursively(name, draw);
}

void CGameObjectVer2::PrintPartNames()
{
	cout << m_pstrFrameName << endl;

	if (m_pSibling) m_pSibling->PrintPartNames();
	if (m_pChild) m_pChild->PrintPartNames();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
ExportedObject::ExportedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

ExportedObject::~ExportedObject()
{
}

void ExportedObject::OnPrepareAnimate()
{
}

void ExportedObject::Animate(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
}
