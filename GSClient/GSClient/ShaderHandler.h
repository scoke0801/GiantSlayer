#pragma once
  
enum class ShaderHandlerUser
{
	TH,
	YJ,
	JH,
};
class CShader;
class CTerrain;

class CShaderHandler
{
private:
	ShaderHandlerUser m_UserID; 
	unordered_map<string, CShader*> m_Data;
	  
private:
	CShaderHandler() {}
	CShaderHandler(const CShaderHandler& other) = delete;
	CShaderHandler& operator=(const CShaderHandler& other) = delete;

public:
	static CShaderHandler& GetInstance()
	{
		static CShaderHandler self;
		return self;
	}
public:
	void AddData(const string& id, CShader* data)
	{
		m_Data.emplace(id, data);
	}
	void DeleteData(const string& id)
	{
		m_Data.erase(id);
	}
	CShader* GetData(const string& id)
	{
		auto targetData = m_Data.find(id);
		if (targetData != m_Data.end())
			return targetData->second;

		string resultText = "지정되지 않은 셰이더 호출!";
		resultText += (" " + id);
		assert(!resultText.c_str());
		return nullptr;
	}
public:
	void CreateAllShaders(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateFBXShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateFBXAnimatedShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateBasicObjectShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature); 
	void CreateLaserShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateStandardShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateSkinnedShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateDoorWallShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateBridgeShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreatePuzzleShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateSignShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateTreeShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateMirrorShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreatePlayerShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateUiShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateShadowShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateBillboardShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateMinmapShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateSkyboxShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateTerrainShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateTerrainWaterShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateParticleShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateEffectShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateTextRenderShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
public: 
	void SetUserID(ShaderHandlerUser user) { m_UserID = user; }
};