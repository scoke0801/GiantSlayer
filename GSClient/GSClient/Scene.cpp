#include "stdafx.h"
#include "Scene.h"
 
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneJH.h" 
#include "SceneTH.h"
#include "SceneYJ.h" 
#include "SceneExport.h" 

CScene::CScene()
{

}

CScene::~CScene()
{

} 
CNullScene::CNullScene()
{

}

CNullScene::~CNullScene()
{

}

void CNullScene::Update(float elapsedTime)
{ 
	ProcessInput(); 
}

void CNullScene::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{   

}

void CNullScene::ProcessInput()
{
	auto gameInput = GAME_INPUT;
	if (gameInput.KEY_F1)
	{
		ChangeScene<CTitleScene>();
	}
	if (gameInput.KEY_F2)
	{
		ChangeScene<CSceneTH>();
	}
	if (gameInput.KEY_F3)
	{ 
		ChangeScene<CSceneYJ>();
	}
	if (gameInput.KEY_F4)
	{
		ChangeScene<CSceneJH>();
	}  
	if (gameInput.KEY_F5)
	{
		//ChangeScene<CGameScene>();
	}
	if (gameInput.KEY_F9)
	{
		ChangeScene<CSceneExport>();
	}
}
