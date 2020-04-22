/**
 * @file
 *
 * @author Jonathan Wilson
 *
 * @brief Index Buffer classes
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

class IndexBufferClass : public W3DMPO, public RefCountClass
{
public:
    class WriteLockClass
    {
        IndexBufferClass *index_buffer;
        unsigned short *indices;

    public:
        WriteLockClass(IndexBufferClass *index_buffer_, unsigned int flags);
        ~WriteLockClass();
        unsigned short *Get_Index_Array() { return indices; }
    };
    class AppendLockClass
    {
        IndexBufferClass *index_buffer;
        unsigned short *indices;

    public:
        AppendLockClass(IndexBufferClass *index_buffer_, unsigned int start_index, unsigned int index_range);
        ~AppendLockClass();
        unsigned short *Get_Index_Array() { return indices; }
    };

protected:
    int engine_refs;
    unsigned short index_count;
    unsigned int type;

public:
    virtual ~IndexBufferClass();
    IndexBufferClass(unsigned int type_, unsigned short index_count_);
    void Add_Engine_Ref() { engine_refs++; }
    void Release_Engine_Ref() { engine_refs--; }
    void Copy(unsigned short *indices, unsigned int first_index, unsigned int count);
    void Copy(unsigned int *indices, unsigned int first_index, unsigned int count);
    static unsigned int Get_Total_Buffer_Count();
    static unsigned int Get_Total_Allocated_Indices();
    static unsigned int Get_Total_Allocated_Memory();
};

class DX8IndexBufferClass : public IndexBufferClass
{
    IMPLEMENT_W3D_POOL(DX8IndexBufferClass)
private:
#ifdef BUILD_WITH_D3D8
    IDirect3DIndexBuffer8 *index_buffer;
#endif

public:
    enum UsageType
    {
        USAGE_DEFAULT,
        USAGE_DYNAMIC,
        USAGE_SOFTWAREPROCESSING,
        USAGE_NPATCHES,
    };
    ~DX8IndexBufferClass();
    DX8IndexBufferClass(unsigned short index_count_, UsageType usage);
#ifdef BUILD_WITH_D3D8
    IDirect3DIndexBuffer8 *Get_DX8_Index_Buffer() { return index_buffer; }
#endif
};

class SortingIndexBufferClass : public IndexBufferClass
{
    IMPLEMENT_W3D_POOL(SortingIndexBufferClass)
private:
    unsigned short *index_buffer;

public:
    SortingIndexBufferClass(unsigned short index_count_);
    ~SortingIndexBufferClass();
};

class DynamicIBAccessClass : public W3DMPO
{
    IMPLEMENT_W3D_POOL(DynamicIBAccessClass)
public:
    class WriteLockClass
    {
        DynamicIBAccessClass *DynamicIBAccess;
        unsigned short *Indices;

    public:
        WriteLockClass(DynamicIBAccessClass *ib_access_);
        ~WriteLockClass();
        unsigned short *Get_Index_Array() { return Indices; }
    };

private:
    unsigned int Type;
    unsigned short IndexCount;
    unsigned short IndexBufferOffset;
    IndexBufferClass *IndexBuffer;

public:
    DynamicIBAccessClass(unsigned short type_, unsigned short index_count_);
    ~DynamicIBAccessClass();
    void Allocate_Sorting_Dynamic_Buffer();
    void Allocate_DX8_Dynamic_Buffer();
    static void _Deinit();
    static void _Reset(bool frame_changed);
    static unsigned short Get_Default_Index_Count();
};
