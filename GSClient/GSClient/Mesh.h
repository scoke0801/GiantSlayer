#pragma once
#include "FbxLoader.h"

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
	XMFLOAT3				m_xmf3Normal; 
	BYTE					m_TextureInfo;

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

class CParticleVertex : public CVertex
{
public:
	XMFLOAT4						m_xmf4Diffuse; // 색상
	XMFLOAT3						m_xmf3Speed;   // 속도
	XMFLOAT2						m_xmf2Time;   // x: emitTime, y: lifeTime 
	XMFLOAT3						m_xmf2RandomValue;

public:
	CParticleVertex() {}
	CParticleVertex(const XMFLOAT3& xmf3Position, const XMFLOAT4& xmf4Diffuse, const XMFLOAT2& xmf2Time);
	~CParticleVertex() { }
};

class CParticleTextureVertex : public CVertex
{
public:	
	XMFLOAT3						m_xmf3Speed;   // 속도
	XMFLOAT2						m_xmf2Time;   // x: emitTime, y: lifeTime 
	XMFLOAT3						m_xmf2RandomValue;
	XMFLOAT2						m_xmf2TexCoord;
	UINT							m_nTexture;

public:
	CParticleTextureVertex() {}
	CParticleTextureVertex(const XMFLOAT3& xmf3Position, const XMFLOAT2& xmf2Time,
 UINT textureCode);
	~CParticleTextureVertex() { } 
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
	UINT m_nBones = 0;

	CMesh** m_ppMeshes = NULL;
	int m_Meshes = 0;

	BoundingBox m_bBox;

protected:
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

public:
	CTexturedVertex* m_pxmf4MappedPositions = NULL;

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
	CPlaneMeshTextured(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		float uvXStart, float uvYStart,
		float uvXEnd, float uvYEnd,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CPlaneMeshTextured();
};

//////////////////////////////////////////////////////////////////////////////
//
class CArrowParticleMesh : public CMesh
{
private:
	CParticleVertex*	m_Vertices;
	int					m_CurrentVertexIndex = 0;

public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CArrowParticleMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, 
		int particleCount); 

	virtual ~CArrowParticleMesh();

public:
	void CreateMeshes(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, int count);

	void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	 
};

class CFogParticleMesh : public CMesh
{
private:
	CParticleVertex* m_Vertices;
	int					m_CurrentVertexIndex = 0;

public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CFogParticleMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		int particleCount);

	virtual ~CFogParticleMesh();

public:
	void CreateMeshes(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, int count);

	void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

class CRainParticleMesh : public CMesh
{
private:
	CParticleTextureVertex*	m_Vertices;
	int					m_CurrentVertexIndex = 0;

public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CRainParticleMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		int particleCount);

	virtual ~CRainParticleMesh();

public:
	void CreateMeshes(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		int count, PARTICLE_TYPE type);

	void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

class CTexParticleMesh : public CMesh
{
private:
	CParticleTextureVertex * m_Vertices;
	int					m_CurrentVertexIndex = 0;

public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CTexParticleMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		int particleCount);

	virtual ~CTexParticleMesh();

public: 
	void CreateMeshes(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		int count, PARTICLE_TYPE type);

	void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

};
//////////////////////////////////////////////////////////////////////////////
//
class CSphereMeshDiffused : public CMesh
{
public:
	//직사각형의 가로, 세로 길이를 지정하여 직사각형 메쉬를 생성한다. 
	CSphereMeshDiffused(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		PulledModel pulledModelInfo,
		float radius, UINT32 sliceCount, UINT32 stackCount,
		const XMFLOAT3& shift);
	virtual ~CSphereMeshDiffused();
};

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
		bool isHeightHalfOn,
		float fWidth, float fHeight, float fDepth);

	CCubeMeshDiffused(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		bool isHeightHalfOn,
		float fWidth, float fHeight, float fDepth,
		const XMFLOAT3& shift);

	CCubeMeshDiffused(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		PulledModel pulledModelInfo,
		float fWidth, float fHeight, float fDepth);

	CCubeMeshDiffused(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		PulledModel pulledModelInfo,
		float fWidth, float fHeight, float fDepth,
		const XMFLOAT3& shift);
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
		float sizeX, float sizeY);
	CBillboardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CBillboardVertex* pGeometryBillboardVertices, UINT nGeometryBillboardVertices);
	virtual ~CBillboardMesh();
};

//////////////////////////////////////////////////////////////////////////////
//

typedef struct CMeshinfo
{
	vector<CTexturedVertex> vertex;
	//vector<CDiffusedVertex> vertex;
	int vertics;
};

class CFixedMesh : public CMesh
{
public:
	int nVertices = 0;
	int nPolygons = 0;
	int nBones = 0;

	int nAnimLength;
	bool hasAnimation;

	string animName;
	vector<Vertex> vertices;
	vector<int> indices;
	vector<Bone> skeleton;

	CFixedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		char* pstrFbxFileName);
	virtual ~CFixedMesh();

	void LoadFile(char* pstrFbxFileName);
};

/*
vector<Vertex> vertices;
	vector<int> indices;
	vector<Bone> skeleton;
*/

class CAnimatedMesh : public CMesh
{
protected:
	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	ID3D12Resource* m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	ID3D12Resource* m_pd3dTextureCoord0Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord0UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

	ID3D12Resource* m_pd3dBoneIndexBuffer = NULL;
	ID3D12Resource* m_pd3dBoneIndexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	ID3D12Resource* m_pd3dBoneWeightBuffer = NULL;
	ID3D12Resource* m_pd3dBoneWeightUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

	vector<Vertex> temp;
public:
	int nBones = 0;

	CAnimatedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		vector<Vertex> vertices, vector<int> indices, vector<Bone> skeletons);
	virtual ~CAnimatedMesh();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;
};

class CMeshFbx : public CMesh
{
public:
	CMeshFbx(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, char* pstrFbxFileName,
		bool rotateFlag = false);
	virtual ~CMeshFbx();

public:
	void LoadMesh(FbxNode* node, CMeshinfo* info, bool rotateFlag = false);
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
	// 벽 생성	 
	CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		int heights[25],
		XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1],
		int xNomalPos, int zNormalPos);
	CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		bool xZero, bool zZero,
		int* heights,
		XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1],
		int xNomalPos, int zNormalPos);
	 
	CTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		int x_Index, int z_Index,  
		int heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1],
		XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1]);

	~CTerrainMesh();
};
  
class CBindingTerrainMesh : public CMesh
{
private:
	CTerrainVertex*	m_Vertices; 
	int				m_CurrentVertexIndex = 0;

protected:
	XMFLOAT4 m_xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

	int m_nWidth;
	int m_nDepth;

public:
	CBindingTerrainMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int verticesCount);
	~CBindingTerrainMesh();

public:
	// 벽 생성	 
	void CreateWallMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, 
		const XMFLOAT3& shift, BYTE textureInfo, 
		int heights[25],
		XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1], int xNomalPos, int zNormalPos); 

	void CreateGridMesh(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList, 
		const XMFLOAT3& shift, BYTE textureInfo, 
		int x_Index, int z_Index,
		int heights[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1], XMFLOAT3 normals[TERRAIN_HEIGHT_MAP_HEIGHT + 1][TERRAIN_HEIGHT_MAP_WIDTH + 1]);

	void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};
