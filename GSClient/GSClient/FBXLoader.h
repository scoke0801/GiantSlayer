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

	void SortBlendingInfoByWeight() {
		std::sort(blendInfo.begin(), blendInfo.end());
	}

	bool operator==(const FbxVertex& rhs) const {
		bool isSame = true;

		if (!(blendInfo.empty() && rhs.blendInfo.empty())) {
			for (int i = 0; i < 4; i++) {
				if (blendInfo[i].index != rhs.blendInfo[i].index || 
					abs(blendInfo[i].weight - rhs.blendInfo[i].weight) > 0.001)
				{
					isSame = false;
					break;
				}
			}
		}

		XMFLOAT3 vector3Epsilon = { 0.00001f, 0.00001f, 0.00001f };
		XMFLOAT2 vector2Epsilon = { 0.00001f, 0.00001f };

		bool posSame = (XMVector3NearEqual(XMLoadFloat3(&pos), XMLoadFloat3(&rhs.pos), XMLoadFloat3(&vector3Epsilon)) == TRUE);
		bool uvSame = (XMVector2NearEqual(XMLoadFloat2(&uv), XMLoadFloat2(&rhs.uv), XMLoadFloat2(&vector2Epsilon)) == TRUE);
		bool normalSame = (XMVector3NearEqual(XMLoadFloat3(&normal), XMLoadFloat3(&rhs.normal), XMLoadFloat3(&vector3Epsilon)) == TRUE);
	
		return isSame && posSame && uvSame && normalSame;
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

	void Optimize();

	void ExploreFbxHierarchy(FbxNode* pNode);
	void SaveAsFile();

};
