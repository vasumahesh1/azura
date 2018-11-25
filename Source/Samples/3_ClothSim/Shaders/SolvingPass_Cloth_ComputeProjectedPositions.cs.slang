import SolvingPass;

static const float3 GRAVITY = {0, -0.8, 0};

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

	clothDataBlock.projectedPos[threadIdx] = clothDataBlock.vertices[threadIdx];

	clothDataBlock.deltaX[threadIdx] = 0;
	clothDataBlock.deltaY[threadIdx] = 0;
	clothDataBlock.deltaZ[threadIdx] = 0;
	clothDataBlock.constraintCount[threadIdx] = 0;

	const float invMass = constraintBlock.invMasses[threadIdx];
	if (invMass < EPSILON) // ANCHORS
	{
		return;
	}

	const float3 updatedVelocity = 0.999 * (clothDataBlock.velocities[threadIdx] + timeDelta * (1.0 / invMass) * GRAVITY);
	// Projected Pos of that Vertex
	clothDataBlock.projectedPos[threadIdx] = clothDataBlock.vertices[threadIdx] + updatedVelocity * timeDelta;
}