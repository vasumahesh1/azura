#pragma pack_matrix(column_major)

#line 57 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass3.ps.slang"
Texture2D<vector<float,4> > attachmentBlock_gbuffer1_0 : register(t0);

Texture2D<vector<float,4> > attachmentBlock_gbuffer2_0 : register(t1);

Texture2D<vector<float,4> > attachmentBlock_gbuffer3_0 : register(t2);

Texture2D<vector<float,4> > attachmentBlock_light_0 : register(t3);

Texture2D<vector<float,4> > attachmentBlock_clusterTexture_0 : register(t4);


#line 57
SamplerState samplerBlock_samp_0 : register(s0);


#line 9 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
static const uint SCREEN_WIDTH_0 = (uint) 1280;

static const uint TILES_X_0 = (uint) 16;



static const uint WIDTH_PER_TILE_0 = SCREEN_WIDTH_0 / TILES_X_0;

#line 10
static const uint SCREEN_HEIGHT_0 = (uint) 720;

static const uint TILES_Y_0 = (uint) 16;



static const uint HEIGHT_PER_TILE_0 = SCREEN_HEIGHT_0 / TILES_Y_0;

#line 109
SamplerState lightSamplerBlock_lightSamp_0 : register(s1);


#line 4 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass3.ps.slang"
float ExtractFloat_0(Texture2D<vector<float,4> > tex_0, uint textureWidth_0, uint textureHeight_0, uint index_0, uint component_0)
{

#line 5
    float u_0 = (float) (index_0 + (uint) 1) / (float) (textureWidth_0 + (uint) 1);
    uint pixel_0 = component_0 / (uint) 4;
    float v_0 = (float) (pixel_0 + (uint) 1) / (float) (textureHeight_0 + (uint) 1);

    vector<float,4> texel_0 = tex_0.Sample(lightSamplerBlock_lightSamp_0, vector<float,2>(u_0, v_0));

    uint pixelComponent_0 = component_0 - pixel_0 * (uint) 4;

    if(pixelComponent_0 == (uint) 0)
    {

#line 14
        return texel_0[0];
    }
    else
    {

#line 15
        if(pixelComponent_0 == (uint) 1)
        {

#line 16
            return texel_0[1];
        }
        else
        {

#line 17
            if(pixelComponent_0 == (uint) 2)
            {

#line 18
                return texel_0[2];
            }
            else
            {

#line 19
                if(pixelComponent_0 == (uint) 3)
                {

#line 20
                    return texel_0[3];
                }

#line 17
            }

#line 15
        }

#line 13
    }

#line 23
    return 0.00000000000000000000;
}


#line 4 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
static const uint MAX_LIGHTS_PER_CLUSTER_0 = (uint) 1020;

#line 24
static const uint CLUSTER_PIXEL_HEIGHT_0 = (uint) 1 + MAX_LIGHTS_PER_CLUSTER_0 / (uint) 4;

#line 1
static const uint NUM_LIGHTS_0 = (uint) 256;

#line 112
struct Light_0
{
    vector<float,3> position_0;
    float radius_0;
    vector<float,3> color_0;
    float _pad_0;
};


#line 26 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass3.ps.slang"
Light_0 UnpackLight_0(int index_1)
{

#line 27
    Light_0 light_0;
    float u_1 = (float) (index_1 + 1) / (float) (NUM_LIGHTS_0 + (uint) 1);

    vector<float,4> v1_0 = attachmentBlock_light_0.Sample(lightSamplerBlock_lightSamp_0, vector<float,2>(u_1, 0.00000000000000000000));
    vector<float,4> v2_0 = attachmentBlock_light_0.Sample(lightSamplerBlock_lightSamp_0, vector<float,2>(u_1, 0.50000000000000000000));

    light_0.position_0 = v1_0.xyz;
    float _S1 = ExtractFloat_0(attachmentBlock_light_0, NUM_LIGHTS_0, (uint) 2, (uint) index_1, (uint) 3);

#line 34
    light_0.radius_0 = _S1;
    light_0.color_0 = v2_0.xyz;

    return light_0;
}


#line 144 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
float cubicGaussian_0(float h_0)
{

#line 145
    if(h_0 < 1.00000000000000000000)
    {

#line 146
        float _S2 = pow(2.00000000000000000000 - h_0, 3.00000000000000000000);

#line 146
        float _S3 = 0.25000000000000000000 * _S2;

#line 146
        float _S4 = pow(1.00000000000000000000 - h_0, 3.00000000000000000000);

#line 146
        return _S3 - _S4;
    }
    else
    {

#line 147
        if(h_0 < 2.00000000000000000000)
        {

#line 148
            float _S5 = pow(2.00000000000000000000 - h_0, 3.00000000000000000000);

#line 148
            return 0.25000000000000000000 * _S5;
        }
        else
        {

#line 150
            return 0.00000000000000000000;
        }
    }
}


