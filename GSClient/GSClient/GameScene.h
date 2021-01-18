#pragma once

#include "GameTimer.h"
#include "Shader.h"
#include "Scene.h"

class CShader;
class CGameObject;
class CCamera;

class CGameScene : public CScene
{
public:
	CGameScene();
	~CGameScene();
	
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	void AnimateObjects(float fTimeElapsed);
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	virtual void ReleaseUploadBuffers() override;

	//그래픽 루트 시그너쳐를 생성한다.

private:
	void CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override;

protected:
	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CGameObject		** m_ppObjects = NULL;
	int m_nObjects = 0;
	CShader			** m_ppShaders = NULL;
	CSkyBox			* m_pSkyBox = NULL;
	
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

};
