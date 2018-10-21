import Common;
import ForwardCommon;

float3 applyNormalMap(float3 geomnor, float3 normap) {
  normap = normap * 2.0 - 1.0;

  float3 up = normalize(float3(0.001, 1, 0.001));
  float3 surftan = normalize(cross(geomnor, up));
  float3 surfbinor = cross(geomnor, surftan);

  return normap.y * surftan + normap.x * surfbinor + normap.z * geomnor;
}

float4 main(ColorPixelInput input) : SV_Target
{
  float3 albedo = textureBlock.tex.Sample(samplerBlock.samp, input.uv).rgb;
  float3 normalFromMap = textureBlock.norm.Sample(samplerBlock.samp, input.uv).rgb;
  float3 normalFromGeo = input.normal;

  float3 finalNormal = applyNormalMap(normalFromGeo, normalFromMap);

  float3 fragColor = float3(0.0);

  float4 normalColor = float4((finalNormal + float3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  float4 uvColor = float4(input.uv.x, input.uv.y, 0.0, 1.0);

  float3 ambientLight = float3(1.0);
  fragColor += albedo * ambientLight;

  return float4(fragColor, 1.0);
}
