#pragma once

struct Keyframe
{
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext(nullptr)
	{}
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

struct BlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	BlendingIndexWeightPair() :
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

struct CtrlPoint
{
	XMFLOAT3 mPosition;
	XMFLOAT2 mUv;
	std::vector<BlendingIndexWeightPair> mBlendingInfo;

	CtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};



class FbxLoader
{
private:
	FbxManager*					mFbxManager = NULL;
	FbxScene*					mFbxScene = NULL;

	int							nControlPoints = 0;
	int							nVertics = 0;
	int							nPolygons = 0;
	float						mAnimationLength;
	string						mAnimationName;

	vector<CTexturedVertex>		mMesh;
	vector<Joint>				mSkeleton;
	
	unordered_map<unsigned int, CtrlPoint*> mControlPoints;

public:
	FbxLoader(FbxManager* pfbxSdkManager, char* pstrFbxFileName);
	~FbxLoader();

	void LoadScene(char* pstrFbxFileName);

	void LoadFbxHierarchy(FbxNode* pNode);

	void LoadMesh(FbxNode* pNode);

	void LoadAnimations(FbxNode* pNode);
	void LoadSkeletonHierarchy(FbxNode* pNode);
	void LoadSkeletonRecursively(FbxNode* pNode, int inDepth, int myIndex, int inParentIndex);

	void SaveAsFile();
};