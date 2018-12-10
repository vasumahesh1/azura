import SolvingPass;

[numthreads(BLOCK_SIZE_X, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
	const uint threadIdx = dispatchThreadId.x;

	// Update all points
	if (threadIdx >= CLOTH_GRID_X * CLOTH_GRID_Y * CLOTH_GRID_Z) {
		return;
	}

	clothDataBlock.binCount[threadIdx] = 0;
}