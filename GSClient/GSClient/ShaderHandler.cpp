#include "stdafx.h"
#include "ShaderHandler.h"
#include "Shader.h"

void CShaderHandler::CreateAllShaders(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CreateFBXShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateFBXAnimatedShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateBasicObjectShader(pd3dDevice,pd3dGraphicsRootSignature);


	CreateDoorWallShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateBridgeShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreatePuzzleShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateSignShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateTreeShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreatePlayerShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateTerrainShader(pd3dDevice, pd3dGraphicsRootSignature);
	//CreateShadowShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreateBillboardShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreateMirrorShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreateUiShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreateMinmapShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreateSkyboxShader(pd3dDevice, pd3dGraphicsRootSignature); 
	CreateTerrainWaterShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreateParticleShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateEffectShader(pd3dDevice, pd3dGraphicsRootSignature);

	CreateStandardShader(pd3dDevice, pd3dGraphicsRootSignature);
	CreateSkinnedShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CShaderHandler::CreateFBXShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pFBXShader = new CShader(); 
	CShader* pFBXFeatureShaderLeft = new CShader();	
	CShader* pFBXFeatureShaderRight = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pFBXShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pFBXShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTexturedLighting"); 

		pFBXFeatureShaderLeft->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pFBXFeatureShaderLeft->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSFBXFeatureShader");

		pFBXFeatureShaderRight->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pFBXFeatureShaderRight->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSFBXFeatureShader");
	}
	else if (m_UserID == ShaderHandlerUser::YJ){
		pFBXShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pFBXShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSTexturedLighting"); 

		pFBXFeatureShaderLeft->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pFBXFeatureShaderLeft->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSFBXFeatureShader");
		
		pFBXFeatureShaderRight->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pFBXFeatureShaderRight->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSFBXFeatureShader");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pFBXShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pFBXShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSTexturedLighting");

		pFBXFeatureShaderLeft->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pFBXFeatureShaderLeft->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSFBXFeatureShader");

		pFBXFeatureShaderRight->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pFBXFeatureShaderRight->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSFBXFeatureShader");
	}

	pFBXShader->CreateInputLayout(ShaderTypes::Textured);
	pFBXShader->CreateFBXMeshShader(pd3dDevice, pd3dGraphicsRootSignature);
	pFBXShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("FBX", pFBXShader);
	 
	pFBXFeatureShaderLeft->CreateInputLayout(ShaderTypes::Textured);
	pFBXFeatureShaderLeft->CreateFBXMeshShader(pd3dDevice, pd3dGraphicsRootSignature);
	pFBXFeatureShaderLeft->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("FBXFeatureLeft", pFBXFeatureShaderLeft);

	pFBXFeatureShaderRight->CreateInputLayout(ShaderTypes::Textured);
	pFBXFeatureShaderRight->CreateFBXMeshShader(pd3dDevice, pd3dGraphicsRootSignature, false);
	pFBXFeatureShaderRight->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("FBXFeatureRight", pFBXFeatureShaderRight); 
}

void CShaderHandler::CreateFBXAnimatedShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pFBXShader = new CShader();
	CShader* pFBXFeatureShaderLeft = new CShader();
	CShader* pFBXFeatureShaderRight = new CShader();

	//if (m_UserID == ShaderHandlerUser::TH) 
	{
		pFBXShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSFbxAnimated");
		pFBXShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSFbxAnimated");
	}

	pFBXShader->CreateInputLayout(ShaderTypes::FbxAnimated);
	pFBXShader->CreateFBXMeshShader(pd3dDevice, pd3dGraphicsRootSignature);
	pFBXShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("FbxAnimated", pFBXShader);
}

void CShaderHandler::CreateUiShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pUiShader = new CShader();
	CShader* pUiHelpTextShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pUiShader->CreateVertexShader(L"Shaders/ShaderJH.hlsl", "VS_UI_Textured");
		pUiShader->CreatePixelShader(L"Shaders/ShaderJH.hlsl", "PS_UI_Textured");
		
		pUiHelpTextShader->CreateVertexShader(L"Shaders/ShaderJH.hlsl", "VS_UI_Textured");
		pUiHelpTextShader->CreatePixelShader(L"Shaders/ShaderJH.hlsl", "PS_UI_HelpText");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pUiShader->CreateVertexShader(L"Shaders/ShaderYJ.hlsl", "VS_UI_Textured");
		pUiShader->CreatePixelShader(L"Shaders/ShaderYJ.hlsl", "PS_UI_Textured");	
		
		pUiHelpTextShader->CreateVertexShader(L"Shaders/ShaderYJ.hlsl", "VS_UI_Textured");
		pUiHelpTextShader->CreatePixelShader(L"Shaders/ShaderYJ.hlsl", "PS_UI_HelpText");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pUiShader->CreateVertexShader(L"Shaders/ShaderTH.hlsl", "VS_UI_Textured");
		pUiShader->CreatePixelShader(L"Shaders/ShaderTH.hlsl", "PS_UI_Textured");

		pUiHelpTextShader->CreateVertexShader(L"Shaders/ShaderTH.hlsl", "VS_UI_Textured");
		pUiHelpTextShader->CreatePixelShader(L"Shaders/ShaderTH.hlsl", "PS_UI_HelpText");
	}
	pUiShader->CreateInputLayout(ShaderTypes::Textured);
	pUiShader->CreateUIShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Ui", pUiShader);
	 
	pUiHelpTextShader->CreateInputLayout(ShaderTypes::Textured);
	pUiHelpTextShader->CreateUIShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("UiHelpText", pUiHelpTextShader);
}

