import ShadingPass;

const static float AMBIENT_TERM = 0.2;

float4 main(NormalPixelInput input) : SV_Target
{
  float4 diffuseColor = float4(1.0, 0.33, 0.05, 1.0);

  float3 fragNormal = input.normal;
  float4 modelSpacePoint = input.modelPos;

  float3 lightVec = light.u_pos.xyz - modelSpacePoint.xyz;

  float diffuseTerm = dot(normalize(fragNormal), normalize(lightVec));
  diffuseTerm = clamp(diffuseTerm, 0.0, 1.0);

  float lightIntensity = AMBIENT_TERM + diffuseTerm;

  float4 finalColor = float4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
  finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  // finalColor = float4((fragNormal + float3(1.0, 1.0, 1.0)) / 2.0, 1.0);

  return finalColor;
}
