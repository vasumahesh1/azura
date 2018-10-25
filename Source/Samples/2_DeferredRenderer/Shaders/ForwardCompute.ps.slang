import Common;
import ForwardComputeCommon;

static const float3 ambientLight = float3(0.1);

float ExtractFloat(Texture2D tex, int textureWidth, int textureHeight, int index, int component) {
  float u = float(index + 1) / float(textureWidth + 1);
  int pixel = component / 4;
  float v = float(pixel + 1) / float(textureHeight + 1);

  float4 texel = tex.Sample(samplerBlock.lightSamp, float2(u, v));

  int pixelComponent = component - pixel * 4;

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
  
  float4 v1 = attachmentBlock.light.Sample(samplerBlock.lightSamp, float2(u, 0.0));
  float4 v2 = attachmentBlock.light.Sample(samplerBlock.lightSamp, float2(u, 0.5));

  light.position = v1.xyz;
  light.radius = ExtractFloat(attachmentBlock.light, NUM_LIGHTS, 2, index, 3);
  light.color = v2.rgb;
  
  return light;
}

float4 main(ColorPixelInput input) : SV_Target
{
  float3 albedo = textureBlock.tex.Sample(samplerBlock.samp, input.uv).rgb;
  float3 normalFromMap = textureBlock.norm.Sample(samplerBlock.samp, input.uv).rgb;
  float3 normalFromGeo = input.normal;

  float3 finalNormal = applyNormalMap(normalFromGeo, normalFromMap);

  float3 fragColor = float3(0.0);

  for (int i = 0; i < NUM_LIGHTS; ++i) {
    Light light = UnpackLight(i);

    float lightDistance = distance(light.position, input.worldPos.xyz);

    float3 L = (light.position - input.worldPos.xyz) / lightDistance;

    float lightIntensity = cubicGaussian(2.0 * lightDistance / light.radius);
    float lambertTerm = max(dot(L, finalNormal), 0.0);
    fragColor += albedo * lambertTerm * light.color * float3(lightIntensity);
  }

  // float4 normalColor = float4((finalNormal + float3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  // float4 uvColor = float4(input.uv.x, input.uv.y, 0.0, 1.0);

  fragColor += albedo * ambientLight;

  return float4(fragColor, 1.0);
}
