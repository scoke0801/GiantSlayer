#pragma once
  
enum class ShaderHandlerUser
{
	TH,
	YJ,
	JH,
};
class CShader;
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
	CShader* GetData22(const string& id)
	{
		auto targetData = m_Data.find(id);
		if (targetData != m_Data.end())
			return targetData->second;

		if (id.compare("FBX")){ 
			return GetData("FBX");
		} 
		if (id.compare("UI") || id.compare("Ui")){ 
			return GetData("UI");
		}
		if (id.compare("Minimap"))
		{ 
			return GetData("Minimap");
		}
		if (id.compare("Player")){ 
			return GetData("Player");
		}
		if (id.compare("Skybox")){ 
			return GetData("SkyboxShader");
		}
		if (id.compare("Terrain")){ 
			return GetData("Terrain");
		}
		if (id.compare("Billboard")){ 
			return GetData("Billboard");
		}
		if (id.compare("Object") || id.compare("BasicObject")){ 
			return GetData("BasicObject");
		}
		if (id.compare("DoorWall")){ 
			return GetData("DoorWall");
		}
		if (id.compare("Puzzle")) { 
			return GetData("Puzzle");
		}
		if (id.compare("Bridge")) { 
			return GetData("Bridge");
		}
		if (id.compare("FbxFixed")) {
			return GetData("Bridge");
		}
		if (id.compare("FbxAnimated")) {
			return GetData("FbxAnimated");
		}
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

	void CreateDoorWallShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateBridgeShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreatePuzzleShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateSignShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateTreeShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateMirrorShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreatePlayerShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateUiShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateBillboardShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateMinmapShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateSkyboxShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateTerrainShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

public: 
	void SetUserID(ShaderHandlerUser user) { m_UserID = user; }
};