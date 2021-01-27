#include "stdafx.h"
#include "SceneJH.h"
#include "stdafx.h" 
#include "GameFramework.h"
#include "Shader.h"
#include "UI.h"
#include "Camera.h"

#pragma region SceneJH - About Minimap
CSceneJH::CSceneJH()
{
	m_pd3dGraphicsRootSignature = NULL;
}

CSceneJH::~CSceneJH()
{
}

void CSceneJH::Update(double elapsedTime)
{
	//m_pcbMappedTestData->MouseClikced = CInputHandler::GetInstance().TestingMouseClick;
	//
	//D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dTestData->GetGPUVirtualAddress();
	//
	//pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

void CSceneJH::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 그래픽 루트 시그너쳐를 설정한다.
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	// 파이프라인 상태를 설정한다.
	pd3dCommandList->SetPipelineState(GET_PSO("TestTitlePSO"));

	//if (pCamera)
	//{
	//	pCamera->UpdateShaderVariables(pd3dCommandList, 1);
	//	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//}
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, tex);

	// m_pcbMappedTestData->MouseClikced = CInputHandler::GetInstance().TestingMouseClick;

	// D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dTestData->GetGPUVirtualAddress();

	// pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);

	// 프리미티브 토폴로지를 설정한다.
	pd3dCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 정점 6개를 사용하여 렌더링 한다.
	pd3dCommandList->DrawInstanced(18, 1, 0, 0);

	pd3dCommandList->SetPipelineState(GET_PSO("MinimapPSO"));
	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
	pd3dCommandList->DrawInstanced(72, 1, 0, 0);
}

void CSceneJH::DrawUI(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CSceneJH::ProcessInput()
{
}

void CSceneJH::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateRootSignature(pd3dDevice, pd3dCommandList);
	CreatePipelineState(pd3dDevice, pd3dCommandList);

	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);
	BuildObjects(pd3dDevice, pd3dCommandList);
}

void CSceneJH::CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 4;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; // SceneData
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

#pragma region sampler
	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[1];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_pd3dGraphicsRootSignature));

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}

void CSceneJH::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 정점 셰이더와 픽셸 셰이더를 생성한다.
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;

	auto pTempPSO = make_unique<ID3D12PipelineState*>();

	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 
	HRESULT hRes = D3DCompileFromFile(L"Shaders/MinimapTest.hlsl", NULL, NULL,
		"VSTextured", "vs_5_1", nCompileFlags, 0, &pd3dVertexShaderBlob, NULL);

	hRes = D3DCompileFromFile(L"Shaders/MinimapTest.hlsl", NULL, NULL,
		"PSTextured", "ps_5_1", nCompileFlags, 0, &pd3dPixelShaderBlob, NULL);

	//	그래픽 파이프라인 상태를 설정한다.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS.pShaderBytecode = pd3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pd3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pd3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pd3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = CreateDefaultRasterizerDesc();
	d3dPipelineStateDesc.BlendState = CreateDefaultBlendDesc();
	d3dPipelineStateDesc.DepthStencilState = CreateDefaultDepthStencilDesc();
	d3dPipelineStateDesc.InputLayout.NumElements = 0;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	//d3dPipelineStateDesc.StreamOutput = 0;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		IID_PPV_ARGS(pTempPSO.get()));

	m_PSOs["TestTitlePSO"] = std::move(pTempPSO);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

void CSceneJH::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	BuildConstantsBuffers(pd3dDevice, pd3dCommandList);

	BuildOBJAboutMinimap(pd3dDevice, pd3dCommandList);
}

void CSceneJH::BuildConstantsBuffers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*UINT ncbElementBytes = CalcCBufferSize(sizeof(TestData));
	::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dTestData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dTestData->Map(0, NULL, (void**)&m_pcbMappedTestData);*/
}

