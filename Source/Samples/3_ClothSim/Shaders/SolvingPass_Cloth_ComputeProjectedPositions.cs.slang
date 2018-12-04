import SolvingPass;

static const float3 GRAVITY = {0, -2.8, 0};

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

	const float3 vertexPosition = clothDataBlock.vertices[threadIdx];

	clothDataBlock.deltaX[threadIdx] = 0;
	clothDataBlock.deltaY[threadIdx] = 0;
	clothDataBlock.deltaZ[threadIdx] = 0;
	clothDataBlock.constraintCount[threadIdx] = 0;

	const float invMass = constraintBlock.invMasses[threadIdx];
	if (invMass < EPSILON) // ANCHORS
	{
		// Move Anchors by Model Matrix
		clothDataBlock.projectedPos[threadIdx] = mul(computeUBOBlock.modelMatrix, float4(vertexPosition, 1.0)).xyz;
		return;
	}

	const float3 updatedVelocity = 0.996 * (clothDataBlock.velocities[threadIdx] + timeDelta * GRAVITY);
	// Projected Pos of that Vertex
	clothDataBlock.projectedPos[threadIdx] = vertexPosition + updatedVelocity * timeDelta;
}