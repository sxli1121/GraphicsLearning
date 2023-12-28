#include "Terrain.h"
#include "UtilityHelper.h"
#include "MyTexture2D.h"
#include "RenderList.h"

Terrain::~Terrain()
{
    _Clear();
}

bool Terrain::LoadHeightMap(const char* hm, float unitsize, float _floor, float _top)
{
    _Clear();

    // 加载位图
    MyTexture2D heightmap; 
    if (heightmap.LoadTexture(hm))
    {
        const int& w = heightmap.Width();
        const int& h = heightmap.Height();

        if (!(w > 2 && w == h && IS_POWER_OF2(w - 1)))  // 图片尺寸检测
        {
            printf("地形的高度图尺寸不符合要求！");
            return false;
        }

        // 使用meshData中的函数加载灰度图的模型数据
        mPMeshData = new MeshData();
        if (mPMeshData->LoadGrayMap(hm, unitsize, _floor, _top))
        {
            mSize = w;
            mRealSize = unitsize * (w - 1);

            mPVertexts = new vec3f[mPMeshData->mVertextCount];
            gameobject()->transform().NeedUpdate() = true;
            return true;
        }
        else
        {
            delete mPMeshData;
            mPMeshData = nullptr;
            printf("通过高度图加载地图数据失败\n");
        }
    }
    else
    {
        printf("高度图加载失败\n");
    }
    return false;
}

void Terrain::DrawTerrain()
{
    const vec3f campos = Camera::mainCamera->gameobject()->transform().position();
    int& tc = mPMeshData->mTriangleCount;
    int*& pindex = mPMeshData->mPIndex;

    const Frustum& frustum = Camera::mainCamera->GetWorldFrustum();
    vec3f dir;
    for (int j = 0; j < tc; ++j)
    {
        Triangle& t = TRL.GetNextTriangle();
        t.p1 = mPVertexts[pindex[j * 3]];
        t.p2 = mPVertexts[pindex[j * 3 + 1]];
        t.p3 = mPVertexts[pindex[j * 3 + 2]];

        if (frustum.TriangleInFrustum(t))
        {
            dir = t.p1 - campos;
            if (t.CheckFront(dir))
            {
                t.uv1 = mPMeshData->mPUVs[pindex[j * 3]];
                t.uv2 = mPMeshData->mPUVs[pindex[j * 3 + 1]];
                t.uv3 = mPMeshData->mPUVs[pindex[j * 3 + 2]];
                t.ptexture = mPMeshData->mPTexture;
                TRL.AddTriangle();
            }
        }
    }
}

void Terrain::UpdateTerrain()
{
    Transform& transform = gameobject()->transform();
    bool& isNeedUpdate = transform.NeedUpdate();
    if (isNeedUpdate)
    {
        if (mPMeshData == nullptr)
        {
            return;
        }
        Mat4x4f matworld = transform.MatS() * transform.MatR() * transform.MatT();

        //获取顶点的数量
        int& vertextCount = mPMeshData->mVertextCount;
        vec3f*& pVertexts = mPMeshData->mPVertexts;
        for (int i = 0; i < vertextCount; ++i)
        {
            mPVertexts[i] = pVertexts[i] * matworld;
        }
        isNeedUpdate = true;
    }
}

void Terrain::_Clear()
{
    SAFE_DELETE_ARRAY_PTR(mPVertexts);   // 自己管理地形的模型数据
    delete mPMeshData;
    mPMeshData = nullptr;
}
