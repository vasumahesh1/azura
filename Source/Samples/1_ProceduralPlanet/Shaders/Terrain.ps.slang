import Common;

[[vk::binding(0, 1)]] ParameterBlock<ShaderControls> controls;

[[vk::binding(0, 2)]] SamplerState     linearSampler : register(s2);
[[vk::binding(1, 2)]] Texture2D        targetTexture : register(t2);

static const float TEXTURE_GRID_SIZE = 4;
static const float3 TEXTURE_GRID_VEC = float3(1.0 / TEXTURE_GRID_SIZE, 1.0 / TEXTURE_GRID_SIZE, 1.0 / TEXTURE_GRID_SIZE);

float4 main(PixelShaderInput input) : SV_Target
{
  float ambientTerm = 0.2;

  float3 lightVec = (controls.u_lightPos - input.modelPos).xyz;

  float3 normalDir = float3((input.normal.xyz + float3(1,1,1)) / 2.0) * TEXTURE_GRID_VEC;
  float4 diffuseColor = float4(targetTexture.Sample(linearSampler, normalDir.xy + input.textureOffset).rgb, 1.0);

  float diffuseTerm = dot(normalize(input.normal.xyz), normalize(lightVec));
  diffuseTerm = clamp(diffuseTerm, 0.0, 1.0);

  float lightIntensity = ambientTerm + diffuseTerm;

  float4 finalColor = float4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
  finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  // Debug Normals
  // finalColor = float4((input.normal.xyz + float3(1,1,1)) / 2.0, 1.0);
  // finalColor = float4((normalize(lightVec) + float3(1,1,1)) / 2.0, 1.0);

  return finalColor;
}