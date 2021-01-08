#pragma once
class CFrameResource
{
public: 
    CFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount);
    CFrameResource(const CFrameResource& rhs) = delete;
    CFrameResource& operator=(const CFrameResource& rhs) = delete;
    ~CFrameResource();

    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    // std::unique_ptr<UploadBuffer<FrameConstants>> FrameCB = nullptr;
    // std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    // std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
    // std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    // We cannot update a dynamic vertex buffer until the GPU is done processing
    // the commands that reference it.  So each frame needs their own.
    // std::unique_ptr<UploadBuffer<Vertex>> WavesVB = nullptr;

    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 Fence = 0;
};

