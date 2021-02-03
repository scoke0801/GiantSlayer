#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Player.h"

class CSceneTH : public CScene
{
private:
	ID3D12RootSignature*	m_pd3dGraphicsRootSignature = NULL;

	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CGameObject**			m_ppObjects = NULL;
	CPlayer**				m_ppPlayers = NULL;

	CCamera**				m_Cameras;
	CCamera*				m_CurrentCamera = nullptr;

	int						m_nObjects = 0;
	int						m_nPlayers = 0;

	POINT					m_LastMousePos;

	// FBX
	FbxManager*				m_pfbxManager = nullptr;
	FbxScene*				m_pfbxScene = nullptr;
	FbxIOSettings*			m_pfbxIOs = nullptr;
	FbxImporter*			m_pfbxImporter = nullptr;

public:
	CSceneTH();
	~CSceneTH();

	virtual void Update(double elapsedTime) override;
	virtual void Draw(ID3D12GraphicsCommandList* pd3dCommandList) override;
	 
	virtual void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseUploadBuffers() override;
	void ReleaseObjects();

	virtual void BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int width, int height) override;

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	virtual ID3D12RootSignature* GetGraphicsRootSignature() override;

public:
	// about Keyboard process
	virtual void ProcessInput();

	// about Mouse process
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override; 
};

