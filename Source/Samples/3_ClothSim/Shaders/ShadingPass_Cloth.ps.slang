import ShadingPass;

float4 main(NormalPixelInput input) : SV_Target
{
  float4 modelSpacePoint = input.modelPos;

  float4 diffuseColor = textureBlock.albedo.Sample(samplerBlock.linearSampler, input.uv);
  float4 normalColor = textureBlock.normal.Sample(samplerBlock.linearSampler, input.uv);
  float4 roughness = textureBlock.roughness.Sample(samplerBlock.linearSampler, input.uv);
  float4 aoShadow = textureBlock.ao.Sample(samplerBlock.linearSampler, input.uv);
  float3 textureNormal = normalColor.xyz * 2.0 - 1.0;

  float3 bitangent = normalize(cross(-input.tangent, input.normal));

  float3x3 tbnT = float3x3(bitangent, input.tangent, input.normal);

  float3 fragNormal = normalize(mul(tbnT, textureNormal.rgb));

  float3 lightVec = normalize(light.u_pos.xyz - modelSpacePoint.xyz);
  float4 cameraPos = -mul(ubo.u_view, float4(0, 0, 0, 1));
  float3 eyeVec = normalize(cameraPos.xyz - modelSpacePoint.xyz);

  Material material;
  material.albedo = diffuseColor;
  material.specular = float3(1,1,1);
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
  attributes.tangent = normalize(input.tangent);

  float NdotL = saturate(dot(fragNormal, lightObj.lightVector));

  float3 diffuse = NdotL * Disney(attributes, lightObj, material, eyeVec) * lightObj.color.rgb * lightObj.intensity + AMBIENT_TERM;

  float3 specular = NdotL * GGX(attributes, lightObj, material, eyeVec) * lightObj.color.rgb * lightObj.intensity;

  float visibility = aoShadow.r;

  float3 finalColor = material.albedo.rgb * diffuse.rgb * visibility + (specular * visibility);

  float ExposureBias = 7.0;
  float3 curr = Uncharted2Tonemap(ExposureBias * finalColor);

  float3 whiteScale = 1.0 / Uncharted2Tonemap(W);
  finalColor = curr * whiteScale;

  finalColor = FinalGamma(finalColor);
  return float4(finalColor, material.albedo.a);

  // float diffuseTerm = dot(normalize(fragNormal), normalize(lightVec));
  // diffuseTerm = abs(diffuseTerm) * aoShadow.r;

  // float lightIntensity = AMBIENT_TERM + diffuseTerm;

  // float4 finalColor = float4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
  // finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  // finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  // finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  // finalColor = float4((input.tangent + float3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  // finalColor = float4(input.uv.x, input.uv.y, 0.0, 1.0);
  // finalColor = diffuseColor;

  // return finalColor;
}
