/**
 * @file
 *
 * @author Jonathan Wilson
 *
 * @brief Vertex Buffer classes
 *
 * @copyright Thyme is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "always.h"
#include "refcount.h"
#include "w3dmpo.h"
#include "vector3.h"
#include "vector2.h"
#include "vector4.h"
class FVFInfoClass;
class VertexFormatXYZNDUV2;
class VertexBufferClass;
class VertexBufferLockClass
{
protected:
    VertexBufferClass *VertexBuffer;
    void *Vertices;

public:
    VertexBufferLockClass(VertexBufferClass *VertexBuffer);
    void *Get_Vertex_Array() const { return Vertices; }
};

class VertexBufferClass : public W3DMPO, public RefCountClass
{
public:
    class WriteLockClass : public VertexBufferLockClass
    {
    public:
        WriteLockClass(VertexBufferClass *VertexBuffer, int Flags);
        ~WriteLockClass();
    };

    class AppendLockClass : public VertexBufferLockClass
    {
    public:
        AppendLockClass(VertexBufferClass *VertexBuffer, unsigned int start_index, unsigned int index_range);
        ~AppendLockClass();
    };

protected:
    unsigned int type;
    unsigned short VertexCount;
    int engine_refs;
    FVFInfoClass *fvf_info;

public:
    VertexBufferClass(unsigned int type_, unsigned int FVF, unsigned short vertex_count_, unsigned int fvf_size);
    ~VertexBufferClass();
    void Add_Engine_Ref() { engine_refs++; }
    void Release_Engine_Ref() { engine_refs--; }
    static unsigned int Get_Total_Buffer_Count();
    static unsigned int Get_Total_Allocated_Indices();
    static unsigned int Get_Total_Allocated_Memory();
};

class DX8VertexBufferClass : public VertexBufferClass
{
    IMPLEMENT_W3D_POOL(DX8VertexBufferClass)
private:
#ifdef BUILD_WITH_D3D8
    IDirect3DVertexBuffer8 *VertexBuffer;
#endif

public:
    enum UsageType
    {
        USAGE_DEFAULT,
        USAGE_DYNAMIC,
        USAGE_SOFTWAREPROCESSING,
        USAGE_NPATCHES,
    };
    ~DX8VertexBufferClass();
    DX8VertexBufferClass(unsigned int FVF, unsigned short vertex_count_, UsageType usage, unsigned int flags);
    DX8VertexBufferClass(
        Vector3 *vertices, Vector3 *normals, Vector2 *tex_coords, unsigned short VertexCount, UsageType usage);
    DX8VertexBufferClass(Vector3 *vertices, Vector2 *tex_coords, unsigned short VertexCount, UsageType usage);
    DX8VertexBufferClass(Vector3 *vertices, Vector3 *normals, Vector4 *diffuse, Vector2 *tex_coords,
        unsigned short VertexCount, UsageType usage);
    DX8VertexBufferClass(
        Vector3 *vertices, Vector4 *diffuse, Vector2 *tex_coords, unsigned short VertexCount, UsageType usage);
    void Copy(Vector3 *loc, Vector3 *norm, Vector2 *uv, unsigned int first_vertex, unsigned int count);
    void Copy(Vector3 *loc, unsigned int first_vertex, unsigned int count);
    void Copy(Vector3 *loc, Vector2 *uv, unsigned int first_vertex, unsigned int count);
    void Copy(Vector3 *loc, Vector3 *norm, unsigned int first_vertex, unsigned int count);
    void Copy(Vector3 *loc, Vector3 *norm, Vector2 *uv, Vector4 *diffuse, unsigned int first_vertex, unsigned int count);
    void Copy(Vector3 *loc, Vector2 *uv, Vector4 *diffuse, unsigned int first_vertex, unsigned int count);
    void Create_Vertex_Buffer(UsageType usage);
};

class SortingVertexBufferClass : public VertexBufferClass
{
private:
    VertexFormatXYZNDUV2 *VertexBuffer;

public:
    ~SortingVertexBufferClass();
    SortingVertexBufferClass(unsigned short VertexCount);
};

class DynamicVBAccessClass
{
public:
    class WriteLockClass
    {
        DynamicVBAccessClass *DynamicVBAccess;
        VertexFormatXYZNDUV2 *Vertices;

    public:
        WriteLockClass(DynamicVBAccessClass *dynamic_vb_access_);
        ~WriteLockClass();
        VertexFormatXYZNDUV2 *Get_Formatted_Vertex_Array() { return Vertices; }
    };

private:
    FVFInfoClass &FVFInfo;
    unsigned int Type;
    unsigned short VertexCount;
    unsigned short VertexBufferOffset;
    VertexBufferClass *VertexBuffer;

public:
    DynamicVBAccessClass(unsigned int t, unsigned int fvf, unsigned short vertex_count_);
    ~DynamicVBAccessClass();
    void Allocate_Sorting_Dynamic_Buffer();
    void Allocate_DX8_Dynamic_Buffer();
    static void _Reset(bool frame_changed);
    static void _Deinit();
    static unsigned short Get_Default_Vertex_Count();
};
