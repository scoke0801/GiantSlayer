#pragma once

#include "GameObject.h"
#include "Camera.h"

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다.
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};
enum class ShaderTypes
{
	None,
	Diffused,
	Textured,
	Billboard,
	Count
};
class CShader
{
protected:
	D3D12_SHADER_BYTECODE			m_d3dVSBytecode;
	D3D12_SHADER_BYTECODE			m_d3dPSBytecode;
	D3D12_SHADER_BYTECODE			m_d3dGSBytecode;

	ID3DBlob*						m_pd3dVertexShaderBlob = nullptr;
	ID3DBlob*						m_pd3dPixelShaderBlob = nullptr;
	ID3DBlob*						m_pd3dGeometryShaderBlob = nullptr;

	D3D12_INPUT_LAYOUT_DESC			m_d3dInputLayoutDesc;

public:
	CShader();
	virtual ~CShader();

private:
	int m_nReferences = 0;

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

	D3D12_SHADER_BYTECODE CreateVertexShader(WCHAR* pszFileName, LPCSTR pszShaderName);
	D3D12_SHADER_BYTECODE CreatePixelShader(WCHAR* pszFileName, LPCSTR pszShaderName);
	D3D12_SHADER_BYTECODE CreateGeometryShader(WCHAR* pszFileName, LPCSTR pszShaderName);
	 
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice,
		ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateGeneralShader(ID3D12Device* pd3dDevice,
		ID3D12RootSignature* pd3dGraphicsRootSignature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE d3dPrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

protected:
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};

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

class CBillboardShader : public CShader
{
private:

public:
	CBillboardShader();
	virtual ~CBillboardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
};