void CSceneJH::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto multiButtonTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, multiButtonTex.get(), "MultiButton", L"resources/UI/MultiPlayButton.dds");

	auto singleButtonTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, singleButtonTex.get(), "SingleButton", L"resources/UI/SinglePlayButton.dds");

	auto titleTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, titleTex.get(), "Title", L"resources/UI/TitleTest.dds");

	auto minimapTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, minimapTex.get(), "MiniMap", L"resources/UI/Minimap.dds");

	m_Textures[multiButtonTex->m_Name] = std::move(multiButtonTex);
	m_Textures[singleButtonTex->m_Name] = std::move(singleButtonTex);
	m_Textures[titleTex->m_Name] = std::move(titleTex);
	m_Textures[minimapTex->m_Name] = std::move(minimapTex);
}

void CSceneJH::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 4;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dSrvDescriptorHeap));

	//
	// Fill out the heap with actual descriptors.
	//
	D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = m_pd3dSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	auto multiBtnTex = m_Textures["MultiButton"]->m_pd3dResource;
	auto simpleBtnTex = m_Textures["SingleButton"]->m_pd3dResource;
	auto titleTex = m_Textures["Title"]->m_pd3dResource;
	auto minimapTex = m_Textures["MiniMap"]->m_pd3dResource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = multiBtnTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	pd3dDevice->CreateShaderResourceView(multiBtnTex, &srvDesc, hDescriptor);

	// next descriptor 
	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = simpleBtnTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(simpleBtnTex, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = titleTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(titleTex, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = minimapTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(minimapTex, &srvDesc, hDescriptor);
}

void CSceneJH::BuildOBJAboutMinimap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	BasicVertex vertices[72];
	float radius = 0.3f;
	float angle = 15;
	for (int i = 0; i < 24; ++i)
	{
		vertices[i * 3].xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertices[i * 3 + 1].xmf3Position = XMFLOAT3(std::cos(GetRadian((i + 1) * angle)) * radius, std::sin(GetRadian(i + 1) * angle) * radius, 0.0f);
		vertices[i * 3 + 2].xmf3Position = XMFLOAT3(std::cos(GetRadian(i * angle)) * radius, std::sin(GetRadian(i * angle)) * radius, 0.0f);

		vertices[i * 3].m_xmf2TexC = XMFLOAT2(0.5f, 0.5f);
		vertices[i * 3 + 1].m_xmf2TexC = XMFLOAT2(std::cos(GetRadian((i + 1) * angle)) * 0.5 + 0.5f, 1 - (std::sin(GetRadian(i + 1) * angle) * 0.5f + 0.5f));
		vertices[i * 3 + 2].m_xmf2TexC = XMFLOAT2(std::cos(GetRadian(i * angle)) * 0.5f + 0.5f, 1 - (std::sin(GetRadian(i * angle)) * 0.5f + 0.5f));
	}

	int nVertices = 72;
	int stride = sizeof(BasicVertex);
	int sizeInBytes = nVertices * stride;

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, &vertices, stride * nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = stride;
	m_d3dVertexBufferView.SizeInBytes = stride * nVertices;

	// 정점 셰이더와 픽셸 셰이더를 생성한다.
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;
	auto pTempPSO = make_unique<ID3D12PipelineState*>();
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif  
	HRESULT hRes = D3DCompileFromFile(L"Shaders/MinimapTest.hlsl", NULL, NULL,
		"VSMinimap", "vs_5_1", nCompileFlags, 0, &pd3dVertexShaderBlob, NULL);

	hRes = D3DCompileFromFile(L"Shaders/MinimapTest.hlsl", NULL, NULL,
		"PSMinimap", "ps_5_1", nCompileFlags, 0, &pd3dPixelShaderBlob, NULL);

	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCORD" , 0, DXGI_FORMAT_R32G32_FLOAT,	 0,  12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	//	그래픽 파이프라인 상태를 설정한다.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS.pShaderBytecode = pd3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pd3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pd3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pd3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = CreateDefaultRasterizerDesc();
	d3dPipelineStateDesc.BlendState = CreateDefaultBlendDesc();
	d3dPipelineStateDesc.DepthStencilState = CreateDefaultDepthStencilDesc();
	d3dPipelineStateDesc.InputLayout = d3dInputLayoutDesc;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	//d3dPipelineStateDesc.StreamOutput = 0;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		IID_PPV_ARGS(pTempPSO.get()));

	m_PSOs["MinimapPSO"] = std::move(pTempPSO);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}