#line 40 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass3.ps.slang"
vector<float,3> ShadeLightByIndex_0(int index_2, vector<float,3> worldPos_0, vector<float,3> normal_0, vector<float,3> albedo_0)
{

#line 41
    Light_0 light_1 = UnpackLight_0(index_2);

    float lightDistance_0 = distance(light_1.position_0, worldPos_0);

    vector<float,3> L_0 = (light_1.position_0 - worldPos_0) / lightDistance_0;

    float _S6 = 2.00000000000000000000 * lightDistance_0 / light_1.radius_0;

#line 47
    float lightIntensity_0 = cubicGaussian_0(_S6);
    float _S7 = dot(L_0, normal_0);

#line 48
    float lambertTerm_0 = max(_S7, 0.00000000000000000000);
    return albedo_0 * lambertTerm_0 * light_1.color_0 * (vector<float,3>)(lightIntensity_0);
}


#line 22 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/Common.slang"
static const vector<float,3> AMBIENT_LIGHT_INTENSITY_0 = (vector<float,3>)(0.10000000000000000555);

#line 84
struct QuadInput_PS_0
{
    vector<float,4> pos_0 : SV_Position;
    vector<float,2> uv_0 : UV;
};


#line 52 "X:/AzuraStudio/Source/Samples/2_DeferredRenderer/Shaders/ForwardPlusComputePass3.ps.slang"
vector<float,4> main(QuadInput_PS_0 input_0) : SV_TARGET
{
    uint i_0;
    uint height_0;
    vector<float,3> fragColor_0;

#line 54
    vector<float,2> screenUV_0;

#line 54
    screenUV_0 = input_0.uv_0;
    screenUV_0.y = 1.00000000000000000000 - screenUV_0.y;

    vector<float,4> _S8 = attachmentBlock_gbuffer1_0.Sample(samplerBlock_samp_0, screenUV_0);

#line 57
    vector<float,3> albedo_1 = _S8.xyz;
    vector<float,4> _S9 = attachmentBlock_gbuffer2_0.Sample(samplerBlock_samp_0, screenUV_0);

#line 58
    vector<float,3> _S10 = _S9.xyz;
    vector<float,4> _S11 = attachmentBlock_gbuffer3_0.Sample(samplerBlock_samp_0, screenUV_0);

#line 59
    vector<float,3> _S12 = _S11.xyz;

    float _S13 = screenUV_0.x * (float) SCREEN_WIDTH_0 / (float) WIDTH_PER_TILE_0;

#line 61
    float _S14 = floor(_S13);

#line 61
    uint tileX_0 = (uint) _S14;
    float _S15 = screenUV_0.y * (float) SCREEN_HEIGHT_0 / (float) HEIGHT_PER_TILE_0;

#line 62
    float _S16 = floor(_S15);

    uint fragClusterIdx_0 = tileX_0 * TILES_Y_0 + (uint) _S16;

#line 71
    float _S17 = ExtractFloat_0(attachmentBlock_clusterTexture_0, TILES_X_0 * TILES_Y_0, CLUSTER_PIXEL_HEIGHT_0, fragClusterIdx_0, (uint) 0);
    i_0 = (uint) 0;
    height_0 = (uint) 4;
    fragColor_0 = (vector<float,3>)(0.00000000000000000000);

    [loop]
    for(;;)
    {

#line 76
        if((float) i_0 < _S17)
        {
        }
        else
        {
            break;
        }

#line 77
        float lightIdx_0 = ExtractFloat_0(attachmentBlock_clusterTexture_0, TILES_X_0 * TILES_Y_0, CLUSTER_PIXEL_HEIGHT_0, fragClusterIdx_0, height_0);
        vector<float,3> _S18 = ShadeLightByIndex_0((int) lightIdx_0, _S12, _S10, albedo_1);

#line 78
        vector<float,3> _S19 = fragColor_0 + _S18;
        uint _S20 = height_0 + 1;

#line 76
        i_0 = i_0 + 1;
        height_0 = _S20;
        fragColor_0 = _S19;
    }

#line 87
    return vector<float,4>(fragColor_0 + albedo_1 * AMBIENT_LIGHT_INTENSITY_0, 1.00000000000000000000);
}

