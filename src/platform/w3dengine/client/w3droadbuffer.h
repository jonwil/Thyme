/**
 * @file
 *
 * @author Jonathan Wilson
 *
 * @brief Road Drawing Code
 *
 * @copyright Thyme is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "always.h"
#include "vector2.h"
#include "asciistring.h"
#include "sphere.h"
#include "multilist.h"
class TextureClass;
class DX8VertexBufferClass;
class DX8IndexBufferClass;
class WorldHeightMap;
class RenderObjClass;
struct VertexFormatXYZDUV1;
class CameraClass;
struct IRegion2D;

class RoadType
{
private:
    TextureClass *m_roadTexture;
    DX8VertexBufferClass *m_vertexRoad;
    DX8IndexBufferClass *m_indexRoad;
    int m_numRoadVertices;
    int m_numRoadIndices;
    int m_uniqueID;
    bool m_isAutoLoaded;
    int m_stackingOrder;
    Utf8String m_texturePath;

public:
    RoadType();
    ~RoadType();
    void loadTexture(Utf8String path, int ID);
    void applyTexture();
    int getStacking() { return m_stackingOrder; }
    void setStacking(int stacking) { m_stackingOrder = stacking; }
    int getUniqueID() { return m_uniqueID; }
    DX8VertexBufferClass *getVB() { return m_vertexRoad; }
    DX8IndexBufferClass *getIB() { return m_indexRoad; }
    int getNumVertices() { return m_numRoadVertices; }
    void setNumIndices(int indices) { m_numRoadIndices = indices; }
    void setNumVertices(int vertices) { m_numRoadVertices = vertices; }
    int getNumIndices() { return m_numRoadIndices; }
    void setAutoLoaded() { m_isAutoLoaded = true; }
    int isAutoLoaded() { return m_isAutoLoaded; }
    Utf8String getPath() { return m_texturePath; }
    void loadTestTexture();
};

struct TRoadPt
{
    Vector2 loc;
    Vector2 top;
    Vector2 bottom;
    int last;
    char multi;
    char count;
    char isAngled;
    char isJoin;
};

enum TCorner
{
    SEGMENT,
    CURVE,
    TEE,
    FOUR_WAY,
    THREE_WAY_Y,
    THREE_WAY_H,
    THREE_WAY_H_FLIP,
    ALPHA_JOIN,
    NUM_JOINS,
};

struct TRoadSegInfo
{
    Vector2 loc;
    Vector2 roadNormal;
    Vector2 roadVector;
    Vector2 corners[4];
    float uOffset;
    float vOffset;
    float scale;
};

class RoadSegment
{
private:
    TRoadPt m_pt1;
    TRoadPt m_pt2;
    float m_widthInTexture;
    TCorner m_type;
    float m_scale;
    float m_curveRadius;
    int m_uniqueID;
    bool m_isVisible;
    int m_numVertex;
    VertexFormatXYZDUV1 *m_vb;
    int m_numIndex;
    unsigned short *m_ib;
    TRoadSegInfo m_info;
    SphereClass m_Bounds;

public:
    RoadSegment();
    ~RoadSegment();
    void SetVertexBuffer(VertexFormatXYZDUV1 *vb, int numVertex);
    void SetIndexBuffer(unsigned short *ib, int numIndex);
    void SetRoadSegInfo(TRoadSegInfo *info) { m_info = *info; }
    void GetRoadSegInfo(TRoadSegInfo *info) { *info = m_info; }
    SphereClass &getBounds() { return m_Bounds; }
    int GetNumVertex() { return m_numVertex; }
    int GetNumIndex() { return m_numIndex; }
    int GetVertices(VertexFormatXYZDUV1 *destination_vb, int numToCopy);
    int GetIndices(unsigned short *destination_ib, int numToCopy, int offset);
    void updateSegLighting();
    void flip();
};

class W3DRoadBuffer
{
private:
    RoadType *m_roadTypes;
    RoadSegment *m_roads;
    int m_numRoads;
    bool m_initialized;
    WorldHeightMap *m_map;
    RefMultiListIterator<RenderObjClass> *m_lightsIterator;
    int m_curUniqueID;
    int m_curRoadType;
    int m_maxUID;
    int m_curOpenRoad;
    int m_maxRoadSegments;
    int m_maxRoadVertex;
    int m_maxRoadIndex;
    int m_maxRoadTypes;
    int m_curNumRoadVertices;
    int m_curNumRoadIndices;
    bool m_Dirty;

public:
    W3DRoadBuffer();
    ~W3DRoadBuffer();
    void loadRoads();
    void clearAllRoads();
    void drawRoads(CameraClass *camera, TextureClass *cloudTexture, TextureClass *noiseTexture, int wireframe, int minX,
        int maxX, int minY, int maxY, RefMultiListIterator<RenderObjClass> *pDynamicLightsIterator);
    void setMap(WorldHeightMap *pMap);
    void updateLighting();
    void addMapObjects();
    void addMapObject(RoadSegment *pRoad, int updateTheCounts);
    void adjustStacking(int topUniqueID, int bottomUniqueID);
    int findCrossTypeJoinVector(Vector2 loc, Vector2 *joinVector, int uniqueID);
    void insertCurveSegmentAt(int ndx1, int ndx2);
    void insertCrossTypeJoins();
    void miter(int ndx1, int ndx2);
    void moveRoadSegTo(int fromNdx, int toNdx);
    void checkLinkAfter(int ndx);
    void checkLinkBefore(int ndx);
    void updateCounts(RoadSegment *pRoad);
    void updateCountsAndFlags();
    void insertCurveSegments();
    void insertTeeIntersections();
    void insertTee(Vector2 loc, int index1, float scale);
    int insertY(Vector2 loc, int index1, float scale);
    void insert4Way(Vector2 loc, int index1, float scale);
    void offset4Way(TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, TRoadPt *pr3, TRoadPt *pc4, Vector2 loc, Vector2 alignVector,
        float widthInTexture);
    void offset3Way(
        TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, Vector2 loc, Vector2 upVector, Vector2 teeVector, float widthInTexture);
    void offsetY(TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, Vector2 loc, Vector2 upVector, float widthInTexture);
    void offsetH(TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, Vector2 loc, Vector2 upVector, Vector2 teeVector, int flip,
        int mirror, float widthInTexture);
    void preloadRoadsInVertexAndIndexBuffers();
    void preloadRoadSegment(RoadSegment *pRoad);
    void loadCurve(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, float scale);
    void loadTee(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, int is4Way, float scale);
    void loadY(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, float scale);
    void loadAlphaJoin(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, float scale);
    void loadH(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, int flip, float scale);
    void loadFloatSection(RoadSegment *pRoad, Vector2 loc, Vector2 roadVector, float halfHeight, float left, float right,
        float uOffset, float vOffset, float scale);
    void loadFloat4PtSection(RoadSegment *pRoad, Vector2 loc, Vector2 roadNormal, Vector2 roadVector, Vector2 *cornersP,
        float uOffset, float vOffset, float uScale, float vScale);
    void loadLit4PtSection(RoadSegment *pRoad, unsigned short *ib, VertexFormatXYZDUV1 *vb,
        RefMultiListIterator<RenderObjClass> *pDynamicLightsIterator);
    void loadRoadsInVertexAndIndexBuffers();
    void loadLitRoadsInVertexAndIndexBuffers(RefMultiListIterator<RenderObjClass> *pDynamicLightsIterator);
    void loadRoadSegment(unsigned short *ib, VertexFormatXYZDUV1 *vb, RoadSegment *pRoad);
    void allocateRoadBuffers();
    void freeRoadBuffers();
    void rotateAbout(Vector2 *ptP, Vector2 center, float angle);
    bool visibilityChanged(const IRegion2D &region);
    void updateCenter();
    void setDirty();
};

#ifdef GAME_DLL
#include "hooker.h"
#endif
