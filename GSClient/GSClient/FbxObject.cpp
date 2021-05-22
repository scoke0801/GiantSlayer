#include "stdafx.h"
#include "FbxObject.h"
#include "Shader.h"
#include "ShaderHandler.h"
#include "Camera.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CFbxObject::CFbxObject()
{
	m_xmf3Position = { 0, 0, 0 };
	m_xmf3Velocity = { 0, 0, 0 };
	m_time = 0;

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CFbxObject::CFbxObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	LoadFile(pstrFbxFileName);

	finTransform = new XMFLOAT4X4[100];

	cout << indices.size() << " " << vertices.size() << " " << skeleton.size() << endl;

	//CFixedMesh* tempMesh = new CFixedMesh(pd3dDevice, pd3dCommandList, pstrFbxFileName);
	CAnimatedMesh* tempMesh = new CAnimatedMesh(pd3dDevice, pd3dCommandList, vertices, indices, skeleton);

	SetMesh(tempMesh);
	//SetShader(CShaderHandler::GetInstance().GetData("FBX"));
	SetShader(CShaderHandler::GetInstance().GetData("FbxAnimated"));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_time = 0;
	t = 0;
	endTime = 0;
	for (int i = 0; i < skeleton.size(); i++) {
		float t = animations[curAnim].BoneAnimation[i].keyframes.back().frameTime;
		if (t > endTime)
			endTime = t;
	}

	cout << endTime << endl;
}

CFbxObject::~CFbxObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CFbxObject::LoadFile(char* pstrFbxFileName)
{
	ifstream file;
	file.open(pstrFbxFileName, ios::in | ios::binary);
	string temp;
	int count;

	file >> temp >> nVertices;
	file >> temp >> nPolygons;
	file >> temp >> nBones;
	file >> temp;

	cout << "파일 로드: " << nVertices << " " << nPolygons << " " << nBones << endl;

	for (int i = 0; i < nVertices; i++) {
		Vertex tempVertex;

		/*file >> tempVertex.pos.x >> tempVertex.pos.z >> tempVertex.pos.y >>
				tempVertex.uv.x >> tempVertex.uv.y >>
				tempVertex.normal.x >> tempVertex.normal.z >> tempVertex.normal.y;*/
		file >> tempVertex.pos.x >> tempVertex.pos.y >> tempVertex.pos.z >>
			tempVertex.uv.x >> tempVertex.uv.y >>
			tempVertex.normal.x >> tempVertex.normal.y >> tempVertex.normal.z;
		file >> tempVertex.indices[0] >> tempVertex.indices[1] >> tempVertex.indices[2] >> tempVertex.indices[3] >>
				tempVertex.weights.x >> tempVertex.weights.y >> tempVertex.weights.z >> temp;

		tempVertex.pos.z *= -1;
		vertices.push_back(tempVertex);
	}
	file >> temp;

	for (int i = 0; i < (nPolygons * 3); i++) {
		int tempInt;
		file >> tempInt;
		indices.push_back(tempInt);
	}
	file >> temp;

	if (nBones != 0) {
		m_pxmf4x4BindPoseBoneOffsets = new XMFLOAT4X4[nBones];

		for (int i = 0; i < nBones; i++) {
			Bone tempBone;

			file >> temp >> tempBone.name >> tempBone.parentIndex;
			file >> tempBone.offset._11 >> tempBone.offset._12 >> tempBone.offset._13 >> tempBone.offset._14 >>
				tempBone.offset._21 >> tempBone.offset._22 >> tempBone.offset._23 >> tempBone.offset._24 >>
				tempBone.offset._31 >> tempBone.offset._32 >> tempBone.offset._33 >> tempBone.offset._34 >>
				tempBone.offset._41 >> tempBone.offset._42 >> tempBone.offset._43 >> tempBone.offset._44;

			//cout << tempBone.name << " " << tempBone.parentIndex << endl;
			skeleton.push_back(tempBone);
			m_pxmf4x4BindPoseBoneOffsets[i] = tempBone.offset;
		}
		file >> temp;

		file >> temp >> nAnimations;
		animations.resize(nAnimations);
		for (int i = 0; i < nAnimations; i++) {
			file >> temp >> animations[i].name;
			animations[i].BoneAnimation.resize(nBones);

			for (int j = 0; j < nBones; j++) {
				int nKeyframe;
				file >> temp >> nKeyframe;
				for (int k = 0; k < nKeyframe; k++) {
					Keyframe tempKey;

					file >> tempKey.frameTime;
					file >> tempKey.translation.x >> tempKey.translation.y >> tempKey.translation.z >>
							tempKey.scale.x >> tempKey.scale.y >> tempKey.scale.z >>
							tempKey.rotationquat.x >> tempKey.rotationquat.y >> tempKey.rotationquat.z >> tempKey.rotationquat.w;
					
					/*file >> tempKey.animatrix._11 >> tempKey.animatrix._12 >> tempKey.animatrix._13 >> tempKey.animatrix._14 >>
							tempKey.animatrix._21 >> tempKey.animatrix._22 >> tempKey.animatrix._23 >> tempKey.animatrix._24 >>
							tempKey.animatrix._31 >> tempKey.animatrix._32 >> tempKey.animatrix._33 >> tempKey.animatrix._34 >>
							tempKey.animatrix._41 >> tempKey.animatrix._42 >> tempKey.animatrix._43 >> tempKey.animatrix._44;*/
					
					animations[i].BoneAnimation[j].keyframes.push_back(tempKey);
				}
			}
		}
	}

	//cout << skeleton[skeleton.size()-1].animations[0].translation.x << endl;
	file.close();
}

