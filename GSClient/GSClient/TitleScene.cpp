#include "stdafx.h"
#include "TitleScene.h"
#include "GameFramework.h" 

CTitleScene::CTitleScene()
{
	m_pd3dGraphicsRootSignature = NULL;
	m_pd3dPipelineState = NULL;
}

CTitleScene::~CTitleScene()
{
}

void CTitleScene::Update(double elapsedTime)
{
	POINT mousePos = GET_MOUSE_POS;

	if ((mousePos.x >= 128 && mousePos.x <= 384)
		&&(mousePos.y >= 508 && mousePos.y <= 585))
	{
		cout << "멀티 플레이\n";
		CInputHandler::GetInstance().ResetMousePos();
	}
	else if ((mousePos.x >= 639 && mousePos.x <= 896)
		&& (mousePos.y >= 508 && mousePos.y <= 585))
	{
		cout << "싱글 플레이\n"; 
		CInputHandler::GetInstance().ResetMousePos();
	}
	//m_pcbMappedTestData->MouseClikced = CInputHandler::GetInstance().TestingMouseClick;
	//
	//D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dTestData->GetGPUVirtualAddress();
	//
	//pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

void CTitleScene::Draw(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{		
	// 그래픽 루트 시그너쳐를 설정한다.
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	// 파이프라인 상태를 설정한다.
	pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
	
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pd3dSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE tex = m_pd3dSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	pd3dCommandList->SetGraphicsRootDescriptorTable(1, tex);

	m_pcbMappedTestData->MouseClikced = CInputHandler::GetInstance().TestingMouseClick;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dTestData->GetGPUVirtualAddress();

	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
	 
	// 프리미티브 토폴로지를 설정한다.
	pd3dCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
	// 정점 6개를 사용하여 렌더링 한다.
	pd3dCommandList->DrawInstanced(18, 1, 0, 0);
}

void CTitleScene::ProcessInput()
{
}

void CTitleScene::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CreateRootSignature(pd3dDevice, pd3dCommandList);
	CreatePipelineState(pd3dDevice, pd3dCommandList);

	LoadTextures(pd3dDevice, pd3dCommandList);
	BuildDescripotrHeaps(pd3dDevice, pd3dCommandList);
	BuildObjects(pd3dDevice, pd3dCommandList); 
}

void CTitleScene::CreateRootSignature(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 3;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; // test
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);	
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

void CTitleScene::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{	
	// 정점 셰이더와 픽셸 셰이더를 생성한다.
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;

	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 
	HRESULT hRes = D3DCompileFromFile(L"TitleScene.hlsl", NULL, NULL,
		"VSTextured", "vs_5_1", nCompileFlags, 0, &pd3dVertexShaderBlob, NULL);
	
	hRes = D3DCompileFromFile(L"TitleScene.hlsl", NULL, NULL,
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
		IID_PPV_ARGS(&m_pd3dPipelineState));

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

void CTitleScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	BuildConstantsBuffers(pd3dDevice, pd3dCommandList);
}

void CTitleScene::BuildConstantsBuffers(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = CalcCBufferSize(sizeof(TestData));
	::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dTestData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dTestData->Map(0, NULL, (void**)&m_pcbMappedTestData);
}

void CTitleScene::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto multiButtonTex = make_unique<CTexture>();
	MakeTexture(pd3dDevice, pd3dCommandList, multiButtonTex.get(), "MultiButton", L"resources/UI/MultiPlayButton.dds");

	auto singleButtonTex = make_unique<CTexture>(); 
	MakeTexture(pd3dDevice, pd3dCommandList, singleButtonTex.get(), "SingleButton", L"resources/UI/SinglePlayButton.dds");
 
	auto titleTex = make_unique<CTexture>();	
	MakeTexture(pd3dDevice, pd3dCommandList, titleTex.get(), "Title", L"resources/UI/TitleTest.dds");

	m_Textures[multiButtonTex->m_Name] = std::move(multiButtonTex);
	m_Textures[singleButtonTex->m_Name] = std::move(singleButtonTex); 
	m_Textures[titleTex->m_Name] = std::move(titleTex);
}

void CTitleScene::BuildDescripotrHeaps(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{	
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pd3dSrvDescriptorHeap));
	
	//
	// Fill out the heap with actual descriptors.
	//
	D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = m_pd3dSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	 
	auto multiBtnTex	= m_Textures["MultiButton"]->m_pd3dResource;
	auto simpleBtnTex   = m_Textures["SingleButton"]->m_pd3dResource; 
	auto titleTex		= m_Textures["Title"]->m_pd3dResource;

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
}

void CTitleScene::ConnectToServer()
{
	PrepareCommunicate();
}

bool CTitleScene::PrepareCommunicate()
{
	//CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
	int retval = 0;
	// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0) return false;

	// set serveraddr
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	m_IsServerConnected = true;

	// socket()
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Sock == INVALID_SOCKET)
	{
		m_IsServerConnected = false; 
		return false;
	}
	int opt_val = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val)); 
	retval = connect(m_Sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		m_IsServerConnected = false; 
		return false;
	}

	return true;
}
