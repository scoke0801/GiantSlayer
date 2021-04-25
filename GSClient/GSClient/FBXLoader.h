#pragma once

//struct Keyframe
//{
//	FbxLongLong nFrame;
//	FbxAMatrix mGlobalTransform;
//	Keyframe* mNext;
//
//	Keyframe() :
//		mNext(nullptr)
//	{}
//};
//
//struct Joint
//{
//	string name;
//	int parentIndex;
//	XMFLOAT4X4 globalBindpose;
//	Keyframe* mAnimation;
//};

struct FbxSubMesh
{
	int nControlPoint;
	int nPolygon;
	int nDeformer;
	XMFLOAT4X4 gTransform;

	vector<int> mIndex;
	vector<CTexturedVertex> mVertex;

	//vector<double> mClusterWeight;
	//vector<XMFLOAT3> mClusterDef;
	XMFLOAT3* mCP;

	double* mClusterWeight;
	XMFLOAT3* mClusterDef;
};

/*
ControlPoint = Mesh Vertics
Vertex = Triangles Vertics
*/

class FbxLoader
{
private:
	FbxManager* mFbxManager;
	FbxScene* mFbxScene;

	FbxLongLong mAnimationLength;
	string mAnimationName;

	////////////////////////////////////////////////////////

	vector<FbxSubMesh> mFbxMesh;

public:
	FbxLoader();
	FbxLoader(FbxManager* pfbxSdkManager, char* FileName);
	~FbxLoader();

	void LoadScene(char* pstrFbxFileName);

	void LoadSkeletonHierarchy(FbxNode* pNode);
	void LoadSkeletonRecursively(FbxNode* pNode, int inDepth, int myIndex, int inParentIndex);

	void ExploreFbxHierarchy(FbxNode* pNode);
	void ExportFbxFile();

};
