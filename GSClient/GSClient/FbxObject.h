#pragma once
#include "GameObject.h"
#include "Mesh.h"

class CFbxObject : public CGameObject
{
public:
	int nVertices = 0;
	int nPolygons = 0;
	int nBones = 0;
	int nAnimations = 0;
	int curAnim = 0;

	float m_time = 0;
	float t = 0;
	float endTime = 0;

	string animName;
	vector<Vertex> vertices;
	vector<int> indices;
	vector<Bone> skeleton;
	vector<Animation> animations;

	XMFLOAT4X4* m_pxmf4x4BindPoseBoneOffsets = NULL;
	ID3D12Resource* m_pd3dcbBoneOffsets = NULL;
	XMFLOAT4X4* m_pcbxmf4x4BoneOffsets = NULL;

	XMFLOAT4X4* finTransform;
	ID3D12Resource* m_pd3dcbBoneTransforms = NULL;
	XMFLOAT4X4* m_pcbxmf4x4BoneTransforms = NULL;

public:
	CFbxObject();
	CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName);
	virtual ~CFbxObject();

	void LoadFile(char* pstrFbxFileName);

	void Animate(float fTimeElapsed) override;
	void Update(float fTimeElapsed) override;
	void Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseShaderVariables() override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseUploadBuffers() override;
};