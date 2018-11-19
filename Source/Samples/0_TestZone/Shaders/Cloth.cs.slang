import ClothComputeCommon;

static const float4 GRAVITY = {0, -0.8, 0, 0};

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
	if (threadIdx < computeUBOBlock.numVertices) {
		clothDataBlock.projectedPos[threadIdx] = clothDataBlock.vertices[threadIdx];

		if (threadIdx != 0 && threadIdx != 110) // HARD CODED
		{
			clothDataBlock.velocities[threadIdx] = clothDataBlock.velocities[threadIdx] + timeDelta * 1.0f * GRAVITY;
			
			// Projected Pos of that Vertex
			clothDataBlock.projectedPos[threadIdx] = clothDataBlock.projectedPos[threadIdx] + clothDataBlock.velocities[threadIdx] * timeDelta;
		}

		clothDataBlock.constraintCount[threadIdx] = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	if (threadIdx < computeUBOBlock.numStretchConstraints) {
		for (uint solverItr = 0; solverItr < SOLVER_ITERATIONS; ++solverItr) {
			const DistanceConstraint dist = constraintBlock.distanceConstraints[threadIdx]; 

			const float invMassSum = (dist.invMass1 + dist.invMass2);
			const float invMassFactor1 = dist.invMass1 / invMassSum;
			const float invMassFactor2 = dist.invMass2 / invMassSum;
		    const float3 x12 = (clothDataBlock.projectedPos[dist.indexA] - clothDataBlock.projectedPos[dist.indexB]).xyz;

		    const float3 delta1 = (computeUBOBlock.stretchStiffness * -1.0f * invMassFactor1 * (length(x12) - dist.restLength)) * normalize(x12);
		    const float3 delta2 = (computeUBOBlock.stretchStiffness * invMassFactor2 * (length(x12) - dist.restLength)) * normalize(x12);

		    bool isSet;
			do {
				uint swappedValue;
				InterlockedCompareExchange(clothDataBlock.deltaLock[dist.indexA], 0, 1, swappedValue);
				isSet = (swappedValue == 0);
				if (isSet) {
					clothDataBlock.delta1[dist.indexA] += float4(delta1, 0.0);
					clothDataBlock.deltaLock[dist.indexA] = 0;
				}
			} while (isSet);

			do {
				uint swappedValue;
				InterlockedCompareExchange(clothDataBlock.deltaLock[dist.indexB], 0, 1, swappedValue);
				isSet = (swappedValue == 0);
				if (isSet) {
					clothDataBlock.delta1[dist.indexB] += float4(delta2, 0.0);
					clothDataBlock.deltaLock[dist.indexB] = 0;
				}
			} while (isSet);

			InterlockedAdd(clothDataBlock.constraintCount[dist.indexA], 1);
		    InterlockedAdd(clothDataBlock.constraintCount[dist.indexB], 1);

			GroupMemoryBarrierWithGroupSync();

			if (threadIdx < computeUBOBlock.numVertices && clothDataBlock.constraintCount[threadIdx] > 0)
			{
				clothDataBlock.delta1[threadIdx] = clothDataBlock.delta1[threadIdx] / float(clothDataBlock.constraintCount[threadIdx]);
			}

		    // clothDataBlock.deltas[dist.indexA] += float4(delta1, 0.0);
		    // clothDataBlock.deltas[dist.indexB] += float4(delta2, 0.0);

		    // bool isSet = false;

		    // do {
		    // 	isSet = InterlockedCompareStore(clothDataBlock.deltaLock[dist.indexA], 0, 1) == 0;

		    // 	if (currentVal == 0)

		    // } while(isSet);

		 	//    uint indexA = 0;
			// InterlockedAdd(clothDataBlock.deltaLock[dist.indexA], 1, indexA);
			// if (indexA == 0) {
			// 	clothDataBlock.delta1[dist.indexA] += float4(delta1, 0.0);
			// }
			// else {
			// 	clothDataBlock.delta2[dist.indexA] += float4(delta1, 0.0);
			// }

			// uint indexB = 0;
			// InterlockedAdd(clothDataBlock.deltaLock[dist.indexB], 1, indexB);
			// if (indexB == 0) {
			// 	clothDataBlock.delta1[dist.indexB] += float4(delta2, 0.0);
			// }
			// else {
			// 	clothDataBlock.delta2[dist.indexB] += float4(delta2, 0.0);
			// }
		}
	}

	GroupMemoryBarrierWithGroupSync();

	if (threadIdx < computeUBOBlock.numVertices) {
		// Write Projected Points to Buffer
		clothDataBlock.vertices[threadIdx] = clothDataBlock.projectedPos[threadIdx] + clothDataBlock.delta1[threadIdx];
	}
}