void CShaderHandler::CreateShadowShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pShadowShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pShadowShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSStandardShadow");
		pShadowShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSStandardShadow");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pShadowShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSStandardShadow");
		pShadowShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSStandardShadow");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pShadowShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSStandardShadow");
		pShadowShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSStandardShadow");
	}
	pShadowShader->CreateInputLayout(ShaderTypes::Shadow);
	pShadowShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, TRUE);
	m_Data.emplace("Shadow", pShadowShader);
}



void CShaderHandler::CreatePlayerShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pPlayerShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pPlayerShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pPlayerShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTexturedLighting");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pPlayerShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pPlayerShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSTexturedLighting");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pPlayerShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pPlayerShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSTexturedLighting");
	}
	pPlayerShader->CreateInputLayout(ShaderTypes::Textured);
	pPlayerShader->CreateFBXMeshShader(pd3dDevice, pd3dGraphicsRootSignature);
	pPlayerShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Player", pPlayerShader);
}

void CShaderHandler::CreateMinmapShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader*  pMinimapShader = new CShader(); 
	if (m_UserID == ShaderHandlerUser::JH) {
		pMinimapShader->CreateVertexShader(L"Shaders/ShaderJH.hlsl", "VSMinimap");
		pMinimapShader->CreatePixelShader(L"Shaders/ShaderJH.hlsl", "PSMinimap");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pMinimapShader->CreateVertexShader(L"Shaders/ShaderYJ.hlsl", "VSMinimap");
		pMinimapShader->CreatePixelShader(L"Shaders/ShaderYJ.hlsl", "PSMinimap");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pMinimapShader->CreateVertexShader(L"Shaders/ShaderTH.hlsl", "VSMinimap");
		pMinimapShader->CreatePixelShader(L"Shaders/ShaderTH.hlsl", "PSMinimap");
	}
	pMinimapShader->CreateInputLayout(ShaderTypes::Textured);
	pMinimapShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Minimap", pMinimapShader);
}

void CShaderHandler::CreateSkyboxShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pSkyBoxShader = new CSkyBoxShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pSkyBoxShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTextured");
		pSkyBoxShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTextured");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pSkyBoxShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTextured");
		pSkyBoxShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSTextured");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pSkyBoxShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTextured");
		pSkyBoxShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSTextured");
	}
	pSkyBoxShader->CreateInputLayout(ShaderTypes::Textured);
	pSkyBoxShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("SkyBox", pSkyBoxShader);
}

void CShaderHandler::CreateTerrainShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pTerrainShader = new CTerrainTessellationShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pTerrainShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTerrainTessellation");
		pTerrainShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTerrainTessellation");
		pTerrainShader->CreateDomainShader(L"Shaders\\ShaderJH.hlsl", "DSTerrainTessellation");
		pTerrainShader->CreateHullShader(L"Shaders\\ShaderJH.hlsl", "HSTerrainTessellation");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pTerrainShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTerrainTessellation");
		pTerrainShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSTerrainTessellation");
		pTerrainShader->CreateDomainShader(L"Shaders\\ShaderYJ.hlsl", "DSTerrainTessellation");
		pTerrainShader->CreateHullShader(L"Shaders\\ShaderYJ.hlsl", "HSTerrainTessellation");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pTerrainShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTerrainTessellation");
		pTerrainShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSTerrainTessellation");
		pTerrainShader->CreateDomainShader(L"Shaders\\ShaderTH.hlsl", "DSTerrainTessellation");
		pTerrainShader->CreateHullShader(L"Shaders\\ShaderTH.hlsl", "HSTerrainTessellation");
	}
	pTerrainShader->CreateInputLayout(ShaderTypes::Terrain);
	pTerrainShader->CreateTerrainShader(pd3dDevice, pd3dGraphicsRootSignature); 
	m_Data.emplace("Terrain", pTerrainShader);
}

