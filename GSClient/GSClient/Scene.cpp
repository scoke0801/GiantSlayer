#include "stdafx.h"
#include "Scene.h"

const int gNumFrameResources = 3;

CScene::CScene()
{

}

CScene::~CScene()
{
}

CGameScene1::CGameScene1()
{
}

CGameScene1::~CGameScene1()
{
}

void CGameScene1::Update(double elapsedTime)
{
	
}

bool CGameScene1::Initialize()
{
    if (!Initialize())
        return false;

    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildShapeGeometry();
    BuildRenderItems();
    BuildFrameResources();
    BuildDescriptorHeaps();
    BuildConstantBufferViews();
    BuildPSOs();

    // Execute the initialization commands.
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();

    return true;
}

