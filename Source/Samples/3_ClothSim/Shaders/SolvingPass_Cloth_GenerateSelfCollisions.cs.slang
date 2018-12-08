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

	// Update all triangles
	if (threadIdx >= computeUBOBlock.numTriangles) {
		return;
	}

	// Don't Compute Aliases
	// if (constraintBlock.vertexAliases[threadIdx] != -1) {
	// 	return;
	// }

}