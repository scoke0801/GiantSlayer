#pragma once

struct BlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	BlendingIndexWeightPair() :
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

struct VertexBlendingInfo
{
	int mBlendingIndex;
	double mBlendingWeight;

	VertexBlendingInfo() :
		mBlendingIndex(0),
		mBlendingWeight(0.0)
	{}

	bool operator < (const VertexBlendingInfo& rhs)
	{
		return (mBlendingWeight > rhs.mBlendingWeight);
	}
};

struct Keyframe
{
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext(nullptr)
	{}
};

/////////////////////////////////////////////////////////////////////////

struct ControlPoint
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	vector<BlendingIndexWeightPair> mBlendingInfo;

	ControlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};

struct MeshVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	vector<VertexBlendingInfo> mVertexBlendingInfos;

	void SortBlendingInfoByWeight()
	{
		std::sort(mVertexBlendingInfos.begin(), mVertexBlendingInfos.end());
	}
};

struct TriPolygon
{
	vector<int> mIndices;
	string mMaterialName;
	int mMaterialIndex;

	bool operator<(const TriPolygon& rhs)
	{
		return mMaterialIndex < rhs.mMaterialIndex;
	}
};

struct Joint
{
	std::string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	Keyframe* mAnimation;
	FbxNode* mNode;

	Joint() :
		mNode(nullptr),
		mAnimation(nullptr)
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
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

class FbxLoader
{
private:
	FbxManager* mFbxManager;
	FbxScene* mFbxScene;

	FbxLongLong mAnimationLength;
	string mAnimationName;
	bool Animation = 0;

	vector<ControlPoint*> mControlPoint;
	vector<MeshVertex> mVertex;
	vector<TriPolygon> mPolygon;
	vector<Joint> mSkeleton;

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
