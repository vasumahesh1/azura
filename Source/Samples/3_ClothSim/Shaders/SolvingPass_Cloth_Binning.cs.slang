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

	// Update all points
	if (threadIdx >= computeUBOBlock.numVertices) {
		return;
	}

	// Don't Compute Aliases
	if (constraintBlock.vertexAliases[threadIdx] != -1) {
		return;
	}

	float3 vertexPosition = clothDataBlock.vertices[threadIdx];

	float3 boundMin = float3(clothDataBlock.properties[0].minX, clothDataBlock.properties[0].minY, clothDataBlock.properties[0].minZ);
	float3 boundMax = float3(clothDataBlock.properties[0].maxX, clothDataBlock.properties[0].maxY, clothDataBlock.properties[0].maxZ);

	int gridX = floor(((vertexPosition.x - boundMin.x) / (boundMax.x - boundMin.x + 0.0001)) * CLOTH_GRID_X);
	int gridY = floor(((vertexPosition.y - boundMin.y) / (boundMax.y - boundMin.y + 0.0001)) * CLOTH_GRID_Y);
	int gridZ = floor(((vertexPosition.z - boundMin.z) / (boundMax.z - boundMin.z + 0.0001)) * CLOTH_GRID_Z);

	int targetIdx = LinearizeIndex(gridX, gridY, gridZ);

	uint offset;
	InterlockedAdd(clothDataBlock.binCount[targetIdx], 1, offset);

	if (offset < MAX_VERTICES_PER_BIN) {
		uint writeOffset = MAX_VERTICES_PER_BIN * targetIdx + offset;
		clothDataBlock.binVertices[writeOffset] = threadIdx;
	}
}