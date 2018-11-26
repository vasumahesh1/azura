import ShadingPass;

const static float3 FOG_COLOR = float3(0.1, 0.1, 0.1);

float4 main(PlanePixelInput input) : SV_Target
{
  // float4 diffuseColor = float4(1.0, 0.33, 0.05, 1.0);

  float4 modelSpacePoint = input.modelPos;
  float4 diffuseColor = textureBlock.albedo.Sample(samplerBlock.linearSampler, input.uv);
  float4 normalColor = textureBlock.normal.Sample(samplerBlock.linearSampler, input.uv);
  float4 roughness = textureBlock.roughness.Sample(samplerBlock.linearSampler, input.uv);
  float4 aoShadow = textureBlock.ao.Sample(samplerBlock.linearSampler, input.uv);
  float3 textureNormal = normalColor.xyz * 2.0 - 1.0;

  const float3 bitangent = float3(1, 0, 0);
  const float3 tangent = float3(0, 0, 1);
  float3x3 tbnT = float3x3(bitangent, tangent, input.normal);

  float3 fragNormal = normalize(mul(tbnT, textureNormal.rgb));

  float3 lightVec = normalize(light.u_pos.xyz - modelSpacePoint.xyz);
  float4 cameraPos = -mul(ubo.u_view, float4(0, 0, 0, 1));
  float3 eyeVec = normalize(cameraPos.xyz - modelSpacePoint.xyz);

  Material material;
  material.albedo = diffuseColor;
  material.specular = float3(0, 0, 0);
  material.roughness = roughness.r;

  Light lightObj;
  lightObj.color = float4(1.0, 1.0, 1.0, 1.0);
  lightObj.position = light.u_pos.xyz;
  lightObj.lightVector = lightVec;
  lightObj.intensity = 1.0;

  Attributes attributes;
  attributes.position = input.modelPos.xyz;
  attributes.uv = input.uv;
  attributes.normal = fragNormal;
  attributes.binormal = bitangent;
  attributes.tangent = tangent;

  float NdotL = saturate(dot(fragNormal, lightObj.lightVector));

  float3 diffuse = NdotL * Disney(attributes, lightObj, material, eyeVec) * lightObj.color.rgb * lightObj.intensity + AMBIENT_TERM;

  float3 specular = NdotL * GGX(attributes, lightObj, material, eyeVec) * lightObj.color.rgb * lightObj.intensity;

  float visibility = aoShadow.r;

  float3 accumulatedColor = material.albedo.rgb * diffuse.rgb * visibility + (specular * visibility);
  accumulatedColor = FinalGamma(accumulatedColor);
  
  // accumulatedColor = (attributes.normal + float3(1.0, 1.0, 1.0)) / 2.0;
  // accumulatedColor = float3(aoShadow.r, aoShadow.r, aoShadow.r);
  // accumulatedColor = float3(material.roughness, material.roughness, material.roughness);
  // accumulatedColor = diffuseColor.xyz;

  float4 finalColor = float4(accumulatedColor, material.albedo.a);
  
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
