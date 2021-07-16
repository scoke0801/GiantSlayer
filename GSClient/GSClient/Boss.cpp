#include "stdafx.h"
#include "Boss.h"
#include "State.h"
#include "Effect.h"

#include "SceneJH.h"

CBoss::CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature)
{
	CGameObjectVer2* pBossParent = LoadGeometryAndAnimationFromFileForBoss(pd3dDevice, pd3dCommandList,
		pd3dRootSignature, "resources/FbxExported/Boss.bin", NULL, true);

	SetChild(pBossParent, true);
	SetAnimationSet((int)BOSS_ANIMATION::Idle);
	SetShadertoAll();

	//BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 11, 10, 7, XMFLOAT3{ 0,0,0 });
	//AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5.5, 5, 3.5)));
	//BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 4.5f, 5, 6, XMFLOAT3{ 2.5, 3, 7 });
	//AddColider(new ColliderBox(XMFLOAT3(2.5, 5.5, 7), XMFLOAT3(2.25, 2.5, 3)));
	//BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 4.5f, 5, 6, XMFLOAT3{ -2.5, 3, 7 });
	//AddColider(new ColliderBox(XMFLOAT3(-2.5, 5.5, 7), XMFLOAT3(2.25, 2.5, 3)));
	//BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 3, 3, 5, XMFLOAT3{ 0,3,-7 });
	//AddColider(new ColliderBox(XMFLOAT3(0, 4.5, -7), XMFLOAT3(1.5, 1.5, 2.5)));

	m_State = new WaitState(this);
	m_EnemyType = EnemyType::Boss;
	m_AttackRange = 1000.0f;
	m_Speed = 0.0f * 10.0f;
}

CBoss::~CBoss()
{
}

void CBoss::Update(float elapsedTime)
{
	m_State->Execute(this, elapsedTime);

	m_SightBox.Transform(m_SightAABB, XMLoadFloat4x4(&m_xmf4x4ToParent));

	if (m_AttackDelayTime > 0.0f) {
		m_AttackDelayTime = max(m_AttackDelayTime - elapsedTime, 0.0f);
	}

	CGameObjectVer2::Animate(elapsedTime);
	UpdateTransform(NULL);
}

void CBoss::UpdateOnServer(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
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
			// 두 번 방방뛰는 애니메이션 -> 2번 사이클 돌면 좋을듯

			XMFLOAT3 targetPosition;
			//z : 21800.0 14533
			//z : 29700.0 19800
			//x : 29700.0 19800
			//x : 21800.0 14533
			int thunderCount = rand() % 5 + 5;
			auto workScene = MAIN_GAME_SCENE;
			float thunderRange = 3700.0f;
			for (int i = 0; i < thunderCount; ++i) {
				targetPosition.x = (((float)rand() / (RAND_MAX)) * (thunderRange * 2)) + m_xmf3ActivityScopeCenter.x - thunderRange;
				targetPosition.y = m_xmf3Position.y;
				targetPosition.z = (((float)rand() / (RAND_MAX)) * (thunderRange * 2)) + m_xmf3ActivityScopeCenter.z - thunderRange;

				workScene->UseEffects((int)EffectTypes::WarnningCircle, targetPosition);
				workScene->UseEffects((int)EffectTypes::Thunder, targetPosition, 1.5f);
			}
			cout << "공격 이펙트 생성 테스트\n";
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

CGameObjectVer2* CBoss::LoadGeometryAndAnimationFromFileForBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader, bool bHasAnimation)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CGameObjectVer2* pGameObject = LoadFrameHierarchyFromFileForBoss(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader);

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

CGameObjectVer2* CBoss::LoadFrameHierarchyFromFileForBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObjectVer2* pParent, FILE* pInFile, CShader* pShader)
{
	static int count = 0;
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObjectVer2* pGameObject = NULL;

	XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
	XMFLOAT4 xmf4Rotation;
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

		//------------------------------------
		// for test
		//------------------------------------
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
				pGameObject->AddColider(new ColliderBox(boundingBox.Center, boundingBox.Extents)); 
				pGameObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center,
					boundingBox.Extents.x, boundingBox.Extents.y, boundingBox.Extents.z, boundingBox.Center);
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
			CStandardMesh* pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
			if (pMesh->HasBoundingBox()) {
				auto boundingBox = pMesh->GetBoundigBox();
				pGameObject->AddColider(new ColliderBox(boundingBox.Center, boundingBox.Extents));
				 
				pGameObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center,
					boundingBox.Extents.x, boundingBox.Extents.y, boundingBox.Extents.z, boundingBox.Center);
			}
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
			if (pSkinnedMesh->HasBoundingBox()) {
				auto boundingBox = pSkinnedMesh->GetBoundigBox();
				pGameObject->AddColider(new ColliderBox(boundingBox.Center, boundingBox.Extents));

				//boundingBox.Transform(boundingBox, XMLoadFloat4x4(&pGameObject->m_xmf4x4ToParent));
				pGameObject->BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Center,
					boundingBox.Extents.x, boundingBox.Extents.y, boundingBox.Extents.z, boundingBox.Center);
			}

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
					CGameObjectVer2* pChild = LoadFrameHierarchyFromFileForBoss(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader);
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
