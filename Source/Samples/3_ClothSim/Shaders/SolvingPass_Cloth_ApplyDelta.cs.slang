import SolvingPass;

[numthreads(BLOCK_SIZE_X, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
  const float timeDelta = computeUBOBlock.timeDelta;
  const uint threadIdx = dispatchThreadId.x;

  if (threadIdx < computeUBOBlock.numVertices && clothDataBlock.constraintCount[threadIdx] > 0)
  {
    float x = asfloat(clothDataBlock.deltaX[threadIdx]);
    float y = asfloat(clothDataBlock.deltaY[threadIdx]);
    float z = asfloat(clothDataBlock.deltaZ[threadIdx]);

    float3 averaged = (1.5 * float3(x, y, z)) / float(clothDataBlock.constraintCount[threadIdx]);
    clothDataBlock.projectedPos[threadIdx] = clothDataBlock.projectedPos[threadIdx] + averaged;
  }
}