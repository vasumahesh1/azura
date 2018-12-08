import SolvingPass;

[numthreads(BLOCK_SIZE_X, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
  if (dispatchThreadId.x == 0 && dispatchThreadId.y == 0 && dispatchThreadId.z == 0) {
    clothDataBlock.properties[0].maxX = asuint(-10000.0f);
    clothDataBlock.properties[0].maxY = asuint(-10000.0f);
    clothDataBlock.properties[0].maxZ = asuint(-10000.0f);
    clothDataBlock.properties[0].minX = asuint(10000.0f);
    clothDataBlock.properties[0].minY = asuint(10000.0f);
    clothDataBlock.properties[0].minZ = asuint(10000.0f);
  }

  const float timeDelta = computeUBOBlock.timeDelta;
  
  const uint threadIdx = dispatchThreadId.x;

  if (threadIdx < computeUBOBlock.numVertices) {

    if (constraintBlock.vertexAliases[threadIdx] == -1) {
      clothDataBlock.velocities[threadIdx] = (clothDataBlock.projectedPos[threadIdx] - clothDataBlock.vertices[threadIdx]) / timeDelta;
      // Write Projected Points to Buffer
      clothDataBlock.vertices[threadIdx] = clothDataBlock.projectedPos[threadIdx];

      // Used by Normals
      clothDataBlock.deltaX[threadIdx] = 0;
      clothDataBlock.deltaY[threadIdx] = 0;
      clothDataBlock.deltaZ[threadIdx] = 0;

      clothDataBlock.tanX[threadIdx] = 0;
      clothDataBlock.tanY[threadIdx] = 0;
      clothDataBlock.tanZ[threadIdx] = 0;
      return;
    }

    const int aliasId = constraintBlock.vertexAliases[threadIdx];
    // Write Alias Projected Points to Buffer
    clothDataBlock.vertices[threadIdx] = clothDataBlock.projectedPos[aliasId];
  }
}