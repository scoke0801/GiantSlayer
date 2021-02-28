#pragma once

//정점을 표현하기 위한 클래스를 선언한다. 
class CVertex
{
public:
	//정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다).
	XMFLOAT3 m_xmf3Position;
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
public:
	//정점의 색상이다.
	XMFLOAT4 m_xmf4Diffuse;
public:
	CDiffusedVertex()
	{
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse)
	{
		m_xmf3Position = XMFLOAT3(x, y, z);
		m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse)
	{
		m_xmf3Position = xmf3Position;
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

class CTerrainVertex : public CVertex
{
public:
	XMFLOAT2				m_xmf2TexCoord;
	XMFLOAT4				m_xmf4Color;
public:
	CTerrainVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTerrainVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord;  }
	CTerrainVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord;  }
	~CTerrainVertex() { }
};

class CTexturedVertex : public CVertex
{
public:
	XMFLOAT2	m_xmf2TexCoord;
	XMFLOAT3	m_xmf3Normal;

public:
	CTexturedVertex()
	{ 
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); 
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	}
	CTexturedVertex(float x, float y, float z, float uvX, float uvY)
	{
		m_xmf3Position = XMFLOAT3(x, y, z);
		m_xmf2TexCoord = XMFLOAT2(uvX, uvY);
	}
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) 
	{ 
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf2TexCoord = xmf2TexCoord;
	}
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmf3Normal)
	{ 
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf2TexCoord = xmf2TexCoord; 
		m_xmf3Normal = xmf3Normal;	
	}
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f))
	{ 
		m_xmf3Position = xmf3Position; 
		m_xmf2TexCoord = xmf2TexCoord;
	}
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmf3Normal) 
	{ 
		m_xmf3Position = xmf3Position; 
		m_xmf2TexCoord = xmf2TexCoord; 
		m_xmf3Normal = xmf3Normal; 
	}

	~CTexturedVertex() { }
};

class CBillboardVertex : public CVertex
{
public:
	XMFLOAT2						m_xmf2Size;
	UINT							m_nTexture;

public:
	CBillboardVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2Size = XMFLOAT2(5.0f, 10.0f); m_nTexture = 0; }
	CBillboardVertex(float x, float y, float z, XMFLOAT2 xmf2Size = XMFLOAT2(5.0f, 10.0f), UINT nTexture = 0) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2Size = xmf2Size; m_nTexture = nTexture; }
	CBillboardVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2Size = XMFLOAT2(5.0f, 10.0f), UINT nTexture = 0) { m_xmf3Position = xmf3Position; m_xmf2Size = xmf2Size; m_nTexture = nTexture; }
	~CBillboardVertex() { }
};

//////////////////////////////////////////////////////////////////////////////
//

class CMesh
{
private:
	int m_nReferences = 0;
	

protected:
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nIndices=0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
	UINT m_nPolygons = 0;

	CMesh** m_ppMeshes = NULL;
	int m_Meshes = 0;

	BoundingBox m_bBox;

protected:
	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

	int	faceCount = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers(); 

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	BoundingBox GetBoundingBox() { return m_bBox; };

	void SetBoundingBox(XMFLOAT3 center, XMFLOAT3 externs) {  };
};

//////////////////////////////////////////////////////////////////////////////
// 
class CPlaneMeshDiffused : public CMesh
{
public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CPlaneMeshDiffused(ID3D12Device * pd3dDevice,
		ID3D12GraphicsCommandList * pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f),
		bool isVertical = true);

	virtual ~CPlaneMeshDiffused();
}; 
class CPlaneMeshTextured : public CMesh
{
public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CPlaneMeshTextured(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f,
		bool isVertical = true,
		bool isHalfSize = false); 
	CPlaneMeshTextured(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 startPos,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f,
		bool isVertical = true);
	virtual ~CPlaneMeshTextured();
};
//////////////////////////////////////////////////////////////////////////////
//
class CSphereMesh : public CMesh
{
public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CSphereMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		float radius, UINT32 sliceCount, UINT32 stackCount);
	virtual ~CSphereMesh();
};

//////////////////////////////////////////////////////////////////////////////
//

class CCubeMeshDiffused : public CMesh
{
public:
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다. 
	CCubeMeshDiffused(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);

	virtual ~CCubeMeshDiffused();
};

class CCubeMeshTextured : public CMesh
{
public:
	CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, bool sameBackFace = false);
	CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 startPos,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};

class CDoorMesh : public CMesh
{
public:
	CDoorMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f,
		bool isLeft = false);
	virtual ~CDoorMesh();
};
class CDoorMeshTest : public CMesh
{
public:
	CDoorMeshTest(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f,
		bool isLeft = false);
	virtual ~CDoorMeshTest();
};

//////////////////////////////////////////////////////////////////////////////
//
class CTexturedRectMesh : public CMesh
{
public:
	CTexturedRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};

//////////////////////////////////////////////////////////////////////////////
//
class CBillboardMesh : public CMesh
{
public:
	CBillboardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CBillboardVertex* pGeometryBillboardVertices, UINT nGeometryBillboardVertices);
	virtual ~CBillboardMesh();
};

//////////////////////////////////////////////////////////////////////////////
//

typedef struct Meshinfo
{
	vector<CTexturedVertex> vertex;
	//vector<CDiffusedVertex> vertex;
	int vertics;
};

//////////////////////////////////////////////////////////////////////////////
//
class CMeshFbx : public CMesh
{
public:
	CMeshFbx(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, char* pstrFbxFileName,
		bool rotateFlag = false);


	virtual ~CMeshFbx();

public:
	void LoadMesh(FbxNode* node, Meshinfo* info, bool rotateFlag = false);
};

class CMeshFromFbx
{
public:
	CMeshFromFbx(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nVertices, int nIndices, int* pnIndices);
	virtual ~CMeshFromFbx();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nOffset = 0;

protected:
	int								m_nVertices = 0;

	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								m_nIndices = 0;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

public:
	XMFLOAT4* m_pxmf4MappedPositions = NULL;

public:
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////
// Ui Meshes
/////////////////////////////////////////////////////////////////////////////
class CMinimapMesh : public CMesh
{
public:
	CMinimapMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float radius);
	~CMinimapMesh();
};
class CHpSpPercentMesh : public CMesh
{
public:
	CHpSpPercentMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f,
		bool isHp = true);
	~CHpSpPercentMesh();
};
class CMinimapAroowMesh : public CMesh
{
public:
	CMinimapAroowMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	~CMinimapAroowMesh();
};
//////////////////////////////////////////////////////////////////////////////
//
enum class MapMeshHeightType
{
	Plane = 0,
	UpRidge,
	DownRidge,
};
class CTerrainMesh : public CMesh
{
protected:
	XMFLOAT4 m_xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

	int m_nWidth;
	int m_nDepth;

public:
	CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		int x_Index, int z_Index, int WidthBlock_Count, int DepthBlock_Count, int WidthBlock_Index, int DepthBlock_Index,
		MapMeshHeightType heightType = MapMeshHeightType::Plane);
	
	~CTerrainMesh();

	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다.
	float GetHeightPlane(float x, float z) const { return 0; }
	float GetHeightUpRidge(float x, float z, float waveSize);
	float GetHeightDownRidge(float x, float z, float waveSize);
};
