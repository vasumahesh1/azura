#pragma pack_matrix(column_major)

#line 4 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass2.cs.slang"
static groupshared uint groupLightCount_0;


static groupshared uint minDepthInt_0;


#line 8
static groupshared uint maxDepthInt_0;


#line 1 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
static const uint NUM_LIGHTS_0 = (uint) 256;

static const uint BLOCK_SIZE_0 = (uint) 16;
static const uint BLOCK_SIZE_1 = (uint) 16;

#line 18
static const uint THREAD_COUNT_0 = BLOCK_SIZE_0 * BLOCK_SIZE_1;

static const uint LIGHTS_PER_THREAD_0 = (NUM_LIGHTS_0 + THREAD_COUNT_0 - (uint) 1) / THREAD_COUNT_0;

#line 66 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass2.cs.slang"
RWTexture2D<vector<float,4> > rwBlock_lightTexture_0 : register(u0);

RWTexture2D<vector<float,4> > rwBlock_clusterTexture_0 : register(u1);


#line 119 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
struct LightUBO_0
{
    matrix<float,4,4>  view_0;
    float nearPlane_0;
    float farPlane_0;
    float timeDelta_0;
    float pad0;

    matrix<float,4,4>  proj_0;
};


#line 3 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputeCommonPass2.slang"
cbuffer _S1 : register(b0)
{
    LightUBO_0 lightUBO_0 : packoffset(c0);
}

#line 6 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
static const float LIGHT_DT_0 = -2.50000000000000000000;
static const vector<float,3> LIGHT_MIN_0 = vector<float,3>(-5.00000000000000000000, 0.00000000000000000000, -5.00000000000000000000);
static const vector<float,3> LIGHT_MAX_0 = vector<float,3>(5.00000000000000000000, 15.00000000000000000000, 5.00000000000000000000);

#line 81 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass2.cs.slang"
Texture2D<float > attachmentBlock_depthBuffer_0 : register(t0);


#line 12 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
static const uint TILES_Y_0 = (uint) 16;

#line 131
struct Plane_0
{
    vector<float,3> normal_0;
    float dist_0;
};


#line 136
struct Frustum_0
{
    Plane_0  planes_0[4];
};


#line 105 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass2.cs.slang"
StructuredBuffer<Frustum_0 > frustumBlock_frustums_0 : register(t1);


#line 12
bool IsSphereBehindPlane_0(vector<float,3> center_0, float radius_0, Plane_0 plane_0)
{
    float _S2 = dot(plane_0.normal_0, center_0);

#line 14
    return _S2 - plane_0.dist_0 < -radius_0 - 0.00010000000000000000;
}

bool ComputeFrustumIntersection_0(vector<float,3> lightViewPos_0, float lightRadius_0, Frustum_0 frustum_0, float minZ_0, float maxZ_0)
{
    bool result_0;
    int i_0;
    bool result_1;
    bool result_2;

    if(lightViewPos_0.z + lightRadius_0 < maxZ_0)
    {
        result_0 = false;
    }
    else
    {
        result_0 = true;
    }
    i_0 = 0;
    result_1 = result_0;
    for(;;)
    {

#line 28
        if(i_0 < 4 && result_1)
        {
        }
        else
        {
            break;
        }

#line 29
        bool _S3 = IsSphereBehindPlane_0(lightViewPos_0, lightRadius_0, frustum_0.planes_0[i_0]);

#line 29
        if(_S3)
        {
            result_2 = false;
        }
        else
        {
            result_2 = result_1;
        }

#line 28
        i_0 = i_0 + 1;
        result_1 = result_2;
    }



    return result_1;
}


#line 4 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
static const uint MAX_LIGHTS_PER_CLUSTER_0 = (uint) 1020;

#line 5 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass2.cs.slang"
static groupshared uint  groupLightIndices_0[1020];


