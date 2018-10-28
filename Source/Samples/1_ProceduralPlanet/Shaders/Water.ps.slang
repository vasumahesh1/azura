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

  // float4 fragModelPos = attachments.target2.Sample(samplerBlock.linearSampler, input.uv);

  const float waterThreshold = controls.u_waterControls.x - 0.5;
  const float elevation = (0.5 / controls.u_elevation) * 5.0;

  float noiseValue = fragNoiseData.w * elevation;
  const bool isVertexWater = noiseValue < waterThreshold;

  if (!isVertexWater) {
    discard;
  }

  float4 fragNormal = attachments.target2.Sample(samplerBlock.linearSampler, input.uv);
  float4 depthBuffer = attachments.depthTarget.Sample(samplerBlock.linearSampler, input.uv);
  float ndcDepth = depthBuffer.r;

  float2 ndcXY = (input.uv) * 2.0 - 1.0;
  float4 viewSpacePoint = mul(ubo.u_invProj, float4(ndcXY.x, ndcXY.y, ndcDepth, 1.0));
  viewSpacePoint = viewSpacePoint / viewSpacePoint.w;

  float4 viewSpaceOrigin = mul(ubo.u_view, mul(ubo.u_model, float4(0, 0, 0, 1)));
  float3 viewSpaceLight = mul(ubo.u_view, controls.u_lightPos).xyz;

  float3 lightVec = normalize(viewSpaceLight - viewSpacePoint.xyz);

  // Dot between Original Normal & Light Vector from surface point
  float diffuseTerm = dot(normalize(lightVec), normalize(viewSpacePoint.xyz - viewSpaceOrigin.xyz));
  
  diffuseTerm = clamp(diffuseTerm, 0.2, 1.0);

  // Debug Normals
  // finalColor = float4((fragNormal.xyz + float3(1,1,1)) / 2.0, 1.0);
  // finalColor = float4(1.0, 1.0, 0.0, 1.0);
  // finalColor = float4((normalize(lightVec) + float3(1,1,1)) / 2.0, 1.0);

  return float4(controls.u_waterColor.rgb * diffuseTerm, controls.u_waterControls.y);
}
