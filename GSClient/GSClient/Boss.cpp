#include "stdafx.h"
#include "Boss.h"

CBoss::CBoss(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dRootSignature)
{
	CGameObjectVer2* pBossParent = CGameObjectVer2::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
		pd3dRootSignature, "resources/FbxExported/Boss.bin", NULL, true);

	SetChild(pBossParent, true);
	SetAnimationSet((int)BOSS_ANIMATION::Idle);
	SetShadertoAll();

	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 11, 10, 7, XMFLOAT3{ 0,0,0 });
	AddColider(new ColliderBox(XMFLOAT3(0, 0, 0), XMFLOAT3(5.5, 5, 3.5)));
	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 4.5f, 5, 6, XMFLOAT3{ 2.5, 3, 7 });
	AddColider(new ColliderBox(XMFLOAT3(2.5, 5.5, 7), XMFLOAT3(2.25, 2.5, 3)));
	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 4.5f, 5, 6, XMFLOAT3{ -2.5, 3, 7 });
	AddColider(new ColliderBox(XMFLOAT3(-2.5, 5.5, 7), XMFLOAT3(2.25, 2.5, 3)));
	BuildBoundigBoxMesh(pd3dDevice, pd3dCommandList, PulledModel::Top, 3, 3, 5, XMFLOAT3{ 0,3,-7 });
	AddColider(new ColliderBox(XMFLOAT3(0, 4.5, -7), XMFLOAT3(1.5, 1.5, 2.5)));

}

CBoss::~CBoss()
{
}

void CBoss::Update(float elapsedTime)
{
	CGameObjectVer2::Animate(elapsedTime);
	UpdateTransform(NULL);
}

void CBoss::UpdateOnServer(float fTimeElapsed)
{
	CGameObjectVer2::Animate(fTimeElapsed);
	UpdateTransform(NULL);
}

void CBoss::PlayerEnter(CPlayer* target)
{
	if (m_isOnAwaken == false) {
		SetAnimationSet((int)BOSS_ANIMATION::Born_1);
		m_isOnAwaken = true;
	}
}
