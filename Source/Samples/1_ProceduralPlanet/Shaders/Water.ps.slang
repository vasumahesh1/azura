import Common;

static const float TEXTURE_GRID_SIZE = 4;
static const float3 TEXTURE_GRID_VEC = float3(1.0 / TEXTURE_GRID_SIZE, 1.0 / TEXTURE_GRID_SIZE, 1.0 / TEXTURE_GRID_SIZE);

float4 main(QuadInput_PS input) : SV_Target
{
  float4 fragNoiseData = attachments.target1.Sample(textureState.linearSampler, input.uv);

  if (fragNoiseData.x <= 0.0) {
    discard;
  }

  float4 fragModelPos = attachments.target2.Sample(textureState.linearSampler, input.uv);

  const float waterThreshold = controls.u_waterControls.x - 0.5;
  const float elevation = (0.5 / controls.u_elevation) * 5.0;

  float noiseValue = fragNoiseData.w * elevation;
  const bool isVertexWater = noiseValue < waterThreshold;

  if (!isVertexWater) {
    discard;
  }
  
  // float4 fragNormal = attachments.target3.Sample(textureState.linearSampler, input.uv);

  // float ambientTerm = 0.2;

  // float3 lightVec = (controls.u_lightPos - fragModelPos).xyz;

  // float3 normalDir = float3((fragNormal.xyz + float3(1,1,1)) / 2.0) * TEXTURE_GRID_VEC;
  // float4 diffuseColor = float4(textureState.target.Sample(textureState.linearSampler, normalDir.xy + fragNoiseData.yz).rgb, 1.0);

  // float diffuseTerm = dot(normalize(fragNormal.xyz), normalize(lightVec));
  // diffuseTerm = clamp(diffuseTerm, 0.0, 1.0);

  // float lightIntensity = ambientTerm + diffuseTerm;

  // float4 finalColor = float4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
  // finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  // finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  // finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  // Debug Normals
  // finalColor = float4((fragNormal.xyz + float3(1,1,1)) / 2.0, 1.0);
  // finalColor = float4(1.0, 1.0, 0.0, 1.0);
  // finalColor = float4((normalize(lightVec) + float3(1,1,1)) / 2.0, 1.0);

  return float4(0, 0, 1, 0.5);
}
