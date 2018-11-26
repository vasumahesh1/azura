import ShadingPass;

const static float3 FOG_COLOR = float3(0.1, 0.1, 0.1);

float4 main(PlanePixelInput input) : SV_Target
{
  // float4 diffuseColor = float4(1.0, 0.33, 0.05, 1.0);

  float4 modelSpacePoint = input.modelPos;
  float4 diffuseColor = textureBlock.albedo.Sample(samplerBlock.linearSampler, input.uv);
  float4 normalColor = textureBlock.normal.Sample(samplerBlock.linearSampler, input.uv);
  float4 aoColor = textureBlock.ao.Sample(samplerBlock.linearSampler, input.uv);
  float3 textureNormal = normalColor.xyz * 2.0 - 1.0;

  float3x3 tbnT = float3x3(float3(1, 0, 0), float3(0, 0, 1), input.normal);

  float3 fragNormal = normalize(mul(tbnT, textureNormal.rgb));

  float3 lightVec = light.u_pos.xyz - modelSpacePoint.xyz;

  float diffuseTerm = dot(normalize(fragNormal), normalize(lightVec));
  diffuseTerm = clamp(diffuseTerm, 0.0, 1.0) * aoColor.r;

  float lightIntensity = AMBIENT_TERM + diffuseTerm;

  float4 finalColor = float4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
  finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  
  float dist = length(modelSpacePoint.xyz);

  if (dist > 50.0) {
    dist = dist - 50.0;
    float power = dist * 0.1;

    // Exponential Fog but start only some units ahead of the player
    // 1 - exp(-length(wpos - cpos) * c)
    float fogFactor = 1.0 - exp(-power);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    finalColor.rgb = lerp(finalColor.rgb, FOG_COLOR, fogFactor);
  }


  return finalColor;
}