#pragma endregion

#pragma region SceneJH2 - About UIs
CSceneJH2::CSceneJH2()
{
	m_pd3dGraphicsRootSignature = NULL;
}
CSceneJH2::~CSceneJH2()
{

}

void CSceneJH2::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	BuildObjects(pd3dDevice, pd3dCommandList);
}

void CSceneJH2::BuildCamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	int width, int height)
{
	int nCameras = 5;
	m_Cameras = new CCamera * [nCameras];
	for (int i = 0; i < nCameras; ++i)
	{
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, width, height, 1.0f, 5000.0f);
		pCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
		pCamera->SetScissorRect(0, 0, width, height);
		pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_Cameras[i] = pCamera;
	}
	m_Cameras[0]->SetPosition(	  0.0f,    10.0f, -150.0f);
	m_Cameras[1]->SetPosition( 1000.0f,    10.0f, -150.0f);
	m_Cameras[2]->SetPosition(-1000.0f,    10.0f, -150.0f);
	m_Cameras[3]->SetPosition(	  0.0f,  1010.0f, -150.0f);
	m_Cameras[4]->SetPosition(	  0.0f, -1010.0f, -150.0f);

	m_CurrentCamera = m_Cameras[0];
}

void CSceneJH2::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nObjects = 9;
	m_ppObjects = new CGameObject * [m_nObjects];

	CShader* pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders/JHTestShader.hlsl", "VSTextured");
	pShader->CreatePixelShader(L"Shaders/JHTestShader.hlsl", "PSTextured");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	for (int i = 0; i < m_nObjects; ++i)
	{
		CBox* pBox = new CBox(pd3dDevice, pd3dCommandList, 50.0f, 50.0f, 50.0f);
		pBox->SetShader(pShader);
		m_ppObjects[i] = pBox;
	}

	m_ppObjects[0]->SetPosition({	  0,     0,  0 });
	m_ppObjects[1]->SetPosition({  1000,     0,  0 });
	m_ppObjects[2]->SetPosition({ -1000,     0,  0 });
	m_ppObjects[3]->SetPosition({	  0,  1000,  0 });
	m_ppObjects[4]->SetPosition({	  0, -1000,  0 });

	//pShader = new CBillboardShader();
	//pShader->CreateVertexShader(L"Shaders/JHTestShader.hlsl", "VSBillboard");
	//pShader->CreatePixelShader(L"Shaders/JHTestShader.hlsl", "PSBillboard");
	//pShader->CreateGeometryShader(L"Shaders/JHTestShader.hlsl", "GSBillboard");
	//pShader->CreateInputLayout(ShaderTypes::Billboard);
	//pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POIsNT);

	m_nUIStartIndex = 5;
	m_ppObjects[5] = new UI(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.0f);
	m_ppObjects[5]->SetPosition({ -0.75, 0.75,  0.9 });
	m_ppObjects[5]->SetTextureIndex(0x01);

	m_ppObjects[6] = new UI(pd3dDevice, pd3dCommandList, 2.0f, 1.0f, 0.0f); 
	m_ppObjects[6]->SetPosition({ -0.0, -0.5,  0.9 });
	m_ppObjects[6]->SetTextureIndex(0x02);

	m_ppObjects[7] = new UI(pd3dDevice, pd3dCommandList, 0.3f);
	m_ppObjects[7]->SetPosition({ 0.7, 0.7,  0.9 });
	m_ppObjects[7]->SetTextureIndex(0x04);

	m_ppObjects[8] = new UI(pd3dDevice, pd3dCommandList, 0.26f);
	m_ppObjects[8]->SetPosition({ 0.7, 0.7,  0.8 }); 
	m_ppObjects[8]->SetTextureIndex(0x01);

	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders/JHTestShader.hlsl", "VS_UI_Textured");
	pShader->CreatePixelShader(L"Shaders/JHTestShader.hlsl", "PS_UI_Textured");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	m_ppObjects[5]->SetShader(pShader);	
	m_ppObjects[6]->SetShader(pShader);
	m_ppObjects[7]->SetShader(pShader);
	m_ppObjects[8]->SetShader(pShader);
}

