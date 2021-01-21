#include "stdafx.h"
#include "Scene.h"
 
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneJH.h" 

CScene::CScene()
{

}

CScene::~CScene()
{
}

CNullScene::CNullScene()
{ 
	testbox1.SetPosition({ 10, 0, 0 });
	testbox1.SetVelocity({ 0.1f, 0, 0 });

	testbox2.SetPosition({ 20, 0, 0 });
	testbox2.SetVelocity({ -0.1f, 0, 0 }); 
}

CNullScene::~CNullScene()
{
}

void CNullScene::Update(double elapsedTime)
{ 
	ProcessInput(); 
	testbox1.Update();
	testbox2.Update();

}

void CNullScene::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{   
	//cout << "박스1: ";
	//testbox1.Draw(pd3dCommandList, pCamera);
	//cout << "박스2: ";
	//testbox2.Draw(); 
	cout << endl;

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
		//ChangeScene<CLobbyScene>();
	}
	if (gameInput.KEY_F3)
	{
		ChangeScene<CGameScene>();
	}
	if (gameInput.KEY_F4)
	{
		ChangeScene<CSceneJH2>();
	}  
}
