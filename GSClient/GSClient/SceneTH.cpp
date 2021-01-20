#include "stdafx.h"
#include "SceneTH.h"

CSceneTH::CSceneTH()
{

}

CSceneTH::~CSceneTH()
{

}

void CSceneTH::Update(double elapsedTime)
{
	ProcessInput();
}

void CSceneTH::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

}

void CSceneTH::ProcessInput()
{
	auto gameInput = GAME_INPUT;
	if (gameInput.KEY_W)
	{
		
	}
	if (gameInput.KEY_A)
	{

	}
	if (gameInput.KEY_S)
	{

	}
	if (gameInput.KEY_D)
	{

	}
}

void CSceneTH::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CSceneTH::ReleaseUploadBuffers()
{

}

void CSceneTH::ReleaseObjects()
{

}

ID3D12RootSignature* CSceneTH::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CSceneTH::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}