void CSceneJH2::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto boxTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, boxTex.get(), "Box", L"resources/OBJ/Box.dds");

	auto testPlayerUITex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, testPlayerUITex.get(), "PlayerTestUI", L"resources/UI/[Test]PlayerInfo.dds");

	auto testTextBGUITex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, testTextBGUITex.get(), "TextBGUI", L"resources/UI/[Test]TextBG2.dds");
	
	auto minimapTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, minimapTex.get(), "MiniMap", L"resources/UI/Minimap.dds");

	m_Textures[boxTex->m_Name] = std::move(boxTex);
	m_Textures[testPlayerUITex->m_Name] = std::move(testPlayerUITex);
	m_Textures[testTextBGUITex->m_Name] = std::move(testTextBGUITex); 
	m_Textures[minimapTex->m_Name] = std::move(minimapTex);
}

void CSceneJH2::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dSrvDescriptorHeap));

	//
	// Fill out the heap with actual descriptors.
	//
	D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = m_pd3dSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	auto boxTex = m_Textures["Box"]->m_pd3dResource;
	auto testPlayerUITex = m_Textures["PlayerTestUI"]->m_pd3dResource;
	auto testTextBGUITex = m_Textures["TextBGUI"]->m_pd3dResource;
	auto minimapTex = m_Textures["MiniMap"]->m_pd3dResource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = boxTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	pd3dDevice->CreateShaderResourceView(boxTex, &srvDesc, hDescriptor);

	// next descriptor 
	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = testPlayerUITex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(testPlayerUITex, &srvDesc, hDescriptor);

	// next descriptor 
	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = testTextBGUITex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(testTextBGUITex, &srvDesc, hDescriptor);

	// next descriptor 
	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = minimapTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(minimapTex, &srvDesc, hDescriptor);
}

void CSceneJH2::ReleaseObjects()
{
	m_ppObjects[1];

	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void CSceneJH2::Update(double elapsedTime)
{
	ProcessInput();
}

void CSceneJH2::AnimateObjects(float fTimeElapsed)
{
}

void CSceneJH2::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, tex);

	//씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다.
	for (int j = 0; j < m_nUIStartIndex; j++)
	{
		if (m_ppObjects[j])
			m_ppObjects[j]->Draw(pd3dCommandList, m_CurrentCamera);
	} 
}

void CSceneJH2::DrawUI(ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, tex); 
	*/

	if (m_DrawUI)
	{
		for (int j = m_nUIStartIndex; j < m_nObjects; j++)
		{
			if (m_ppObjects[j])
				m_ppObjects[j]->Draw(pd3dCommandList, m_CurrentCamera);
		} 
	}
}

void CSceneJH2::ProcessInput()
{
	if (m_CurrentCamera == nullptr) return;

	auto keyInput = GAME_INPUT;
	if (keyInput.KEY_W)
	{
		m_CurrentCamera->Walk(1.0f);
	}
	if (keyInput.KEY_A)
	{
		m_CurrentCamera->Strafe(-1.0f);
	}
	if (keyInput.KEY_S)
	{
		m_CurrentCamera->Walk(-1.0f);
	}
	if (keyInput.KEY_D)
	{
		m_CurrentCamera->Strafe(1.0f);
	}

	if (keyInput.KEY_1)
	{
		m_CurrentCamera = m_Cameras[0];
	}
	if (keyInput.KEY_2)
	{
		m_CurrentCamera = m_Cameras[1];
	}
	if (keyInput.KEY_3)
	{
		m_CurrentCamera = m_Cameras[2];
	}
	if (keyInput.KEY_4)
	{
		m_CurrentCamera = m_Cameras[3];
	}
	if (keyInput.KEY_5)
	{
		m_CurrentCamera = m_Cameras[4];
	}
	///////////////////////////////////////////////////////////////
	//
	if (keyInput.KEY_B)
	{
		m_DrawUI = true - m_DrawUI;
	}

	m_CurrentCamera->UpdateViewMatrix();
}