void CShaderHandler::CreateTerrainWaterShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pTerrainWaterShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pTerrainWaterShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTerrainWater");
		pTerrainWaterShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTerrainWater"); 
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pTerrainWaterShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTerrainWater");
		pTerrainWaterShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTerrainWater");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pTerrainWaterShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTerrainWater");
		pTerrainWaterShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTerrainWater");
	}
	pTerrainWaterShader->CreateInputLayout(ShaderTypes::TerrainWater);
	pTerrainWaterShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		false, true);
	m_Data.emplace("TerrainWater", pTerrainWaterShader);
}

void CShaderHandler::CreateParticleShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pParticleShader = new CShader();
	pParticleShader->CreateVertexShader(L"Shaders\\ParticleShader.hlsl", "VSArrowParticle");
	pParticleShader->CreatePixelShader(L"Shaders\\ParticleShader.hlsl", "PSParticle");

	pParticleShader->CreateInputLayout(ShaderTypes::Particle);
	pParticleShader->CreateParticleShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("ArrowParticle", pParticleShader);

	pParticleShader = new CShader();
	pParticleShader->CreateVertexShader(L"Shaders\\ParticleShader.hlsl", "VSTexParticle");
	pParticleShader->CreatePixelShader(L"Shaders\\ParticleShader.hlsl", "PSTexParticle");

	pParticleShader->CreateInputLayout(ShaderTypes::TexParticle);
	pParticleShader->CreateParticleShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("TexParticle", pParticleShader);

	pParticleShader = new CShader();
	pParticleShader->CreateVertexShader(L"Shaders\\ParticleShader.hlsl", "VSFogParticle");
	pParticleShader->CreatePixelShader(L"Shaders\\ParticleShader.hlsl", "PSFogParticle");

	pParticleShader->CreateInputLayout(ShaderTypes::Particle);
	pParticleShader->CreateParticleShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("FogParticle", pParticleShader);

	pParticleShader = new CShader();
	pParticleShader->CreateVertexShader(L"Shaders\\ParticleShader.hlsl", "VSRainParticle");
	pParticleShader->CreatePixelShader(L"Shaders\\ParticleShader.hlsl", "PSTexParticle");

	pParticleShader->CreateInputLayout(ShaderTypes::TexParticle);
	pParticleShader->CreateParticleShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("RainParticle", pParticleShader);

}

void CShaderHandler::CreateEffectShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pEffectShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pEffectShader->CreateVertexShader(L"Shaders/ShaderJH.hlsl", "VSEffect");
		pEffectShader->CreatePixelShader(L"Shaders/ShaderJH.hlsl", "PSEffect"); 
		//pEffectShader->CreateGeometryShader(L"Shaders/ShaderJH.hlsl", "GSEffect");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		//pEffectShader->CreateVertexShader(L"Shaders/ShaderYJ.hlsl", "VSMinimap");
		//pEffectShader->CreatePixelShader(L"Shaders/ShaderYJ.hlsl", "PSMinimap");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		//pEffectShader->CreateVertexShader(L"Shaders/ShaderTH.hlsl", "VSMinimap");
		//pEffectShader->CreatePixelShader(L"Shaders/ShaderTH.hlsl", "PSMinimap");
	}
	pEffectShader->CreateInputLayout(ShaderTypes::Effect);
	pEffectShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, true, true);
	m_Data.emplace("Effect", pEffectShader);
}

void CShaderHandler::CreateBasicObjectShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pShader = new CShader();	
	if (m_UserID == ShaderHandlerUser::JH) {
		pShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSTexturedLighting");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSTexturedLighting");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSTexturedLighting");
	}
	pShader->CreateInputLayout(ShaderTypes::Textured);
	pShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Object", pShader);
}

void CShaderHandler::CreateStandardShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pStandardShader = new CShader();

	if (m_UserID == ShaderHandlerUser::JH) {
		pStandardShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSStandard");
		pStandardShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSStandard");
	}
	else
	{
		pStandardShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSStandard");
		pStandardShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSStandard");
	}

	pStandardShader->CreateInputLayout(ShaderTypes::Standard);
	pStandardShader->CreateFBXMeshShader(pd3dDevice, pd3dGraphicsRootSignature, false);
	pStandardShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Standard", pStandardShader);
}

