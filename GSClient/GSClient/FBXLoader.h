#pragma once

class FBXLoader
{

};

extern FbxAMatrix XmFloat4x4MatrixToFbxMatrix(XMFLOAT4X4& xmf4x4Source);

extern FbxScene* LoadFbxScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxManager* pfbxSdkManager, char* pstrFbxFileName);
extern void LoadFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pfbxNode);
extern void CreateFbxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FbxNode* pfbxNode);

extern void DrawFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxMesh* pfbxMesh, FbxAMatrix& fbxmtxNodeToRoot, FbxAMatrix& fbxmtxGeometryOffset, FbxAMatrix fbxmtxWorld);

extern void RenderFbxMesh(ID3D12GraphicsCommandList* pd3dCommandList, FbxMesh* pfbxMesh, FbxAMatrix& fbxmtxNodeToRoot, FbxAMatrix& fbxmtxGeometryOffset, FbxAMatrix fbxmtxWorld);
extern void RenderFbxNodeHierarchy(ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pfbxNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld);