void CSceneJH2::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(CFramework::GetInstance().GetHWND());
}

void CSceneJH2::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CSceneJH2::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

		m_CurrentCamera->Pitch(dy);
		m_CurrentCamera->RotateY(dx);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void CSceneJH2::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* CSceneJH2::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = m_Textures.size();
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 17;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	 
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0; 
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; 

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

#pragma region sampler
	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
		(void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
} 
#pragma endregion

#pragma region SceneJH3 - About Terrain and Lights
CSceneJH3::CSceneJH3()
{
	m_pd3dGraphicsRootSignature = NULL;
}

CSceneJH3::~CSceneJH3()
{
	if (m_pd3dcbLight)
	{
		m_pd3dcbLight->Unmap(0, NULL);
		m_pd3dcbLight->Release();
	} 
}

void CSceneJH3::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	BuildMaterials(pd3dDevice, pd3dCommandList);
	BuildLights(pd3dDevice, pd3dCommandList);
	BuildObjects(pd3dDevice, pd3dCommandList);
}

void CSceneJH3::BuildCamera(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	int width, int height)
{
	int nCameras = 5;
	m_Cameras = new CCamera * [nCameras];
	for (int i = 0; i < nCameras; ++i)
	{
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, width, height, 1.0f, 5000.0f);
		pCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
		pCamera->SetScissorRect(0, 0, width, height);
		pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_Cameras[i] = pCamera;
	}
	m_Cameras[0]->SetPosition(0.0f, 300.0f, -150.0f);
	//m_Cameras[0]->RotateY(XMConvertToRadians(45));
	m_Cameras[0]->Pitch(XMConvertToRadians(65));
	m_Cameras[1]->SetPosition(1000.0f, 10.0f, -150.0f);
	m_Cameras[2]->SetPosition(-1000.0f, 10.0f, -150.0f);
	m_Cameras[3]->SetPosition(0.0f, 1010.0f, -150.0f);
	m_Cameras[4]->SetPosition(0.0f, -1010.0f, -150.0f);

	m_CurrentCamera = m_Cameras[0];
}

void CSceneJH3::BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto box = std::make_unique<CMaterial>();
	box->m_Name = "Box";
	box->m_MatCBIndex = 0;
	box->m_DiffuseSrvHeapIndex = 0;
	box->m_xmf4DiffuseAlbedo = XMFLOAT4(Colors::BurlyWood);
	box->m_xmf3FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	box->m_Roughness = 0.1f;
	box->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	auto terrain = std::make_unique<CMaterial>();
	terrain->m_Name = "Terrain";
	terrain->m_MatCBIndex = 1;
	terrain->m_DiffuseSrvHeapIndex = 2;
	terrain->m_xmf4DiffuseAlbedo = XMFLOAT4(Colors::DarkGreen);
	terrain->m_xmf3FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	terrain->m_Roughness = 0.2f;
	terrain->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_Materials["Box"] = std::move(box);
	m_Materials["Terrain"] = std::move(terrain);
}

void CSceneJH3::BuildLights(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_Light = new LightInfo();
	m_Light->AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	m_Light->Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	m_Light->Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	m_Light->Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	m_Light->Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	m_Light->Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	m_Light->Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	UINT ncbElementBytes = ((sizeof(LightInfo) + 255) & ~255); //256의 배수
	m_pd3dcbLight = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, 
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	 
	m_pd3dcbLight->Map(0, NULL, (void**)&m_pcbMappedLight);

	::memcpy(m_pcbMappedLight, m_Light, sizeof(LightInfo)); 
}

