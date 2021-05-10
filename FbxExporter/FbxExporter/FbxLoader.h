#pragma once

struct Keyframe
{
	float frameTime;

	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMFLOAT4 rotationquat;
	
	bool operator == (const Keyframe& rhs) {
		if (translation.x != rhs.translation.x || translation.y != rhs.translation.y || translation.z != rhs.translation.z)
			return false;

		if (scale.x != rhs.scale.x || scale.y != rhs.scale.y || scale.z != rhs.scale.z)
			return false;

		if (rotationquat.x != rhs.rotationquat.x || rotationquat.y != rhs.rotationquat.y || rotationquat.z != rhs.rotationquat.z || rotationquat.w != rhs.rotationquat.w)
			return false;

		return true;
	}
};

struct Joint
{
	string name;
	int parentIndex = -1;
	XMFLOAT4X4 offset;
	vector<Keyframe> mAnimation;
};

struct BlendingInfo
{
	int index;
	float weight;

	bool operator < (const BlendingInfo& rhs) {
		return (weight > rhs.weight);
	}
};

struct ControlPoint
{
	XMFLOAT3 pos;
	vector<BlendingInfo> blendInfo;

	ControlPoint() {
		pos = { 0, 0, 0 };
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

	bool operator<(const TrianglePG& rhs) { 
		return matIndex < rhs.matIndex;
	}
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

	unordered_map<int, ControlPoint> cpoints;
	vector<TrianglePG> triangles;
	vector<Joint> mSkeleton;
	vector<FbxVertex> vertices;

public:
	FbxLoader();
	FbxLoader(FbxManager* pfbxSdkManager, char* FileName);
	~FbxLoader();

	void ExploreFbxHierarchy(FbxNode* pNode);

	void LoadScene(char* pstrFbxFileName);

	void LoadSkeletonHierarchy(FbxNode* pNode);
	void LoadSkeletonRecursively(FbxNode* pNode, int inDepth, int myIndex, int inParentIndex);

	void LoadControlPoints(FbxNode* pNode);

	void LoadAnimation(FbxNode* pNode);
	void LoadMesh(FbxNode* pNode);

	void Optimize();

	void SaveAsFile();
};