#line 39
[numthreads(16, 16, 1)]
void main(vector<uint,3> groupID_0 : SV_GROUPID, vector<uint,3> groupThreadId_0 : SV_GROUPTHREADID, uint groupIndex_0 : SV_GROUPINDEX, vector<uint,3> dispatchThreadId_0 : SV_DISPATCHTHREADID)
{
    uint idx_0;
    uint lightIdx_0;


    if(groupIndex_0 == (uint) 0)
    {

#line 47
        groupLightCount_0 = (uint) 0;
        minDepthInt_0 = (uint) 4294967295;
        maxDepthInt_0 = (uint) 0;

#line 46
    }
    else
    {
    }


    GroupMemoryBarrierWithGroupSync();



    if(groupID_0.x == (uint) 0 && groupID_0.y == (uint) 0)
    {
        idx_0 = (uint) 0;
        for(;;)
        {

#line 57
            if(idx_0 < LIGHTS_PER_THREAD_0)
            {
            }
            else
            {
                break;
            }

#line 58
            uint lightIdx_1 = idx_0 * THREAD_COUNT_0 + groupIndex_0;

            if(lightIdx_1 >= NUM_LIGHTS_0)
            {

#line 61
                break;
            }

            vector<float,2> upper_0 = vector<float,2>((float) lightIdx_1, (float) 0);

            vector<float,4> currentLightPosRad_0;

#line 66
            vector<float,4> _S4 = rwBlock_lightTexture_0[(vector<uint,2>) upper_0];

#line 66
            currentLightPosRad_0 = _S4;

            currentLightPosRad_0.y = currentLightPosRad_0.y + LIGHT_DT_0 * lightUBO_0.timeDelta_0;

            if(currentLightPosRad_0.y < LIGHT_MIN_0.y)
            {

#line 71
                currentLightPosRad_0.y = LIGHT_MAX_0.y;

#line 70
            }
            else
            {
            }
            rwBlock_lightTexture_0[(vector<uint,2>) upper_0] = currentLightPosRad_0;

#line 57
            idx_0 = idx_0 + 1;
        }

#line 56
    }
    else
    {
    }

#line 81
    float _S5 = attachmentBlock_depthBuffer_0.Load((vector<int,3>) vector<uint,3>(dispatchThreadId_0.xy, (uint) 0));

    // Linearize the depth value from depth buffer (must do this because we created it using projection)
    _S5.x = (0.5 * lightUBO_0.proj_0[3][2]) / (_S5.x + 0.5 * lightUBO_0.proj_0[2][2] - 0.5);
    
    uint depthInt_0 = asuint(_S5.x);

    GroupMemoryBarrierWithGroupSync();

#line 86
    InterlockedMin(minDepthInt_0, depthInt_0);

#line 86
    InterlockedMax(maxDepthInt_0, depthInt_0);

    GroupMemoryBarrierWithGroupSync();

    float minGroupDepth_0 = asfloat(minDepthInt_0);
    float maxGroupDepth_0 = asfloat(maxDepthInt_0);

    // minGroupDepth_0 = minGroupDepth_0 * 2.0 - 1.0;
    // maxGroupDepth_0 = maxGroupDepth_0 * 2.0 - 1.0;

    // vector<float,4> minDepthVec_0 = mul(lightUBO_0.proj_0, vector<float,4>((float) 0, (float) 0, minGroupDepth_0, (float) 1));
    // vector<float,4> maxDepthVec_0 = mul(lightUBO_0.proj_0, vector<float,4>((float) 0, (float) 0, maxGroupDepth_0, (float) 1));

    // vector<float,4> minDepthVec_1 = minDepthVec_0 / minDepthVec_0.w;
    // vector<float,4> maxDepthVec_1 = maxDepthVec_0 / maxDepthVec_0.w;

    // float _S7 = minDepthVec_1.z;
    // float _S8 = maxDepthVec_1.z;
    float _S123 = lightUBO_0.nearPlane_0;

    float _S7 = -minGroupDepth_0;
    float _S8 = maxGroupDepth_0;


    uint frustIdx_0 = groupID_0.x * TILES_Y_0 + groupID_0.y;
    Frustum_0 _S9 = frustumBlock_frustums_0[frustIdx_0];
    lightIdx_0 = groupIndex_0;
    for(;;)
    {

#line 108
        if(lightIdx_0 < NUM_LIGHTS_0)
        {
        }
        else
        {
            break;
        }

#line 110
        vector<float,2> _S10 = vector<float,2>((float) lightIdx_0, (float) 1);

        vector<float,4> currentLightPosRad_1 = rwBlock_lightTexture_0[(vector<uint,2>) vector<float,2>((float) lightIdx_0, (float) 0)];

        vector<float, 4> temp = mul(lightUBO_0.view_0, vector<float,4>(currentLightPosRad_1.xyz, 1.00000000000000000000));

        bool _S11 = ComputeFrustumIntersection_0(temp.xyz, currentLightPosRad_1.w, _S9, _S7, _S8);

#line 116
        if(_S11)
        {
            uint _S12 = (uint) 1;

#line 118
            uint _S13;

#line 118
            InterlockedAdd(groupLightCount_0, _S12, _S13);

#line 118
            uint offset_0 = _S13;

            if(offset_0 < MAX_LIGHTS_PER_CLUSTER_0)
            {

#line 121
                groupLightIndices_0[offset_0] = lightIdx_0;

#line 120
            }
            else
            {
            }

#line 116
        }
        else
        {
        }

#line 108
        lightIdx_0 = lightIdx_0 + THREAD_COUNT_0;
    }

#line 126
    GroupMemoryBarrierWithGroupSync();


    uint _S14 = groupLightCount_0 / (uint) 4;

#line 129
    float _S15 = floor((float) _S14);

#line 129
    uint fullSets_0 = (uint) (_S15 * (float) 4);
    uint remainingSets_0 = groupLightCount_0 - fullSets_0;
    uint fullSetIdx_0 = fullSets_0 / (uint) 4;


    if(groupIndex_0 == (uint) 0)
    {
        rwBlock_clusterTexture_0[(vector<uint,2>) vector<float,2>((float) frustIdx_0, (float) 0)] = vector<float,4>((float) groupLightCount_0, 0.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000);


        vector<float,4> lastEncode_0;

#line 139
        lastEncode_0 = vector<float,4>(0.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000, 0.00000000000000000000);
        vector<float,2> remainingUV_0 = vector<float,2>((float) frustIdx_0, (float) (fullSetIdx_0 + (uint) 1));
        if(remainingSets_0 >= (uint) 1)
        {

#line 142
            lastEncode_0.x = (float) groupLightIndices_0[fullSets_0];

#line 141
        }
        else
        {
        }
        if(remainingSets_0 >= (uint) 2)
        {

#line 146
            lastEncode_0.y = (float) groupLightIndices_0[fullSets_0 + (uint) 1];

#line 145
        }
        else
        {
        }
        if(remainingSets_0 >= (uint) 3)
        {

#line 150
            lastEncode_0.z = (float) groupLightIndices_0[fullSets_0 + (uint) 2];

#line 149
        }
        else
        {
        }
        rwBlock_clusterTexture_0[(vector<uint,2>) remainingUV_0] = lastEncode_0;

#line 134
    }
    else
    {
    }

#line 157
    if(groupIndex_0 < fullSetIdx_0)
    {
        uint indices_0 = (uint) 4 * groupIndex_0;
        rwBlock_clusterTexture_0[(vector<uint,2>) vector<float,2>((float) frustIdx_0, (float) (groupIndex_0 + (uint) 1))] = vector<float,4>((float) groupLightIndices_0[indices_0], (float) groupLightIndices_0[indices_0 + (uint) 1], (float) groupLightIndices_0[indices_0 + (uint) 2], (float) groupLightIndices_0[indices_0 + (uint) 3]);

#line 157
    }
    else
    {
    }

#line 39
    return;
}

