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

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.

private:
	void CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override;

protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CGameObject		** m_ppObjects = NULL;
	int m_nObjects = 0;
	CShader			** m_ppShaders = NULL;
	CSkyBox			* m_pSkyBox = NULL;
	
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

};
