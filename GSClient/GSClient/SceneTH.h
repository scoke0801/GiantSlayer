#pragma once
#include "Scene.h"

class CSceneTH : public CScene
{
private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	CGameObject** m_ppObjects = NULL;
	int m_nObjects = 0;

public:
	CSceneTH();
	~CSceneTH();

	virtual void Update(double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	virtual void ProcessInput();

	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseUploadBuffers() override;
	void ReleaseObjects();

	//그래픽 루트 시그너쳐를 생성한다.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override;

};

