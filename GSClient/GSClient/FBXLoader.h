#pragma once

struct FbxSubMesh
{
	int nControlPoint;
	int nPolygon;
	int nDeformer;

	vector<int> mIndex;
	vector<CTexturedVertex> mVertex;

	vector<double> mClusterWeight;
	vector<XMFLOAT3> mClusterDef;
};

struct FbxModel
{
	vector<FbxSubMesh> mMesh;
};

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
