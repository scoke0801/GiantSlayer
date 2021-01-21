#include "stdafx.h"
#include "SceneJH.h"
#include "stdafx.h" 
#include "GameFramework.h"
#include "Shader.h"

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

	m_Textures[multiButtonTex->m_Name]  = std::move(multiButtonTex);
	m_Textures[singleButtonTex->m_Name] = std::move(singleButtonTex);
	m_Textures[titleTex->m_Name]		= std::move(titleTex); 
	m_Textures[minimapTex->m_Name]		= std::move(minimapTex);
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

	auto multiBtnTex  = m_Textures["MultiButton"]->m_pd3dResource;
	auto simpleBtnTex = m_Textures["SingleButton"]->m_pd3dResource;
	auto titleTex	  = m_Textures["Title"]->m_pd3dResource;
	auto minimapTex	  = m_Textures["MiniMap"]->m_pd3dResource;

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
		vertices[i * 3].xmf3Position	 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertices[i * 3 + 1].xmf3Position = XMFLOAT3(std::cos(GetRadian((i + 1) * angle)) * radius, std::sin(GetRadian(i + 1) * angle) * radius, 0.0f);
		vertices[i * 3 + 2].xmf3Position = XMFLOAT3(std::cos(GetRadian(i * angle)) * radius, std::sin(GetRadian(i * angle)) * radius, 0.0f);
		  
		vertices[i * 3].m_xmf2TexC		 = XMFLOAT2(0.5f, 0.5f);
		vertices[i * 3 + 1].m_xmf2TexC	 = XMFLOAT2(std::cos(GetRadian((i + 1) * angle)) * 0.5 + 0.5f, 1 -( std::sin(GetRadian(i + 1) * angle) * 0.5f + 0.5f));
		vertices[i * 3 + 2].m_xmf2TexC   = XMFLOAT2(std::cos(GetRadian(i * angle)) * 0.5f + 0.5f,      1-( std::sin(GetRadian(i * angle)) * 0.5f + 0.5f));
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
  
CSceneJH2::CSceneJH2()
{
	m_pd3dGraphicsRootSignature = NULL;
}
CSceneJH2::~CSceneJH2()
{

}

void CSceneJH2::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	
	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);
 
	BuildObjects(pd3dDevice, pd3dCommandList); 
}

void CSceneJH2::BuildCamera(int width, int height)
{
	int nCameras = 5; 
	m_Cameras = new CCamera * [nCameras];
	for (int i = 0; i < nCameras; ++i)
	{
		CCamera* pCamera = new CCamera;
		pCamera->SetLens(0.25f * PI, width, height, 1.0f, 5000.0f); 
		pCamera->SetViewport(0, 0, width, height, 0.0f, 1.0f);
		pCamera->SetScissorRect(0, 0, width, height);
		m_Cameras[i] = pCamera;
	}
	m_Cameras[0]->SetPosition(    0.0f,    10.0f, -150.0f);
	m_Cameras[1]->SetPosition( 1000.0f,    10.0f, -150.0f);
	m_Cameras[2]->SetPosition(-1000.0f,    10.0f, -150.0f);
	m_Cameras[3]->SetPosition(	  0.0f,	 1010.0f, -150.0f);
	m_Cameras[4]->SetPosition(	  0.0f,	-1010.0f, -150.0f); 

	m_CurrentCamera = m_Cameras[0];
}

void CSceneJH2::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//가로x세로x깊이가 12x12x12인 정육면체 메쉬를 생성한다.
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		50.0f, 50.0f, 50.0f);

	CCubeMeshTextured* pCubeMeshTex = new CCubeMeshTextured(pd3dDevice, pd3dCommandList,
		50.0f, 50.0f, 50.0f);

	m_nObjects = 5;
	m_ppObjects = new CGameObject * [m_nObjects];

	CDiffusedShader* pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CGeneralShader* pGeneralShader = new CGeneralShader();
	pGeneralShader->CreateVertexShader(L"Shaders/JHTestShader.hlsl", "VSTextured");
	pGeneralShader->CreatePixelShader(L"Shaders/JHTestShader.hlsl", "PSTextured");
	pGeneralShader->CreateInputLayout(0);
	pGeneralShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	for (int i = 0; i < m_nObjects; ++i)
	{
		CRotatingObject* pObject = new CRotatingObject();
		//pRotatingObject->SetMesh(pCubeMesh);
		//pRotatingObject->SetShader(pShader);

		pObject->SetMesh(pCubeMeshTex);
		pObject->SetShader(pGeneralShader);

		m_ppObjects[i] = pObject;
	}

	m_ppObjects[0]->SetPosition({     0,     0,  0 });
	m_ppObjects[1]->SetPosition({  1000,     0,  0 });
	m_ppObjects[2]->SetPosition({ -1000,     0,  0 });
	m_ppObjects[3]->SetPosition({	  0,  1000,  0 });
	m_ppObjects[4]->SetPosition({	  0, -1000,  0 });
}

void CSceneJH2::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto boxTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, boxTex.get(), "Box", L"resources/OBJ/Box.dds");
	 
	m_Textures[boxTex->m_Name] = std::move(boxTex); 
}

void CSceneJH2::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{	
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dSrvDescriptorHeap));

	//
	// Fill out the heap with actual descriptors.
	//
	D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = m_pd3dSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	auto boxTex = m_Textures["Box"]->m_pd3dResource; 

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = boxTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	pd3dDevice->CreateShaderResourceView(boxTex, &srvDesc, hDescriptor); 
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
		m_CurrentCamera->UpdateShaderVariables(pd3dCommandList);
		m_CurrentCamera->SetViewportsAndScissorRects(pd3dCommandList);
	} 

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, tex);

	//씬을 렌더링하는 것은 씬을 구성하는 게임 객체(셰이더를 포함하는 객체)들을 렌더링하는 것이다.
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
			m_ppObjects[j]->Draw(pd3dCommandList, m_CurrentCamera);
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
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

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
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
		(void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

