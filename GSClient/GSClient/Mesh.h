#pragma once

class Mesh
{
public:
    using uint16 = uint16_t;
    using uint32 = uint32_t;

    struct Vertex
    {
        Vertex() {}
        Vertex(
            const XMFLOAT3& p,
            const XMFLOAT3& n,
            const XMFLOAT3& t,
            const XMFLOAT2& uv) :
            Position(p),
            Normal(n),
            TangentU(t),
            TexC(uv) {}
        Vertex(
            float px, float py, float pz,
            float nx, float ny, float nz,
            float tx, float ty, float tz,
            float u, float v) :
            Position(px, py, pz),
            Normal(nx, ny, nz),
            TangentU(tx, ty, tz),
            TexC(u, v) {}

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT3 TangentU;
        XMFLOAT2 TexC;
    };

    struct MeshData
    {
        vector<Vertex> Vertices;
        vector<uint32> Indices32;

        vector<uint16>& GetIndices16()
        {
            if (mIndices16.empty())
            {
                mIndices16.resize(Indices32.size());
                for (size_t i = 0; i < Indices32.size(); ++i)
                    mIndices16[i] = static_cast<uint16>(Indices32[i]);
            }

            return mIndices16;
        }

    private:
        vector<uint16> mIndices16;
    };

    MeshData CreateGrid(float width, float depth, uint32 m, uint32 n);

    ///<summary>
    /// Creates a box centered at the origin with the given dimensions, where each
    /// face has m rows and n columns of vertices.
    ///</summary>
    MeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);

    ///<summary>
    /// Creates a sphere centered at the origin with the given radius.  The
    /// slices and stacks parameters control the degree of tessellation.
    ///</summary>
    MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

    ///<summary>
    /// Creates a geosphere centered at the origin with the given radius.  The
    /// depth controls the level of tessellation.
    ///</summary>
    MeshData CreateGeosphere(float radius, uint32 numSubdivisions);

    ///<summary>
    /// Creates a cylinder parallel to the y-axis, and centered about the origin.  
    /// The bottom and top radius can vary to form various cone shapes rather than true
    // cylinders.  The slices and stacks parameters control the degree of tessellation.
    ///</summary>
    MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

    ///<summary>
    /// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
    ///</summary>
    MeshData CreateQuad(float x, float y, float w, float h, float depth);
private:
    void Subdivide(MeshData& meshData);
    Vertex MidPoint(const Vertex& v0, const Vertex& v1);
    void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
    void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);

};

