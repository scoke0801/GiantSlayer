#pragma once

#include "GameObject.h"

class CAnimationController;

// 애니메이션 정보를 통해 바운딩박스를 업데이트 하고자 정의하는 클래스입니다.
// 애니메이션 정보를 가지는 객체들은 
// 해당 클래스를 상속받는 객체로 정의하거나 직접 사용하도록 합니다.
class CAnimationObject : public CGameObject
{
public: 
	CAnimationObject();

public:
	void Update(float fTimeElapsed) override;
	void Animate(float fTimeElapsed);
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	 
public:
	void SetPosition(XMFLOAT3 pos) override;
	void Move(XMFLOAT3 shift) override;
	void Scale(float x, float y, float z, bool setSize = true) override;
	void Rotate(const XMFLOAT3& pxmf3Axis, float fAngle)override;
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up) override;

	void SetChild(CAnimationObject* pChild, bool bReferenceUpdate = false);

public: // about collision
	void UpdateColliders() override;
	bool CollisionCheck(CGameObject* other) override;

	void GetAABBToBuffer(vector<BoundingBox*>& buffer);
	void CollectAABBFromChilds();

	void SetAABB(vector<BoundingBox*>& buffer) { m_AABB = buffer; buffer.clear(); }

public: // about animation
	int GetAnimationSet()const { return m_AnimationSet; }
	void SetAnimationSet(int nAnimationSet);
	void SetAnimationType(int nType);

	void SetDrawableRecursively(char* name, bool draw);
	void FindWeapon(char* name, CAnimationObject* root);
public: // about read
	static CAnimationObject* LoadFrameHierarchyFromFile(CAnimationObject* pParent, FILE* pInFile);
	static CAnimationObject* LoadGeometryAndAnimationFromFile(const char* pstrFileName, bool bHasAnimation); 
	static bool LoadMeshInfoFromFile(FILE* pInFile, XMFLOAT3& center, XMFLOAT3& extents);
	static bool LoadSkinInfoFromFile(FILE* pInFile, XMFLOAT3& center, XMFLOAT3& extents);
	static void LoadMaterialsFromFile(CAnimationObject* pParent, FILE* pInFile);
	 
	void CacheSkinningBoneFrames(CAnimationObject* pRootFrame);
	void LoadAnimationFromFile(FILE* pInFile);

	CAnimationObject* FindFrame(char* pstrFrameName); 

public:
	XMFLOAT4X4					m_xmf4x4ToParent;

	char						m_pstrFrameName[64];
	 
	CAnimationObject*			m_pParent = nullptr;
	CAnimationObject*			m_pChild = nullptr;
	CAnimationObject*			m_pSibling = nullptr;

	CAnimationObject*			m_pWeapon = NULL;

	CAnimationController*		m_pAnimationController = NULL;
	int							m_AnimationSet = 0;
	bool m_AnimationPaused = false;
};

void ReadExtradataAboutTexture(FILE* pInFile);