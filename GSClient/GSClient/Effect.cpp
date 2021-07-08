#include "stdafx.h"
#include "Effect.h"
#include "Shader.h"

CEffect::CEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float x, float z)
{ 
	CTexturedRectMesh* pMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, x, z, 0.0f);
	SetMesh(pMesh);
	SetShader(CShaderHandler::GetInstance().GetData("Effect"));
	m_isDrawbale = false;
}

CEffect::~CEffect()
{
}

void CEffect::Update(float elapsedTime)
{
}

void CEffect::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_isDrawbale) return;
	OnPrepareRender();

	if (m_pShader)
	{ 
		//���� ��ü�� ���� ��ȯ ����� ���̴��� ��� ���۷� ����(����)�Ѵ�.
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World, m_nTextureIndex, 0);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) {
		m_pMesh->Render(pd3dCommandList);
	}
}