void CShaderHandler::CreateSkinnedShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pSkinnedShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pSkinnedShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSSkinnedAnimationStandard");
		pSkinnedShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSStandard");
	}
	else
	{
		pSkinnedShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSSkinnedAnimationStandard");
		pSkinnedShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSStandard");
	}

	pSkinnedShader->CreateInputLayout(ShaderTypes::Skinned);
	pSkinnedShader->CreateFBXMeshShader(pd3dDevice, pd3dGraphicsRootSignature, false);
	pSkinnedShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Skinned", pSkinnedShader);
}

void CShaderHandler::CreateBillboardShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pBillboardShader = new CBillboardShader();	
	if (m_UserID == ShaderHandlerUser::JH) {
		pBillboardShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSBillboard");
		pBillboardShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSBillboard");
		pBillboardShader->CreateGeometryShader(L"Shaders\\ShaderJH.hlsl", "GSBillboard");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pBillboardShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSBillboard");
		pBillboardShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSBillboard");
		pBillboardShader->CreateGeometryShader(L"Shaders\\ShaderYJ.hlsl", "GSBillboard");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pBillboardShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSBillboard");
		pBillboardShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSBillboard");
		pBillboardShader->CreateGeometryShader(L"Shaders\\ShaderTH.hlsl", "GSBillboard");
	}
	pBillboardShader->CreateInputLayout(ShaderTypes::Billboard);
	pBillboardShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);	
	m_Data.emplace("Billboard", pBillboardShader);
}

void CShaderHandler::CreateDoorWallShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pDoorWallShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pDoorWallShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pDoorWallShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSDoorWall");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pDoorWallShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pDoorWallShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSDoorWall");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pDoorWallShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pDoorWallShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSDoorWall");
	}
	pDoorWallShader->CreateInputLayout(ShaderTypes::Textured);
	pDoorWallShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature);
	pDoorWallShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("DoorWall", pDoorWallShader);
}

void CShaderHandler::CreateBridgeShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pBridgeShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pBridgeShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pBridgeShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSBridgeLight");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pBridgeShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pBridgeShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSBridgeLight");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pBridgeShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pBridgeShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSBridgeLight");
	}
	pBridgeShader->CreateInputLayout(ShaderTypes::Textured);
	pBridgeShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature);
	pBridgeShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Bridge", pBridgeShader);
}

void CShaderHandler::CreatePuzzleShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{ 
	CShader* pPuzzleShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pPuzzleShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pPuzzleShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSPuzzle");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pPuzzleShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pPuzzleShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSPuzzle");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pPuzzleShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pPuzzleShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSPuzzle");
	}
	pPuzzleShader->CreateInputLayout(ShaderTypes::Textured);
	pPuzzleShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false, true);
	pPuzzleShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Puzzle", pPuzzleShader);
}

void CShaderHandler::CreateSignShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pSignShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pSignShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pSignShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSSign");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pSignShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pSignShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSSign");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pSignShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pSignShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSSign");
	}
	pSignShader->CreateInputLayout(ShaderTypes::Textured);
	pSignShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, TRUE);
	pSignShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Sign", pSignShader);
}

void CShaderHandler::CreateTreeShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pTreeShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pTreeShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pTreeShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSFBXFeatureShader");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pTreeShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pTreeShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSFBXFeatureShader");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pTreeShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pTreeShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSFBXFeatureShader");
	}
	pTreeShader->CreateInputLayout(ShaderTypes::Textured);
	pTreeShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, false, true); 
	pTreeShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Tree", pTreeShader);
}

void CShaderHandler::CreateMirrorShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader* pMirrorShader = new CShader();
	if (m_UserID == ShaderHandlerUser::JH) {
		pMirrorShader->CreateVertexShader(L"Shaders\\ShaderJH.hlsl", "VSTexturedLighting");
		pMirrorShader->CreatePixelShader(L"Shaders\\ShaderJH.hlsl", "PSMirror");
	}
	else if (m_UserID == ShaderHandlerUser::YJ) {
		pMirrorShader->CreateVertexShader(L"Shaders\\ShaderYJ.hlsl", "VSTexturedLighting");
		pMirrorShader->CreatePixelShader(L"Shaders\\ShaderYJ.hlsl", "PSMirror");
	}
	else if (m_UserID == ShaderHandlerUser::TH) {
		pMirrorShader->CreateVertexShader(L"Shaders\\ShaderTH.hlsl", "VSTexturedLighting");
		pMirrorShader->CreatePixelShader(L"Shaders\\ShaderTH.hlsl", "PSMirror");
	}
	pMirrorShader->CreateInputLayout(ShaderTypes::Textured);
	pMirrorShader->CreateGeneralShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, TRUE);
	pMirrorShader->CreateBoundaryShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_Data.emplace("Mirror", pMirrorShader);
}
