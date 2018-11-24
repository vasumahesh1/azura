import ShadingPass;

const static float AMBIENT_TERM = 0.2;

float4 main(NormalPixelInput input) : SV_Target
{
  // float4 diffuseColor = float4(1.0, 0.33, 0.05, 1.0);

  float4 modelSpacePoint = input.modelPos;

  float4 diffuseColor = textureBlock.albedo.Sample(samplerBlock.linearSampler, input.uv);
  float4 normalColor = textureBlock.normal.Sample(samplerBlock.linearSampler, input.uv);
  float4 aoShadow = textureBlock.ao.Sample(samplerBlock.linearSampler, input.uv);
  float3 textureNormal = normalColor.xyz * 2.0 - 1.0;

  float3 bitangent = normalize(cross(-input.tangent, input.normal));

  float3x3 tbnT = float3x3(bitangent, input.tangent, input.normal);

  float3 fragNormal = normalize(mul(tbnT, textureNormal.rgb));

  float3 lightVec = light.u_pos.xyz - modelSpacePoint.xyz;

  float diffuseTerm = dot(normalize(fragNormal), normalize(lightVec));
  diffuseTerm = clamp(diffuseTerm, 0.0, 1.0) * aoShadow.r;

  float lightIntensity = AMBIENT_TERM + diffuseTerm;

  float4 finalColor = float4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
  finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  // finalColor = float4((input.tangent + float3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  // finalColor = float4(input.uv.x, input.uv.y, 0.0, 1.0);
  // finalColor = diffuseColor;

  return finalColor;
}
