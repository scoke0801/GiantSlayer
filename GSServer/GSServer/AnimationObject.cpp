#include "stdafx.h"
#include "AnimationObject.h"
#include "AnimationController.h"

void CAnimationObject::Update(float fTimeElapsed)
{
	//CGameObject::Update(fTimeElapsed);
	CAnimationObject::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CAnimationObject::Animate(float fTimeElapsed)
{
	if (m_pAnimationController) m_pAnimationController->AdvanceTime(fTimeElapsed, NULL);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void CAnimationObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CAnimationObject::SetPosition(XMFLOAT3 pos)
{
	m_xmf3PrevPosition = m_xmf3Position;
	m_xmf3Position = pos;
	m_xmf4x4ToParent._41 = pos.x;
	m_xmf4x4ToParent._42 = pos.y;
	m_xmf4x4ToParent._43 = pos.z;

	UpdateTransform(NULL);
}

void CAnimationObject::Move(XMFLOAT3 shift)
{
	SetPosition(Vector3::Add(m_xmf3Position, shift));
}

void CAnimationObject::Scale(float x, float y, float z, bool setSize)
{
	if (setSize)
	{
		m_xmf3Size = { x,y,z };
	}
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CAnimationObject::Rotate(const XMFLOAT3& pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CAnimationObject::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
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

void CAnimationObject::SetChild(CAnimationObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) {

			SetAABB(pChild->GetAABB());
			pChild->AddRef();
		}
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

void CAnimationObject::UpdateColliders()
{
	for (int i = 0; i < m_BoundingBox.size(); ++i) {
		m_BoundingBox[i].Transform(m_AABB[i], XMLoadFloat4x4(&m_xmf4x4World));
	}
	
	if (m_pSibling) m_pSibling->UpdateColliders();
	if (m_pChild) m_pChild->UpdateColliders();
}

bool CAnimationObject::CollisionCheck(CGameObject* other)
{
	auto otherAABB = other->GetAABB();
	for (int i = 0; i < otherAABB.size(); ++i) {
		bool result = CGameObject::CollisionCheck(otherAABB[i]);
		if (result) return true;
	} 
	return false;
}

void CAnimationObject::GetAABBToBuffer(vector<BoundingBox>& buffer)
{
	for (int i = 0; i < m_AABB.size(); ++i) {
		buffer.push_back(m_AABB[i]);
	}
	if (m_pSibling) m_pSibling->GetAABBToBuffer(buffer);
	if (m_pChild) m_pChild->GetAABBToBuffer(buffer);
}

void CAnimationObject::CollectAABBFromChilds()
{
	if (m_pSibling) m_pSibling->GetAABBToBuffer(m_AABB);
	if (m_pChild) m_pChild->GetAABBToBuffer(m_AABB);
}

CAnimationObject* CAnimationObject::LoadFrameHierarchyFromFile(CAnimationObject* pParent, FILE* pInFile)
{
	static int count = 0;
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CAnimationObject* pGameObject = NULL;

	XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
	XMFLOAT4 xmf4Rotation;
	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CAnimationObject();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			pGameObject->m_pstrFrameName[nStrLength] = '\0';
			//cout << pGameObject->m_pstrFrameName << endl;
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{ 
			XMFLOAT3 center, extent;
			bool hasBoundingBox = CAnimationObject::LoadMeshInfoFromFile(pInFile, center, extent); 
			if (hasBoundingBox) { 
				XMFLOAT3 half = Vector3::Multifly(extent, 0.5f);
				pGameObject->AddBoundingBox({ center, half });
			}
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			XMFLOAT3 center, extent;
			bool hasBoundingBox = CAnimationObject::LoadSkinInfoFromFile(pInFile, center, extent);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); //<Mesh>: 
			hasBoundingBox = CAnimationObject::LoadMeshInfoFromFile(pInFile, center, extent);

			if (hasBoundingBox) {
				XMFLOAT3 half = Vector3::Multifly(extent, 0.5f);
				pGameObject->AddBoundingBox({ center, half });
			}

			//pGameObject->isSkinned = true;
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			LoadMaterialsFromFile(pParent, pInFile); 
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CAnimationObject* pChild = CAnimationObject::LoadFrameHierarchyFromFile(pGameObject, pInFile);
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

CAnimationObject* CAnimationObject::LoadGeometryAndAnimationFromFile(char* pstrFileName, bool bHasAnimation)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CAnimationObject* pGameObject = CAnimationObject::LoadFrameHierarchyFromFile(NULL, pInFile);

	//pGameObject->CacheSkinningBoneFrames(pGameObject);

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

	pGameObject->CollectAABBFromChilds();
	return(pGameObject);
}

bool CAnimationObject::LoadMeshInfoFromFile(FILE* pInFile, XMFLOAT3& center, XMFLOAT3& extents)
{
	UINT nVertices = 0;
	char pstrMeshName[64] = { 0 };
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&nVertices, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrMeshName, sizeof(char), nStrLength, pInFile);
	pstrMeshName[nStrLength] = '\0';
	bool			HasBoundingInfo = false;
	UINT			nType = 0x00;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			HasBoundingInfo = true;
			nReads = (UINT)::fread(&center, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&extents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{ 
				XMFLOAT3* pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile); 
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{ 
				XMFLOAT4* pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{ 
				XMFLOAT2* pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile); 
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{ 
				XMFLOAT2* pxmf2TextureCoords1 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(pxmf2TextureCoords1, sizeof(XMFLOAT2), nTextureCoords, pInFile); 
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{ 
				XMFLOAT3* pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile); 
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{ 
				XMFLOAT3* pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile); 
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				XMFLOAT3* pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(nSubMeshes), sizeof(int), 1, pInFile);
			if (nSubMeshes > 0)
			{
				int* pnSubSetIndices = new int[nSubMeshes];
				UINT** ppnSubSetIndices = new UINT * [nSubMeshes];

				for (int i = 0; i < nSubMeshes; i++)
				{
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
					pstrToken[nStrLength] = '\0';
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile);
						nReads = (UINT)::fread(&(pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (pnSubSetIndices[i] > 0)
						{
							ppnSubSetIndices[i] = new UINT[pnSubSetIndices[i]];
							nReads = (UINT)::fread(ppnSubSetIndices[i], sizeof(UINT), pnSubSetIndices[i], pInFile);
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
	return HasBoundingInfo;
}

bool CAnimationObject::LoadSkinInfoFromFile(FILE* pInFile, XMFLOAT3& center, XMFLOAT3& extents)
{
	bool			HasBoundingInfo = false;
	return HasBoundingInfo;
}

void CAnimationObject::LoadMaterialsFromFile(CAnimationObject* pParent, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	int nMaterials;
	UINT nReads = (UINT)::fread(&nMaterials, sizeof(int), 1, pInFile);
	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			XMFLOAT4 xmf4AlbedoColor;
			nReads = (UINT)::fread(&(xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			XMFLOAT4 xmf4EmissiveColor;
			nReads = (UINT)::fread(&(xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			XMFLOAT4 xmf4SpecularColor;
			nReads = (UINT)::fread(&(xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			float fGlossiness;
			nReads = (UINT)::fread(&(fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			float fSmoothness;
			nReads = (UINT)::fread(&(fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			float fSmoothness;
			nReads = (UINT)::fread(&(fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			float fMetallic;
			nReads = (UINT)::fread(&(fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			float fGlossyReflection;
			nReads = (UINT)::fread(&(fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			ReadExtradataAboutTexture(pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{ 
			ReadExtradataAboutTexture(pInFile);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{ 
			ReadExtradataAboutTexture(pInFile);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			ReadExtradataAboutTexture(pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			ReadExtradataAboutTexture(pInFile);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			ReadExtradataAboutTexture(pInFile);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			ReadExtradataAboutTexture(pInFile);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	} 
}

void CAnimationObject::CacheSkinningBoneFrames(CAnimationObject* pRootFrame)
{
}

void CAnimationObject::LoadAnimationFromFile(FILE* pInFile)
{
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
			m_pAnimationController->m_ppAnimationBoneFrameCaches = new CAnimationObject * [m_pAnimationController->m_nAnimationBoneFrames];

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

				if (!strcmp(pstrToken, "<AnimationSet>:"))
				{
					haveToRead = false;
					// 더미 프레임 추가
					pAnimationSet->m_pfKeyFrameTransformTimes[i] = pAnimationSet->m_pfKeyFrameTransformTimes[i - 1] +
						(pAnimationSet->m_pfKeyFrameTransformTimes[i - 1] - pAnimationSet->m_pfKeyFrameTransformTimes[i - 2]); // 트랜스폼 적용되는 시간
				
					pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i] = pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i - 1]; // 트랜스폼


					break;
				}
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKeyFrame = 0;
					nReads = (UINT)::fread(&nKeyFrame, sizeof(int), 1, pInFile); //i

					nReads = (UINT)::fread(&pAnimationSet->m_pfKeyFrameTransformTimes[i], sizeof(float), 1, pInFile);

					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i], sizeof(float), 16 * m_pAnimationController->m_nAnimationBoneFrames, pInFile);
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

CAnimationObject* CAnimationObject::FindFrame(char* pstrFrameName)
{
	CAnimationObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void ReadExtradataAboutTexture(FILE* pInFile)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
}
