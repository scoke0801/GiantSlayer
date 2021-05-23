#pragma once

struct Keyframe
{
	float frameTime;

	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMFLOAT4 rotationquat;

	XMFLOAT4X4 animatrix;

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

struct BoneAnimation
{
	vector<Keyframe> keyframes;

	float GetStartTime()const
	{
		return keyframes.front().frameTime;
	}
	float GetEndTime()const
	{
		return keyframes.back().frameTime;
	}

	void Interpolate(float t, XMFLOAT4X4& M)const
	{
		if (t <= keyframes.front().frameTime)
		{
			XMVECTOR S = XMLoadFloat3(&keyframes.front().scale);
			XMVECTOR P = XMLoadFloat3(&keyframes.front().translation);
			XMVECTOR Q = XMLoadFloat4(&keyframes.front().rotationquat);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
		}
		else if (t >= keyframes.back().frameTime)
		{
			XMVECTOR S = XMLoadFloat3(&keyframes.back().scale);
			XMVECTOR P = XMLoadFloat3(&keyframes.back().translation);
			XMVECTOR Q = XMLoadFloat4(&keyframes.back().rotationquat);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
		}
		else
		{
			for (UINT i = 0; i < keyframes.size() - 1; ++i)
			{
				if (t >= keyframes[i].frameTime && t <= keyframes[i + 1].frameTime)
				{
					float lerpPercent = (t - keyframes[i].frameTime) / (keyframes[i + 1].frameTime - keyframes[i].frameTime);

					XMVECTOR s0 = XMLoadFloat3(&keyframes[i].scale);
					XMVECTOR s1 = XMLoadFloat3(&keyframes[i + 1].scale);

					XMVECTOR p0 = XMLoadFloat3(&keyframes[i].translation);
					XMVECTOR p1 = XMLoadFloat3(&keyframes[i + 1].translation);

					XMVECTOR q0 = XMLoadFloat4(&keyframes[i].rotationquat);
					XMVECTOR q1 = XMLoadFloat4(&keyframes[i + 1].rotationquat);

					XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
					XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
					XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

					XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
					XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

					break;
				}
			}
		}
	}
};

struct AnimationClip
{
	string name;
	float startTime = 0;
	float endTime = 0;

	vector<BoneAnimation> BoneAnimation;

	void Interpolate(float t, vector<XMFLOAT4X4>& boneTransforms)const
	{
		for (UINT i = 0; i < BoneAnimation.size(); ++i)
		{
			BoneAnimation[i].Interpolate(t, boneTransforms[i]);
		}
	}
};

struct Joint
{
	string name;
	int parentIndex = -1;
	XMFLOAT4X4 offset;
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

	void SortBlendingWeight() {
		sort(blendInfo.begin(), blendInfo.end());
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

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 binormal;
	XMFLOAT3 tangent;

	float weights[4];
	int indices[4];
};

struct CKeyFrame
{
	float time;

	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMFLOAT4 rotationquat;
};

struct Bone
{
	string name;
	int parentIndex;
	XMFLOAT4X4 offset;

	vector<CKeyFrame> animations;
};

class FbxLoader
{
private:
	// Fbx
	FbxManager* mFbxManager;
	FbxScene* mFbxScene;
	FbxAxisSystem mAxisSystem;

	int numCP, numPG, numDF;
	bool hasAnimation;
	int rotateNum;

	// Mesh Info
	unordered_map<int, ControlPoint> cpoints;
	vector<FbxVertex> vertices;
	vector<TrianglePG> triangles;

	// Animation Info
	int nAnimation = 0;
	FbxAnimStack** ppAnimationStacks;;

	vector<Joint> mSkeleton;
	vector<AnimationClip> animClip;


public:
	FbxLoader();
	FbxLoader(FbxManager* pfbxSdkManager, string fileName, bool hasAnim, int nRotate);
	~FbxLoader();

	void ExploreFbxHierarchy(FbxNode* pNode);

	void LoadScene(char* pstrFbxFileName);

	void LoadSkeletonHierarchy(FbxNode* pNode);
	void LoadSkeletonRecursively(FbxNode* pNode, int inDepth, int myIndex, int inParentIndex);

	void LoadControlPoints(FbxNode* pNode);

	void LoadBoneOffsets(FbxNode* pNode);
	void LoadMesh(FbxNode* pNode);

	void LoadAnimations(FbxNode* pNode, int stackNum);

	void Optimize();

	void SaveAsFile(string filePath);
};

FbxAMatrix GeometricOffsetTransform(FbxNode* pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

XMFLOAT4X4 FbxMatrixToXmFloat4x4(FbxAMatrix* pfbxmtxSource)
{
	FbxVector4 S = pfbxmtxSource->GetS();
	FbxVector4 R = pfbxmtxSource->GetR();
	FbxVector4 T = pfbxmtxSource->GetT();

	FbxAMatrix fbxmtxTransform = FbxAMatrix(T, R, S);

	XMFLOAT4X4 xmf4x4Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)(*pfbxmtxSource)[i][j];
	}

	XMFLOAT3 xmf3S = XMFLOAT3((float)S.mData[0], (float)S.mData[1], (float)S.mData[2]);
	XMFLOAT3 xmf3R = XMFLOAT3((float)R.mData[0], (float)R.mData[1], (float)R.mData[2]);
	XMFLOAT3 xmf3T = XMFLOAT3((float)T.mData[0], (float)T.mData[1], (float)T.mData[2]);

	XMMATRIX Rx = XMMatrixRotationX(XMConvertToRadians(xmf3R.x));
	XMMATRIX Ry = XMMatrixRotationY(XMConvertToRadians(xmf3R.y));
	XMMATRIX Rz = XMMatrixRotationZ(XMConvertToRadians(xmf3R.z));
	XMMATRIX xmR = XMMatrixMultiply(XMMatrixMultiply(Rx, Ry), Rz);
	XMFLOAT4X4 xmf4x4Multiply;
	XMStoreFloat4x4(&xmf4x4Multiply, XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(xmf3S.x, xmf3S.y, xmf3S.z), xmR), XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z)));

	return(xmf4x4Result);
}

void SetMatrixScale(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxSrcMatrix[i][j] *= pValue;
	}
}

void SetMatrixAdd(FbxAMatrix& fbxmtxDstMatrix, FbxAMatrix& fbxmtxSrcMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxDstMatrix[i][j] += fbxmtxSrcMatrix[i][j];
	}
}