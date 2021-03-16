#pragma once
template<class T>
class CHandler
{
private:
	unordered_map<string, T> m_Data;

private:
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
	T& GetData(const string& id)
	{
		auto targetData = m_Data.find(id);
		if (targetData != m_Data.end())
			return targetData->second;
		return T();
	}
};