void CSceneJH3::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CPlaneMeshTextured* pPlaneMeshTex = new CPlaneMeshTextured(pd3dDevice, pd3dCommandList, 200.0f, 200.0f, 0.0f, false);

	m_nObjects = 20;
	m_ppObjects = new CGameObject * [m_nObjects];

	CShader* pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders/[TestJH]TerrainAndLight.hlsl", "VSTextured");
	pShader->CreatePixelShader(L"Shaders/[TestJH]TerrainAndLight.hlsl", "PSTexIndex");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	CShader* pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateVertexShader(L"Shaders/[TestJH]TerrainAndLight.hlsl", "VSTextured");
	pSkyBoxShader->CreatePixelShader(L"Shaders/[TestJH]TerrainAndLight.hlsl", "PSTexIndex");
	pSkyBoxShader->CreateInputLayout(ShaderTypes::Textured);
	pSkyBoxShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	// 추가 스카이박스 
	CTexturedRectMesh* pSkyBoxMesh_Front = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Back = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	CTexturedRectMesh* pSkyBoxMesh_Left = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Right = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, 10.0f, 0.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Top = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f, +10.0f, +0.0f);
	CTexturedRectMesh* pSkyBoxMesh_Bottom = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 00.0f, 20.0f, 0.0f, -10.0f, +0.0f);

	for (int i = 0; i < 15; ++i)
	{
		CRotatingObject* pObject = new CRotatingObject();
		
		if (i > 5)  pObject->SetObjectName(OBJ_NAME::Terrain); 
		m_ppObjects[i] = pObject; 
	}
	// 스카이박스
	m_ppObjects[0]->SetMesh(pSkyBoxMesh_Front);
	m_ppObjects[0]->SetTextureIndex(0x02);
	m_ppObjects[0]->SetShader(pSkyBoxShader);

	m_ppObjects[1]->SetMesh(pSkyBoxMesh_Back);
	m_ppObjects[1]->SetTextureIndex(0x04);
	m_ppObjects[1]->SetShader(pSkyBoxShader);

	m_ppObjects[2]->SetMesh(pSkyBoxMesh_Left);
	m_ppObjects[2]->SetTextureIndex(0x08);
	m_ppObjects[2]->SetShader(pSkyBoxShader);

	m_ppObjects[3]->SetMesh(pSkyBoxMesh_Right);
	m_ppObjects[3]->SetTextureIndex(0x10);
	m_ppObjects[3]->SetShader(pSkyBoxShader);

	m_ppObjects[4]->SetMesh(pSkyBoxMesh_Top);
	m_ppObjects[4]->SetTextureIndex(0x20);
	m_ppObjects[4]->SetShader(pSkyBoxShader);

	m_ppObjects[5]->SetMesh(pSkyBoxMesh_Bottom);
	m_ppObjects[5]->SetTextureIndex(0x40);
	m_ppObjects[5]->SetShader(pSkyBoxShader);

	// 지형
	m_ppObjects[6]->SetMesh(pPlaneMeshTex);
	m_ppObjects[6]->SetObjectName(OBJ_NAME::Terrain);
	m_ppObjects[6]->SetMaterial(m_Materials[m_ppObjects[6]->GetObjectName()].get(), 2);
	m_ppObjects[6]->SetPosition({ 0,  0,  0 });
	m_ppObjects[6]->SetTextureIndex(0x01);
	m_ppObjects[6]->SetShader(pShader);
	
	m_ppObjects[7]->SetMesh(pPlaneMeshTex);
	m_ppObjects[7]->SetPosition({ 0,  0, 200 });
	m_ppObjects[7]->SetTextureIndex(0x01);
	m_ppObjects[7]->SetShader(pShader);

	m_ppObjects[8]->SetMesh(pPlaneMeshTex);
	m_ppObjects[8]->SetPosition({ 0,  0, -200 });
	m_ppObjects[8]->SetTextureIndex(0x01);
	m_ppObjects[8]->SetShader(pShader);

	m_ppObjects[9]->SetMesh(pPlaneMeshTex);
	m_ppObjects[9]->SetPosition({ 200,  0, 0 });
	m_ppObjects[9]->SetTextureIndex(0x01);
	m_ppObjects[9]->SetShader(pShader);

	m_ppObjects[10]->SetMesh(pPlaneMeshTex);
	m_ppObjects[10]->SetPosition({ -200,  0, 0 });
	m_ppObjects[10]->SetTextureIndex(0x01);
	m_ppObjects[10]->SetShader(pShader);

	m_ppObjects[11]->SetMesh(pPlaneMeshTex);
	m_ppObjects[11]->SetPosition({ -200,  0, -200 });
	m_ppObjects[11]->SetTextureIndex(0x01);
	m_ppObjects[11]->SetShader(pShader);

	m_ppObjects[12]->SetMesh(pPlaneMeshTex);
	m_ppObjects[12]->SetPosition({ -200,  0, 200 });
	m_ppObjects[12]->SetTextureIndex(0x01);
	m_ppObjects[12]->SetShader(pShader);

	m_ppObjects[13]->SetMesh(pPlaneMeshTex);
	m_ppObjects[13]->SetPosition({ 200,  0, 200 });
	m_ppObjects[13]->SetTextureIndex(0x01);
	m_ppObjects[13]->SetShader(pShader);

	m_ppObjects[14]->SetMesh(pPlaneMeshTex);
	m_ppObjects[14]->SetPosition({ 200,  0, -200 });
	m_ppObjects[14]->SetTextureIndex(0x01);
	m_ppObjects[14]->SetShader(pShader);

	pShader = new CShader();
	pShader->CreateVertexShader(L"Shaders/[TestJH]TerrainAndLight.hlsl", "VSTextured");
	pShader->CreatePixelShader(L"Shaders/[TestJH]TerrainAndLight.hlsl", "PSTextured");
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	 
	for(int i = 15; i < 20; ++i)
	{
		CBox* pBox = new CBox(pd3dDevice, pd3dCommandList, 50.0f, 50.0f, 50.0f);
		pBox->SetShader(pShader);
		pBox->SetObjectName(OBJ_NAME::Box);
		pBox->SetMaterial(m_Materials[pBox->GetObjectName()].get(), 2);
		m_ppObjects[i] = pBox;
	}

	m_ppObjects[15]->SetPosition({    0,  25,    0 });
	m_ppObjects[16]->SetPosition({ -100,  25,    0 });
	m_ppObjects[17]->SetPosition({  100,  25,    0 });
	m_ppObjects[18]->SetPosition({    0,  25,  100 });
	m_ppObjects[19]->SetPosition({    0,  25, -100 }); 
}

