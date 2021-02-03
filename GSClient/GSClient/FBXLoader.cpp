#include "stdafx.h"
#include "Shader.h"
#include "Mesh.h"
#include "FBXLoader.h"

class CFbxRenderInfo
{
public:
	CFbxRenderInfo() { }
	~CFbxRenderInfo();

public:
	CShader* m_pShader = NULL;
	CMeshFromFbx* m_pMesh = NULL;
};

CFbxRenderInfo::~CFbxRenderInfo()
{
	if (m_pShader) m_pShader->Release();
	if (m_pMesh) m_pMesh->Release();
}

FbxAMatrix XmFloat4x4MatrixToFbxMatrix(XMFLOAT4X4& xmf4x4Source)
{
	FbxAMatrix fbxmtxResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxResult[i][j] = xmf4x4Source.m[i][j];
	}
	return(fbxmtxResult);
}

FbxAMatrix GetGeometricOffsetTransform(FbxNode* pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

FbxScene* LoadFbxScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, char* pstrFbxFileName)
{
	FbxScene* pfbxScene = NULL;

	FbxIOSettings* pfbxIOSettings = pfbxSdkManager->GetIOSettings();
	FbxImporter* pfbxImporter = FbxImporter::Create(pfbxSdkManager, " ");
	bool bImportStatus = pfbxImporter->Initialize(pstrFbxFileName, -1, pfbxIOSettings);

	pfbxScene = FbxScene::Create(pfbxSdkManager, " ");
	bool bStatus = pfbxImporter->Import(pfbxScene);

	pfbxImporter->Destroy();

	return(pfbxScene);
}

void LoadFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pfbxNode, CMesh* pmesh)
{
	FbxNodeAttribute* pfbxNodeAttribute = pfbxNode->GetNodeAttribute();

	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pfbxNode->GetMesh();

		int nVertices = pfbxMesh->GetControlPointsCount();

		int nIndices = 0;
		int nPolygons = pfbxMesh->GetPolygonCount();

		cout << "-메쉬 로드됨 [폴리곤]: " << nPolygons << "개 [정점]: " << nVertices << "개\n" << endl;

		FbxGeometryElementNormal* normalEl = pfbxMesh->GetElementNormal();

		CDiffusedVertex *pVertices = new CDiffusedVertex[nVertices];

		for (int pindex = 0; pindex < nPolygons; pindex++)
		{
			const int lVerticeCount = pfbxMesh->GetPolygonSize(pindex);

			for (int lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)
			{
				XMFLOAT3 position;
				float texture[2];
				XMFLOAT3 normalvector;

				position.x = pfbxMesh->GetControlPointAt(lVerticeIndex).mData[0];
				position.y = pfbxMesh->GetControlPointAt(lVerticeIndex).mData[1];
				position.z = pfbxMesh->GetControlPointAt(lVerticeIndex).mData[2];

				FbxVector4 normal = normalEl->GetDirectArray().GetAt(nIndices);
				normalvector.x = (float)normal[0];
				normalvector.y = (float)normal[1];
				normalvector.z = (float)normal[2];

				FbxVector2 fbxTexCoord;
				FbxStringList UVSetNameList;

				for (int l = 0; l < pfbxMesh->GetElementUVCount(); ++l)
				{
					FbxGeometryElementUV* leUV = pfbxMesh->GetElementUV(l);

					int lTextureUVIndex = pfbxMesh->GetTextureUVIndex(pindex, lVerticeIndex);
					fbxTexCoord = leUV->GetDirectArray().GetAt(lTextureUVIndex);

					texture[0] = static_cast<float>(fbxTexCoord[0]);
					texture[1] = static_cast<float>(fbxTexCoord[1]);
				}

				//verticies.push_back({ position,texture ,normalvector });
			}
		}
	}

	int nChilds = pfbxNode->GetChildCount();
	cout <<"[남은 차일드]: " << nChilds << "개" << endl;
	for (int i = 0; i < nChilds; i++) LoadFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pfbxNode->GetChild(i), pmesh);
}

void CreateFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pfbxNode)
{
	FbxNodeAttribute* pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh* pfbxMesh = pfbxNode->GetMesh();

		int nVertices = pfbxMesh->GetControlPointsCount();

		int nIndices = 0;
		int nPolygons = pfbxMesh->GetPolygonCount();

		cout << "폴리곤 수: " << nPolygons << endl;

		for (int i = 0; i < nPolygons; i++) 
			nIndices += pfbxMesh->GetPolygonSize(i);

		int* pnIndices = new int[nIndices];

		for (int i = 0, k = 0; i < nPolygons; i++)
		{
			int nPolygonSize = pfbxMesh->GetPolygonSize(i);
			for (int j = 0; j < nPolygonSize; j++) pnIndices[k++] = pfbxMesh->GetPolygonVertex(i, j);
		}

		CFbxRenderInfo* pFbxRenderInfo = new CFbxRenderInfo();
		pFbxRenderInfo->m_pMesh = new CMeshFromFbx(pd3dDevice, pd3dCommandList, nVertices, nIndices, pnIndices);
		
		int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (nSkinDeformers > 0)
			pFbxRenderInfo->m_pShader = new CFbxSkinnedModelShader();
		else
			pFbxRenderInfo->m_pShader = new CFbxModelShader();
		pFbxRenderInfo->m_pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
		
		pfbxMesh->SetUserDataPtr(pFbxRenderInfo);

		if (pnIndices) delete[] pnIndices;
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++) CreateFbxMesh(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pfbxNode->GetChild(i));

}

void DrawFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxMesh* pfbxMesh, FbxAMatrix& fbxmtxNodeToRoot, FbxAMatrix& fbxmtxGeometryOffset, FbxAMatrix fbxmtxWorld)
{
	int nVertices = pfbxMesh->GetControlPointsCount();
	if (nVertices > 0)
	{
		FbxAMatrix fbxmtxTransform = fbxmtxWorld;
		int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (nSkinDeformers == 0) fbxmtxTransform = fbxmtxWorld * fbxmtxNodeToRoot * fbxmtxGeometryOffset;

		//CGameObject::UpdateShaderVariable(pd3dCommandList, &fbxmtxTransform);

		CFbxRenderInfo* pFbxRenderInfo = (CFbxRenderInfo*)pfbxMesh->GetUserDataPtr();
		if (pFbxRenderInfo->m_pShader) pFbxRenderInfo->m_pShader->Render(pd3dCommandList, NULL);
		if (pFbxRenderInfo->m_pMesh) pFbxRenderInfo->m_pMesh->Render(pd3dCommandList);
	}
}

void RenderFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxMesh* pfbxMesh, FbxAMatrix& fbxmtxNodeToRoot, FbxAMatrix& fbxmtxGeometryOffset, FbxAMatrix fbxmtxWorld)
{
	int nVertices = pfbxMesh->GetControlPointsCount();
	if (nVertices > 0)
	{
		FbxAMatrix fbxmtxTransform = fbxmtxWorld;
		int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (nSkinDeformers == 0) fbxmtxTransform = fbxmtxWorld * fbxmtxNodeToRoot * fbxmtxGeometryOffset;

		//CGameObject::UpdateShaderVariable(pd3dCommandList, &fbxmtxTransform);

		CFbxRenderInfo* pFbxRenderInfo = (CFbxRenderInfo*)pfbxMesh->GetUserDataPtr();
		if (pFbxRenderInfo->m_pShader) pFbxRenderInfo->m_pShader->Render(pd3dCommandList, NULL);
		if (pFbxRenderInfo->m_pMesh) pFbxRenderInfo->m_pMesh->Render(pd3dCommandList);
	}
}

void RenderFbxNodeHierarchy(ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pfbxNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld)
{
	FbxNodeAttribute* pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxAMatrix fbxmtxNodeToRoot = pfbxNode->EvaluateGlobalTransform(fbxCurrentTime);
		FbxAMatrix fbxmtxGeometryOffset = GetGeometricOffsetTransform(pfbxNode);

		FbxMesh* pfbxMesh = pfbxNode->GetMesh();
		RenderFbxMesh(pd3dCommandList, pfbxMesh, fbxmtxNodeToRoot, fbxmtxGeometryOffset, fbxmtxWorld);
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++) RenderFbxNodeHierarchy(pd3dCommandList, pfbxNode->GetChild(i), fbxCurrentTime, fbxmtxWorld);
}
