import ShadingPass;

float4 main(PlanePixelInput input) : SV_Target
{
  float4 modelSpacePoint = input.modelPos;
  float4 diffuseColor = textureBlock.albedo.Sample(samplerBlock.linearSampler, input.uv);

  return diffuseColor;
}