void CFbxObject::AnimateVer2(float fTimeElapsed)
{
	m_time += fTimeElapsed;

	if (m_time > endTime)
		m_time = 0;

	for (UINT i = 0; i < nBones; i++)
	{
		vector<Keyframe> curKF = animations[curAnim].BoneAnimation[i].keyframes;

		if (m_time <= curKF.front().frameTime) {	// 시작
			finTransform[i] = curKF.front().animatrix;
		}
		else if (m_time >= curKF.back().frameTime) {	// 끝
			finTransform[i] = curKF.back().animatrix;
		}
		else {
			for (UINT j = 0; j < curKF.size() - 1; j++) {
				if (m_time >= curKF[j].frameTime && m_time <= curKF[j + 1].frameTime) {
					finTransform[i] = curKF[j].animatrix;
					break;
				}
			}
		}
	}

	for (UINT i = 0; i < nBones; i++)
	{
		XMMATRIX offset = XMLoadFloat4x4(&skeleton[i].offset);
		XMMATRIX toRoot = XMLoadFloat4x4(&finTransform[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
		XMStoreFloat4x4(&finTransform[i], XMMatrixTranspose(finalTransform));
	}
}

void CFbxObject::Animate(float fTimeElapsed)
{
	m_time += fTimeElapsed;

	if (m_time > endTime)
		m_time = 0;

	vector<XMFLOAT4X4> toParentTransforms(nBones);

	animations[curAnim].Interpolate(m_time, toParentTransforms);
	
	vector<XMFLOAT4X4> toRootTransforms(nBones);
	toRootTransforms[0] = toParentTransforms[0];

	for (UINT i = 1; i < nBones; i++)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = skeleton[i].parentIndex;
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	for (UINT i = 0; i < nBones; i++)
	{
		XMMATRIX offset = XMLoadFloat4x4(&skeleton[i].offset);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);

		XMStoreFloat4x4(&finTransform[i], XMMatrixTranspose(finalTransform));
	}

	/*cout << finTransform[nBones - 1]._11 << " " << finTransform[nBones - 1]._12 << " " << finTransform[nBones - 1]._13 << " " << finTransform[nBones - 1]._14 << " " <<
		finTransform[nBones - 1]._21 << " " << finTransform[nBones - 1]._22 << " " << finTransform[nBones - 1]._23 << " " << finTransform[nBones - 1]._24 << " " <<
		finTransform[nBones - 1]._31 << " " << finTransform[nBones - 1]._32 << " " << finTransform[nBones - 1]._33 << " " << finTransform[nBones - 1]._34 << " " <<
		finTransform[nBones - 1]._41 << " " << finTransform[nBones - 1]._42 << " " << finTransform[nBones - 1]._43 << " " << finTransform[nBones - 1]._44 << endl;*/
}

void CFbxObject::Update(float fTimeElapsed)
{
	static float MaxVelocityXZ = 120.0f;
	static float MaxVelocityY = 120.0f;
	static float Friction = 50.0f;

	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = MaxVelocityXZ * fTimeElapsed;
	if (fLength > MaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = MaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > MaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity);

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (Friction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

	Animate(fTimeElapsed);
	//AnimateVer2(fTimeElapsed);

}

void CFbxObject::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	UpdateShaderVariables(pd3dCommandList);

	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);

	if (gbBoundaryOn)
	{
		m_pShader->RenderBoundary(pd3dCommandList, pCamera);
		for (auto pBoundingMesh : m_BoundingObjectMeshes)
		{
			pBoundingMesh->Render(pd3dCommandList);
		}
	}
}

void CFbxObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * 100) + 255) & ~255); //256의 배수
	m_pd3dcbBoneOffsets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbBoneOffsets->Map(0, NULL, (void**)&m_pcbxmf4x4BoneOffsets);

	m_pd3dcbBoneTransforms = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbBoneTransforms->Map(0, NULL, (void**)&m_pcbxmf4x4BoneTransforms);
}
void CFbxObject::ReleaseShaderVariables()
{
	if (m_pd3dcbBoneOffsets) m_pd3dcbBoneOffsets->Release();
	if (m_pd3dcbBoneTransforms) m_pd3dcbBoneTransforms->Release();
}
void CFbxObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbBoneOffsets && m_pd3dcbBoneTransforms)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbBoneOffsets->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dcbBoneOffsetsGpuVirtualAddress); //Skinned Bone Offsets
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = m_pd3dcbBoneTransforms->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(8, d3dcbBoneTransformsGpuVirtualAddress); //Skinned Bone Transforms

		for (int i = 0; i < nBones; i++)
		{
			XMStoreFloat4x4(&m_pcbxmf4x4BoneOffsets[i], XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4BindPoseBoneOffsets[i])));
			XMStoreFloat4x4(&m_pcbxmf4x4BoneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&finTransform[i])));
		}
	}
}
void CFbxObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}