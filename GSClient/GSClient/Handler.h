#pragma once
template<class T>
class CHandler
{
protected:
	unordered_map<string, T> m_Data;

protected:
	CHandler() {}
	CHandler(const CHandler<T>& other) = delete;
	CHandler& operator=(const CHandler<T>& other) = delete;

public:
	static CHandler<T>& GetInstance()
	{
		static CHandler<T> self;
		return self;
	}

public:
	void AddData(const string& id, const T& data)
	{
		m_Data.emplace(id, data);
	}
	void DeleteData(const string& id)
	{
		m_Data.erase(id);
	}
	virtual T& GetData(const string& id)
	{
		auto targetData = m_Data.find(id);
		if (targetData != m_Data.end())
			return targetData->second;
		return T();
	}
};

enum class ShaderHandlerUser
{
	TH,
	YJ,
	JH,
};
class CShader; 
class CShaderHandler : public CHandler<CShader*>
{
private:
	ShaderHandlerUser m_UserID;

private:
	CShaderHandler() {}
	~CShaderHandler() {}

public:
	CShader*& GetData(const string& id) override
	{
		auto targetData = m_Data.find(id);
		if (targetData != m_Data.end())
			return targetData->second;

		if (id.compare("FBX"))
		{
			CreateFBXShader(); 
			return GetData("FBX");
		} 
		if (id.compare("UI") || id.compare("Ui"))
		{
			CreateUiShader();
			return GetData("UI");
		}
		if (id.compare("Minimap"))
		{
			CreateMinmapShader();
			return GetData("Minimap");
		}
		if (id.compare("Player"))
		{
			CreatePlayerShader();
			return GetData("Player");
		}
		if (id.compare("Skybox"))
		{
			CreateSkyboxShader();
			return GetData("SkyboxShader");
		}
		if (id.compare("Terrain"))
		{
			CreateSkyboxShader();
			return GetData("Terrain");
		}
		string resultText = "지정되지 않은 셰이더 호출!";
		resultText += (" " + id);
		assert(!resultText);
	}
public:
	void CreateFBXShader()    { return;}
	void CreateUiShader()     { return;} 
	void CreateBoxShader()    { return;}
	void CreatePlayerShader() { return;}
	void CreateMinmapShader() { return;}
	void CreateSkyboxShader() { return; }
public:
	void SetUserID(ShaderHandlerUser user) { m_UserID = user; }
};