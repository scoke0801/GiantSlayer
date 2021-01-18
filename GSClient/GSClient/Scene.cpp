#include "stdafx.h"
#include "Scene.h"



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

	testbox1.Update();
	testbox2.Update();

}

void CNullScene::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	cout << "박스1: ";
	//testbox1.Draw();
	cout << "박스2: ";
	//testbox2.Draw();
	cout << endl;

}
