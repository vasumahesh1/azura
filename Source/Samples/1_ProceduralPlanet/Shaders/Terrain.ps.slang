import Common;
import Pass2;

static const float TEXTURE_GRID_SIZE = 4;
static const float3 TEXTURE_GRID_VEC = float3(1.0 / TEXTURE_GRID_SIZE, 1.0 / TEXTURE_GRID_SIZE, 1.0 / TEXTURE_GRID_SIZE);

float4 main(QuadInput_PS input) : SV_Target
{
  float4 fragNoiseData = attachments.target1.Sample(samplerBlock.linearSampler, input.uv);

  if (fragNoiseData.x <= 0.0) {
    discard;
  }

  float4 finalColor;
  float ambientTerm = 0.2;

  float4 fragNormal = attachments.target2.Sample(samplerBlock.linearSampler, input.uv);
  float4 depthBuffer = attachments.depthTarget.Sample(samplerBlock.linearSampler, input.uv);
  float ndcDepth = depthBuffer.r;

  float2 ndcXY = (input.uv) * 2.0 - 1.0;
  float4 viewSpacePoint = mul(ubo.u_invProj, float4(ndcXY.x, ndcXY.y, ndcDepth, 1.0));
  viewSpacePoint = viewSpacePoint / viewSpacePoint.w;

  float3 viewSpaceLight = mul(ubo.u_view, controls.u_lightPos).xyz;
  float3 lightVec = normalize(viewSpaceLight - viewSpacePoint.xyz);

  float3 normalDir = float3((fragNormal.xyz + float3(1,1,1)) / 2.0) * TEXTURE_GRID_VEC;
  float4 diffuseColor = float4(textureState.target.Sample(samplerBlock.linearSampler, normalDir.xy + fragNoiseData.yz).rgb, 1.0);

  float diffuseTerm = dot(normalize(fragNormal.xyz), normalize(lightVec));
  diffuseTerm = clamp(diffuseTerm, 0.0, 1.0);

  float lightIntensity = ambientTerm + diffuseTerm;

  finalColor = float4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
  finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  // Debug Normals
  // finalColor = float4((fragNormal.xyz + float3(1,1,1)) / 2.0, 1.0);
  // finalColor = float4(1.0, 1.0, 0.0, 1.0);
  // finalColor = float4((normalize(lightVec) + float3(1,1,1)) / 2.0, 1.0);

  return finalColor;
}
