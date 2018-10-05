import Common;

ParameterBlock<ShaderControls> controls;

float4 main(PixelShaderInput input) : SV_Target
{
  float ambientTerm = 0.2;

  float4 diffuseColor = float4(0.0, 0.0, 0.0, 0.0);

  // if (input.textureID == BEDROCK_1_TEXTURE_ID) {
  //   diffuseColor = controls.u_waterBedrock1Color;
  // }
  // else if (input.textureID == BEDROCK_2_TEXTURE_ID) {
  //   diffuseColor = controls.u_waterBedrock2Color;
  // }
  // else if (input.textureID == SAND_TEXTURE_ID) {
  //   diffuseColor = controls.u_sandColor;
  // }
  // else if (input.textureID == SNOW_TEXTURE_ID) {
  //   diffuseColor = float4(0.8, 0.8, 0.8, 1.0);
  // }
  // else if (input.textureID == GRASS_1_TEXTURE_ID) {
  //   diffuseColor = float4(0.2, 0.8, 0.2, 1.0);
  // }
  // else if (input.textureID == GRASS_2_TEXTURE_ID) {
  //   diffuseColor = float4(0.2, 0.4, 0.2, 1.0);
  // }
  // else if (input.textureID == ROCK_TEXTURE_ID) {
  //   diffuseColor = float4(0.8, 0.8, 0.1, 1.0);
  // }

  float alpha = diffuseColor.a;

  float diffuseTerm = dot(normalize(input.normal.xyz), normalize(input.lightVec.xyz));
  diffuseTerm = clamp(diffuseTerm, 0.0, 1.0);

  float lightIntensity = ambientTerm + diffuseTerm;

  float4 finalColor = float4(diffuseColor.rgb * lightIntensity, alpha);
  finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  // Debug Normals
  // finalColor = float4((input.normal.xyz + float3(1,1,1)) / 2.0, 1.0);

  return finalColor;
}