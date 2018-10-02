import Common;

// ParameterBlock<ShaderControls> controls;

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

  float alpha = diffuseColor.a;

  float diffuseTerm = dot(normalize(input.normal.xyz), normalize(input.lightVec.xyz));
  diffuseTerm = clamp(diffuseTerm, 0.0, 1.0);

  float lightIntensity = ambientTerm + diffuseTerm;

  float4 finalColor = float4(diffuseColor.rgb * lightIntensity, alpha);
  finalColor.x = clamp(finalColor.x, 0.0, 1.0);
  finalColor.y = clamp(finalColor.y, 0.0, 1.0);
  finalColor.z = clamp(finalColor.z, 0.0, 1.0);

  return finalColor;
}