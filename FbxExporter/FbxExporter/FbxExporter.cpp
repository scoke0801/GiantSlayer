#include "stdafx.h"
#include "FbxLoader.h"

int main()
{
	FbxManager* m_pfbxManager = FbxManager::Create();
	FbxScene* m_pfbxScene = FbxScene::Create(m_pfbxManager, "");
	FbxIOSettings* m_pfbxIOs = FbxIOSettings::Create(m_pfbxManager, "");
	FbxImporter* m_pfbxImporter = nullptr;
	m_pfbxImporter->SetIOSettings(m_pfbxIOs);

	FbxLoader(m_pfbxManager, "Files/human.fbx");
}
