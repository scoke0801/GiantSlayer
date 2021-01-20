#pragma once
#include "Scene.h"

class CSceneTH : public CScene
{
private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
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

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override;

};

