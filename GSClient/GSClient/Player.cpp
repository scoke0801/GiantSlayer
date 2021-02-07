#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		 50.0f, 165.0f, 50.0f );

	SetMesh(pCubeMeshTex);
}

CPlayer::~CPlayer()
{

}