void CSceneJH3::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto boxTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, boxTex.get(), "Box", L"resources/OBJ/Box.dds");

	auto terrainTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, terrainTex.get(), "Terrain", L"resources/OBJ/rockygrass00.dds");

	auto SkyTex_Front = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Front.get(), "Sky_Front", L"resources/OBJ/SkyBox_Front_0.dds");

	auto SkyTex_Back = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Back.get(), "Sky_Back", L"resources/OBJ/SkyBox_Back_0.dds");

	auto SkyTex_Left = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Left.get(), "Sky_Left", L"resources/OBJ/SkyBox_Left_0.dds");

	auto SkyTex_Right = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Right.get(), "Sky_Right", L"resources/OBJ/SkyBox_Right_0.dds");

	auto SkyTex_Top = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Top.get(), "Sky_Top", L"resources/OBJ/SkyBox_Top_0.dds");

	auto SkyTex_Bottom = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, SkyTex_Bottom.get(), "Sky_Bottom", L"resources/OBJ/SkyBox_Bottom_0.dds");

	m_Textures[boxTex->m_Name] = std::move(boxTex);
	m_Textures[terrainTex->m_Name] = std::move(terrainTex);
	m_Textures[SkyTex_Back->m_Name] = std::move(SkyTex_Back);
	m_Textures[SkyTex_Front->m_Name] = std::move(SkyTex_Front);
	m_Textures[SkyTex_Left->m_Name] = std::move(SkyTex_Left);
	m_Textures[SkyTex_Right->m_Name] = std::move(SkyTex_Right);
	m_Textures[SkyTex_Top->m_Name] = std::move(SkyTex_Top);
	m_Textures[SkyTex_Bottom->m_Name] = std::move(SkyTex_Bottom);
}

