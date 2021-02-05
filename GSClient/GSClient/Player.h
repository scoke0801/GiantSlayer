#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "stdafx.h"

class CPlayer : public CGameObject
{ 
public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CPlayer();

};