#pragma once

#include "GameObject.h"
#include "Camera.h"


enum class ShaderTypes
{
	None,
	Diffused,
	Textured,
	Billboard,
	Terrain,
	FbxAnimated,
	Particle,
	TexParticle,
	TerrainWater,  
	Shadow, 
	Count
};

class CShader
{
private:
	int m_nReferences = 0;

protected:
	D3D12_SHADER_BYTECODE			m_d3dVSBytecode;
	D3D12_SHADER_BYTECODE			m_d3dPSBytecode;
	D3D12_SHADER_BYTECODE			m_d3dHSBytecode;
	D3D12_SHADER_BYTECODE			m_d3dDSBytecode;
	D3D12_SHADER_BYTECODE			m_d3dGSBytecode;

	ID3DBlob*						m_pd3dVertexShaderBlob = nullptr;
	ID3DBlob*						m_pd3dPixelShaderBlob = nullptr;
	ID3DBlob*						m_pd3dHullShaderBlob = nullptr;
	ID3DBlob*						m_pd3dDomainShaderBlob = nullptr;
	ID3DBlob*						m_pd3dGeometryShaderBlob = nullptr;

	D3D12_INPUT_LAYOUT_DESC			m_d3dInputLayoutDesc; 

protected:
	ID3D12PipelineState**			m_ppd3dPipelineStates = NULL;
	int								m_nPipelineStates = 0;

	ID3D12PipelineState*			m_pd3dBoundaryPipelineState = NULL;

public:
	CShader();
	virtual ~CShader();

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout(ShaderTypes type);

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(WCHAR* pszFileName, LPCSTR pszShaderName);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(WCHAR* pszFileName, LPCSTR pszShaderName);

	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob** ppd3dShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(WCHAR* pszFileName, LPCSTR pszShaderName);
	virtual D3D12_SHADER_BYTECODE CreateShadowPixelShader(WCHAR* pszFileName, LPCSTR pszShaderName);
	virtual D3D12_INPUT_LAYOUT_DESC CreateShadowInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateShadowRasterizerState();
	


	// Å×¼¿ Ãß°¡
	D3D12_SHADER_BYTECODE CreateDomainShader(WCHAR* pszFileName, LPCSTR pszShaderName);
	D3D12_SHADER_BYTECODE CreateHullShader(WCHAR* pszFileName, LPCSTR pszShaderName);

	D3D12_SHADER_BYTECODE CreateGeometryShader(WCHAR* pszFileName, LPCSTR pszShaderName);

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateGeneralShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE d3dPrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		bool isCullModeOn=false, bool isBlendOn = false);
	void CreateBoundaryShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateFBXMeshShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, bool isLeftSide = true);
	void CreateUIShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
 
	void CreateParticleShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
 
	void CreateShadowShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void CreateTerrainShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World, UINT textureIndex = 0, UINT materialIndex = 0);
	virtual void ReleaseShaderVariables();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnPrepareShadowRender(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void RenderBoundary(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render_Shadow(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
}; 

///////////////////////////////////////////////////////////////////////////////////////
//
class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();
	virtual ~CDiffusedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice,
		ID3D12RootSignature* pd3dGraphicsRootSignature);
};

class CTexturedShader : public CShader
{
private:
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int index);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

///////////////////////////////////////////////////////////////////////////////////////
// 
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice,
		ID3D12RootSignature* pd3dGraphicsRootSignature);
}; 

///////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxShader : public CShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState() override;
}; 

///////////////////////////////////////////////////////////////////////////////////////
//
class CBillboardShader : public CShader
{ 
public:
	CBillboardShader();
	virtual ~CBillboardShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
};

class CTerrainTessellationShader : public CShader
{
public:
	CTerrainTessellationShader();
	virtual ~CTerrainTessellationShader();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
