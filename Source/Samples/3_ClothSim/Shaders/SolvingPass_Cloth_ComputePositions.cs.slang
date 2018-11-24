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

  if (threadIdx < computeUBOBlock.numVertices) {
    clothDataBlock.velocities[threadIdx] = (clothDataBlock.projectedPos[threadIdx] - clothDataBlock.vertices[threadIdx]) / timeDelta;
    // Write Projected Points to Buffer
    clothDataBlock.vertices[threadIdx] = clothDataBlock.projectedPos[threadIdx];

    // Used by Normals
    clothDataBlock.deltaX[threadIdx] = 0;
    clothDataBlock.deltaY[threadIdx] = 0;
    clothDataBlock.deltaZ[threadIdx] = 0;

    // clothDataBlock.tanX[threadIdx] = 0;
    // clothDataBlock.tanY[threadIdx] = 0;
    // clothDataBlock.tanZ[threadIdx] = 0;
  }
}