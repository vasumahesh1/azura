import ClothComputeCommon;

static const float4 GRAVITY = {0, -0.1, 0, 0};

[numthreads(BLOCK_SIZE_X, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
	const uint idx = dispatchThreadId.x;
	if (idx >= computeUBOBlock.numVertices)
	{
		return;
	}

	float4 delta = GRAVITY * computeUBOBlock.timeDelta;
	clothDataBlock.vertices[idx] += delta;
}