#pragma once

struct Keyframe
{
	FbxLongLong nFrame;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext(nullptr)
	{}
};

struct Joint
{
	string name;
	int parentIndex;
	FbxAMatrix mGlobalBindpose;
	Keyframe* mAnimation;

	Joint() : mAnimation(nullptr)
	{
		parentIndex = -1;
		mGlobalBindpose.SetIdentity();
	}

	~Joint()
	{
		while (mAnimation)
		{
			Keyframe* temp = mAnimation->mNext;
			delete mAnimation;
			mAnimation = temp;
		}
	}
};

struct BlendingInfo
{
	int index;
	double weight;

	bool operator < (const BlendingInfo& rhs)
	{
		return (weight > rhs.weight);
	}
};

struct ControlPoint
{
	XMFLOAT3 pos;
	vector<BlendingInfo> blendInfo;

	ControlPoint() {
		blendInfo.reserve(4);
	}
};

struct FbxVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;

	vector<BlendingInfo> blendInfo;

	void SortBlendingInfoByWeight()
	{
		std::sort(blendInfo.begin(), blendInfo.end());
	}
};

struct TrianglePG
{
	vector<int> indices;
	string matName;
	int matIndex;
};

class FbxLoader
{
private:
	FbxManager* mFbxManager;
	FbxScene* mFbxScene;

	////////////////////////////////////////////////////////

	int numCP, numPG, numDF;
	bool hasAnimation = true;
	string animName;
	int animLength;

	vector<ControlPoint> cpoints;
	vector<TrianglePG> triangles;
	vector<Joint> mSkeleton;
	vector<FbxVertex> vertices;

public:
	FbxLoader();
	FbxLoader(FbxManager* pfbxSdkManager, char* FileName);
	~FbxLoader();

	void LoadScene(char* pstrFbxFileName);

	void LoadSkeletonHierarchy(FbxNode* pNode);
	void LoadSkeletonRecursively(FbxNode* pNode, int inDepth, int myIndex, int inParentIndex);

	void LoadControlPoints(FbxNode* pNode);

	void LoadAnimation(FbxNode* pNode);
	void LoadMesh(FbxNode* pNode);

	void ExploreFbxHierarchy(FbxNode* pNode);
	void SaveAsFile();

};
