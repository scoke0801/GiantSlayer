#include "stdafx.h"
#include "Boss.h" 
#include "AnimationController.h"

CBoss::CBoss()  
{
	// m_xmf4x4ToParent = Matrix4x4::Identity();
	// m_xmf4x4World = Matrix4x4::Identity();

	CAnimationObject* pBossModel = LoadGeometryAndAnimationFromFileForBoss(
		"resources/FbxExported/Boss.bin", true);
	SetChild(pBossModel, true);

	m_ExistingSector = SECTOR_POSITION::SECTOR_5;

	SetAnimationSet((int)BOSS_ANIMATION::Idle);
	 
	m_State = new WaitState(this);
	m_EnemyType = EnemyType::Boss; 
	m_AttackRange = 1000.0f;
	m_Speed = 165.0f * 10.0f;

	m_TargetPlayer = nullptr;
}


CBoss::~CBoss()
{
}

void CBoss::Update(float elapsedTime)
{
	m_State->Execute(this, elapsedTime);

	m_SightBox.Transform(m_SightAABB, XMLoadFloat4x4(&m_xmf4x4World));

	if (m_AttackDelayTime > 0.0f) {
		m_AttackDelayTime = max(m_AttackDelayTime - elapsedTime, 0.0f);
	}
	CAnimationObject::Animate(elapsedTime);
	UpdateTransform(NULL);
}


void CBoss::Attack(float elapsedTime)
{
	if (m_AttackDelayTime <= 0.0f) {
		// 실제 공격!  
		if (m_AttackType == EnemyAttackType::BossSkill_1) {
			m_AttackDelayTime = BOSS_ATTACK_1_ANIMATION_LENGTH + 0.3f;
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_2) {
			m_AttackDelayTime = BOSS_ATTACK_2_ANIMATION_LENGTH + 0.3f;
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_3) {
			m_AttackDelayTime = BOSS_ATTACK_3_ANIMATION_LENGTH + 0.5f;
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_4) {
			m_AttackDelayTime = BOSS_ATTACK_4_ANIMATION_LENGTH + 0.7f;
		}
	}
}

void CBoss::CalcNextAttackType()
{
	m_AttackType = (EnemyAttackType)(rand() % 4 + (int)EnemyAttackType::BossSkill_1);
	if (m_AttackType == EnemyAttackType::BossSkill_1) {
		m_AttackRange = 1000.0f;
		m_AttackDelayTime = BOSS_ATTACK_1_ANIMATION_LENGTH + 1.0f;
	}
	else if (m_AttackType == EnemyAttackType::BossSkill_2) {
		m_AttackRange = 1000.0f;
		m_AttackDelayTime = BOSS_ATTACK_2_ANIMATION_LENGTH + 1.0f;
	}
	else if (m_AttackType == EnemyAttackType::BossSkill_3) {
		m_AttackRange = 3000.0f;
		m_AttackDelayTime = BOSS_ATTACK_3_ANIMATION_LENGTH + 1.0f;
	}
	else if (m_AttackType == EnemyAttackType::BossSkill_4) {
		m_AttackRange = 16000.0f;
		m_AttackDelayTime = BOSS_ATTACK_3_ANIMATION_LENGTH + 1.0f;
	}
}

void CBoss::PlayerEnter(CPlayer* target)
{
	if (m_isOnAwaken == false) {
		SetAnimationSet((int)BOSS_ANIMATION::Born_1);
		m_isOnAwaken = true;
	}
}
CAnimationObject* CBoss::LoadGeometryAndAnimationFromFileForBoss(const char* pstrFileName, bool bHasAnimation)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CAnimationObject* pGameObject = LoadFrameHierarchyFromFileForBoss( NULL, pInFile);

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

	pGameObject->CollectAABBFromChilds();
	return(pGameObject);
}
CAnimationObject* CBoss::LoadFrameHierarchyFromFileForBoss(CAnimationObject* pParent, FILE* pInFile)
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

			BoundingBox boundingBox;
			bool isMainPart = true;
			vector<string> frames = {
				"Orthos",
				"Bip001",
				"magmadar",
				"Bip001_Neck"
			};
			for (auto frame : frames) {
				if (0 == strcmp(pGameObject->m_pstrFrameName, frame.c_str())) {
					isMainPart = false;
				}
			}
			if (0 == strcmp(pGameObject->m_pstrFrameName, "Bip001_R_Head02")) {
				boundingBox = BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(6.0, 4.0, 2.0));
			}
			else if (0 == strcmp(pGameObject->m_pstrFrameName, "Bip001_L_Head02")) {
				boundingBox = BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(6.0, 4.0, 2.0));
			}
			else {
				boundingBox = BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.5, 1.5, 1.5));
			}
			 
			if (true == isMainPart) {
				XMFLOAT3 half = Vector3::Multifly(boundingBox.Extents, 0.5f);
				pGameObject->AddBoundingBox(new BoundingBox{ boundingBox.Center, half });
			}
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
				pGameObject->AddBoundingBox(new BoundingBox{ center, half });
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
				pGameObject->AddBoundingBox(new BoundingBox{ center, half });
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

void CBoss::ChangeAnimation(ObjectState stateInfo)
{
	switch (stateInfo)
	{
	case ObjectState::Wait:
		SetAnimationSet((int)BOSS_ANIMATION::Idle);
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Idle:
		SetAnimationSet((int)BOSS_ANIMATION::Idle);
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Patrol:
		SetAnimationSet((int)BOSS_ANIMATION::Run);
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Trace:
		SetAnimationSet((int)BOSS_ANIMATION::Run);
		SetAnimationType(ANIMATION_TYPE_LOOP);
		break;
	case ObjectState::Attack:
	{
		if (m_AttackType == EnemyAttackType::BossSkill_1) {
			SetAnimationSet((int)BOSS_ANIMATION::Skill_1);
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_2) {
			SetAnimationSet((int)BOSS_ANIMATION::Skill_2);
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_3) {
			SetAnimationSet((int)BOSS_ANIMATION::Skill_3);
		}
		else if (m_AttackType == EnemyAttackType::BossSkill_4) {
			SetAnimationSet((int)BOSS_ANIMATION::Born_2);
		}
	}
	SetAnimationType(ANIMATION_TYPE_ONCE);
	break;
	case ObjectState::Attacked:
		break;
	case ObjectState::Die:
		SetAnimationSet((int)BOSS_ANIMATION::Dead);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::RunAway:
		break;
	case ObjectState::BossSkill_1:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_1);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_2:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_2);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_3:
		SetAnimationSet((int)BOSS_ANIMATION::Skill_3);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_4:
		SetAnimationSet((int)BOSS_ANIMATION::Born_2);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	case ObjectState::BossSkill_5:
		break;
	case ObjectState::BossBorn:
		SetAnimationSet((int)BOSS_ANIMATION::Born_1);
		SetAnimationType(ANIMATION_TYPE_ONCE);
		break;
	default:
		break;
	}
}

BOSS_ANIMATION CBoss::GetCurrentAnimation() const
{
	int state = GetAnimationSet();
	return BOSS_ANIMATION();
}
