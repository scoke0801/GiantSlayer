#pragma once
#include "GameObject.h"
#include "Mesh.h"

class CMaterial;

class CGameObjectVer2 : public CGameObject
{
public:
	CGameObjectVer2();
	CGameObjectVer2(int nMaterials);
	virtual ~CGameObjectVer2();

public:
	char			m_pstrFrameName[64];

	XMFLOAT4X4		m_xmf4x4ToParent;

	CGameObjectVer2* m_pParent = NULL;
	CGameObjectVer2* m_pChild = NULL;
	CGameObjectVer2* m_pSibling = NULL;

	int				m_nMaterials = 0;
	CMaterial**		m_ppMaterials = NULL;


	CTexture* FindReplicatedTexture(_TCHAR* pstrTextureName);
};
