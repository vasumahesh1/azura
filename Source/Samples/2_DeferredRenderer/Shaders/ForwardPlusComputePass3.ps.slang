import Common;
import ForwardPlusComputeCommonPass3;

float ExtractFloat(Texture2D tex, uint textureWidth, uint textureHeight, uint index, uint component) {
  float u = float(index + 1) / float(textureWidth + 1);
  uint pixel = component / 4;
  float v = float(pixel + 1) / float(textureHeight + 1);

  float4 texel = tex.Sample(lightSamplerBlock.lightSamp, float2(u, v));

  uint pixelComponent = component - pixel * 4;

  if (pixelComponent == 0) {
    return texel[0];
  } else if (pixelComponent == 1) {
    return texel[1];
  } else if (pixelComponent == 2) {
    return texel[2];
  } else if (pixelComponent == 3) {
    return texel[3];
  }

  return 0.0;
}

Light UnpackLight(int index) {
  Light light;
  float u = float(index + 1) / float(NUM_LIGHTS + 1);
  
  float4 v1 = attachmentBlock.light.Sample(lightSamplerBlock.lightSamp, float2(u, 0.0));
  float4 v2 = attachmentBlock.light.Sample(lightSamplerBlock.lightSamp, float2(u, 0.5));

  light.position = v1.xyz;
  light.radius = ExtractFloat(attachmentBlock.light, NUM_LIGHTS, 2, index, 3);
  light.color = v2.rgb;
  
  return light;
}

float3 ShadeLightByIndex(int index, float3 worldPos, float3 normal, float3 albedo) {
  Light light = UnpackLight(index);

  float lightDistance = distance(light.position, worldPos);

  float3 L = (light.position - worldPos) / lightDistance;

  float lightIntensity = cubicGaussian(2.0 * lightDistance / light.radius);
  float lambertTerm = max(dot(L, normal), 0.0);
  return albedo * lambertTerm * light.color * float3(lightIntensity);
}

float4 main(QuadInput_PS input) : SV_Target
{
  float2 screenUV = input.uv;
  screenUV.y = 1.0 - screenUV.y;

  const float3 albedo = attachmentBlock.gbuffer1.Sample(samplerBlock.samp, screenUV).rgb;
  const float3 normal = attachmentBlock.gbuffer2.Sample(samplerBlock.samp, screenUV).rgb;
  const float3 worldPos = attachmentBlock.gbuffer3.Sample(samplerBlock.samp, screenUV).rgb;

  const uint tileX = floor((screenUV.x * SCREEN_WIDTH) / WIDTH_PER_TILE);
  const uint tileY = floor((screenUV.y * SCREEN_HEIGHT) / HEIGHT_PER_TILE);

  const uint fragClusterIdx = ((tileX * TILES_Y) + tileY);
  // const float linearTile = fragClusterIdx / (TILES_X * TILES_Y);

  // const float2 clusterStartUV = float2(linearTile, 0);
  // const float4 clusterStart = attachmentBlock.clusterTexture.Sample(lightSamplerBlock.lightSamp, clusterStartUV);
  // const uint numLights = clusterStart.r;

  float numLights = ExtractFloat(attachmentBlock.clusterTexture, TILES_X * TILES_Y, CLUSTER_PIXEL_HEIGHT, fragClusterIdx, 0);

  float3 fragColor = float3(0.0);

  uint height = 4;
  // for (uint i = 0; i < numLights; ++i) {
  //   float lightIdx = ExtractFloat(attachmentBlock.clusterTexture, TILES_X * TILES_Y, CLUSTER_PIXEL_HEIGHT, fragClusterIdx, height);
  //   fragColor += ShadeLightByIndex(int(lightIdx), worldPos, normal, albedo);
  //   ++height;
  // }

  // float4 normalColor = float4((normal + float3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  // float4 uvColor = float4(input.uv.x, input.uv.y, 0.0, 1.0);

  fragColor += albedo * AMBIENT_LIGHT_INTENSITY;

  return float4(fragColor, 1.0);
}