void CSceneJH3::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dSrvDescriptorHeap));

	//
	// Fill out the heap with actual descriptors.
	//
	D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = m_pd3dSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	auto boxTex = m_Textures["Box"]->m_pd3dResource;
	auto TerrainTex = m_Textures["Terrain"]->m_pd3dResource;
	auto SkyTex_Front = m_Textures["Sky_Front"]->m_pd3dResource;
	auto SkyTex_Back = m_Textures["Sky_Back"]->m_pd3dResource;
	auto SkyTex_Left = m_Textures["Sky_Left"]->m_pd3dResource;
	auto SkyTex_Right = m_Textures["Sky_Right"]->m_pd3dResource;
	auto SkyTex_Top = m_Textures["Sky_Top"]->m_pd3dResource;
	auto SkyTex_Bottom = m_Textures["Sky_Bottom"]->m_pd3dResource;
	 
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TerrainTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	pd3dDevice->CreateShaderResourceView(TerrainTex, &srvDesc, hDescriptor);
	 
	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Front->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Front, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Back->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Back, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Left->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Left, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Right->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Right, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Top->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Top, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = SkyTex_Bottom->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(SkyTex_Bottom, &srvDesc, hDescriptor);

	hDescriptor.ptr += gnCbvSrvDescriptorIncrementSize;
	srvDesc.Format = boxTex->GetDesc().Format;
	pd3dDevice->CreateShaderResourceView(boxTex, &srvDesc, hDescriptor);
}

void CSceneJH3::ReleaseObjects()
{
	m_ppObjects[1];

	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void CSceneJH3::Update(double elapsedTime)
{
	ProcessInput();
}

void CSceneJH3::AnimateObjects(float fTimeElapsed)
{
}

void CSceneJH3::Draw(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT3 xmf3CameraPos = m_CurrentCamera->GetPosition3f();

	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (m_CurrentCamera)
	{
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList, 1);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(4, tex);

	for (int i = 0; i < 6; i++)
	{
		m_ppObjects[i]->SetPosition(xmf3CameraPos);
	}

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbLight->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dGpuVirtualAddress);

	//씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다.
	for (int j = 0; j < m_nObjects; j++)
	{ 
		if (m_ppObjects[j])
			m_ppObjects[j]->Draw(pd3dCommandList, m_CurrentCamera);
	}
}

void CSceneJH3::ProcessInput()
{
	if (m_CurrentCamera == nullptr) return;

	auto keyInput = GAME_INPUT;
	if (keyInput.KEY_W)
	{
		m_CurrentCamera->Walk(1.0f);
	}
	if (keyInput.KEY_A)
	{
		m_CurrentCamera->Strafe(-1.0f);
	}
	if (keyInput.KEY_S)
	{
		m_CurrentCamera->Walk(-1.0f);
	}
	if (keyInput.KEY_D)
	{
		m_CurrentCamera->Strafe(1.0f);
	}

	if (keyInput.KEY_1)
	{
		m_CurrentCamera = m_Cameras[0];
	}
	if (keyInput.KEY_2)
	{
		m_CurrentCamera = m_Cameras[1];
	}
	if (keyInput.KEY_3)
	{
		m_CurrentCamera = m_Cameras[2];
	}
	if (keyInput.KEY_4)
	{
		m_CurrentCamera = m_Cameras[3];
	}
	if (keyInput.KEY_5)
	{
		m_CurrentCamera = m_Cameras[4];
	}

	m_CurrentCamera->UpdateViewMatrix();
}

void CSceneJH3::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(CFramework::GetInstance().GetHWND());
}

void CSceneJH3::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CSceneJH3::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

		m_CurrentCamera->Pitch(dy);
		m_CurrentCamera->RotateY(dx);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void CSceneJH3::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
			if (m_ppObjects[j])
				m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* CSceneJH3::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	// Ground
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = m_Textures.size();
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 17;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; //Material
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Light
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; 

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	 
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

#pragma region sampler
	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
		(void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

#pragma endregion