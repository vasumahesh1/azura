import ClothComputeCommon;

static const float4 GRAVITY = {0, -0.5, 0, 0};

static const float MIN_FLOAT_RANGE = 0.0;
static const float MAX_FLOAT_RANGE = 100.0;

int ToInt(float value, float maxRange, float minRange)
{
	// return int((value - minRange) * (float(0x7fffffff) / (maxRange - minRange)));
	return int(float((value - minRange) / (maxRange - minRange)) * int(0x7fffffff));
}

float FromInt(int value, float maxRange, float minRange) {
	// return (float(value) * ((maxRange - minRange) / float(0x7fffffff))) + minRange;
	return ((float(value) / int(0x7fffffff)) * (maxRange - minRange)) + minRange;
}

uint ToUint(float value, float maxRange, float minRange)
{
	return uint(((value - minRange) / (maxRange - minRange)) * 0xffffffff);
}

float FromUint(uint value, float maxRange, float minRange) {
	return ((float(value) / float(0xffffffff))  * (maxRange - minRange))  + minRange;
	// return (float(value) * ((maxRange - minRange) / float(0x7fffffff))) + minRange;
}

void AtomicAddDeltaX(uint addr, float value)
{
  uint i_val = asuint(value);
  uint tmp0 = 0;
  uint tmp1;

  [allow_uav_condition] while(true)
  {
		InterlockedCompareExchange(clothDataBlock.deltaX[addr], tmp0, i_val, tmp1);

		if (tmp1 == tmp0)
		  break;

		tmp0 = tmp1;
		i_val = asuint(value + asfloat(tmp1));
  }
}

void AtomicAddDeltaY(uint addr, float value)
{
  uint i_val = asuint(value);
  uint tmp0 = 0;
  uint tmp1;

  [allow_uav_condition] while(true)
  {
		InterlockedCompareExchange(clothDataBlock.deltaY[addr], tmp0, i_val, tmp1);

		if (tmp1 == tmp0)
		  break;

		tmp0 = tmp1;
		i_val = asuint(value + asfloat(tmp1));
  }
}

void AtomicAddDeltaZ(uint addr, float value)
{
  uint i_val = asuint(value);
  uint tmp0 = 0;
  uint tmp1;

  [allow_uav_condition] while(true)
  {
		InterlockedCompareExchange(clothDataBlock.deltaZ[addr], tmp0, i_val, tmp1);

		if (tmp1 == tmp0)
		  break;

		tmp0 = tmp1;
		i_val = asuint(value + asfloat(tmp1));
  }
}

float3 ComputeBendingGradient(float4x4 Q, uint indices[4], uint rowI)
{
  float3 sum = float3(0.0, 0.0, 0.0);
  for (uint idj = 0 ; idj < 4; ++idj)
  {
    sum += Q[rowI][idj] * clothDataBlock.projectedPos[indices[idj]].xyz;
  }

  return sum;
}

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

		if (threadIdx != 0 && threadIdx != 110) // HARD CODED ANCHOR
		{
			const float4 updatedVelocity = clothDataBlock.velocities[threadIdx] + timeDelta * 1.0 * GRAVITY;

			// Projected Pos of that Vertex
			clothDataBlock.projectedPos[threadIdx] = clothDataBlock.vertices[threadIdx] + updatedVelocity * timeDelta;
			clothDataBlock.velocities[threadIdx] = updatedVelocity;
		}
	}

	AllMemoryBarrierWithGroupSync();

	for (uint solverItr = 0; solverItr < SOLVER_ITERATIONS; ++solverItr) {
		if (threadIdx < computeUBOBlock.numVertices) {
			clothDataBlock.constraintCount[threadIdx] = 0;
			// clothDataBlock.delta[threadIdx] = float4(0,0,0,0);
			clothDataBlock.deltaX[threadIdx] = 0;
			clothDataBlock.deltaY[threadIdx] = 0;
			clothDataBlock.deltaZ[threadIdx] = 0;
		}

		AllMemoryBarrierWithGroupSync();
		
		if (threadIdx < computeUBOBlock.numStretchConstraints) {
			DistanceConstraint dist = constraintBlock.distanceConstraints[threadIdx];

			InterlockedAdd(clothDataBlock.constraintCount[dist.indexA], 1);
			InterlockedAdd(clothDataBlock.constraintCount[dist.indexB], 1);

			const float invMassSum = (dist.invMass1 + dist.invMass2);
			const float invMassFactor1 = dist.invMass1 / invMassSum;
			const float invMassFactor2 = dist.invMass2 / invMassSum;
			const float3 x12 = (clothDataBlock.projectedPos[dist.indexA] - clothDataBlock.projectedPos[dist.indexB]).xyz;

			const float3 delta1 = (computeUBOBlock.stretchStiffness * -1.0 * invMassFactor1 * (length(x12) - dist.restLength)) * normalize(x12);
			const float3 delta2 = (computeUBOBlock.stretchStiffness * invMassFactor2 * (length(x12) - dist.restLength)) * normalize(x12);

			AtomicAddDeltaX(dist.indexA, delta1.x);
			AtomicAddDeltaY(dist.indexA, delta1.y);
			AtomicAddDeltaZ(dist.indexA, delta1.z);

			AtomicAddDeltaX(dist.indexB, delta2.x);
			AtomicAddDeltaY(dist.indexB, delta2.y);
			AtomicAddDeltaZ(dist.indexB, delta2.z);
		}

		int bendingOffset = threadIdx - computeUBOBlock.numStretchConstraints;

		if (bendingOffset < computeUBOBlock.numBendConstraints && bendingOffset >= 0) {
			BendConstraint bend = constraintBlock.bendConstraints[bendingOffset];

			float cX = 0.0;

			for (int idx = 0; idx < 4; ++idx) {
				for (int idy = 0; idy < 4; ++idy) {
					cX += bend.Q[idx][idy] * dot(clothDataBlock.projectedPos[bend.indices[idx]].xyz, clothDataBlock.projectedPos[bend.indices[idy]].xyz);
				}
			}

			cX = cX / 2.0;

			float3 partialDeltas[4];
			float sum = 0.0;

			for (uint idx = 0; idx < 4; ++idx) {
				partialDeltas[idx] = ComputeBendingGradient(bend.Q, bend.indices, idx);
				sum += (bend.invMasses[idx] * dot(partialDeltas[idx], partialDeltas[idx]));
			}

			if (abs(sum) > EPSILON) {
				for (uint idx = 0; idx < 4; ++idx) {
					float3 delta = (computeUBOBlock.stretchStiffness * -1.0 * cX * bend.invMasses[idx] * partialDeltas[idx]) / sum;
					AtomicAddDeltaX(bend.indices[idx], delta.x);
					AtomicAddDeltaY(bend.indices[idx], delta.y);
					AtomicAddDeltaZ(bend.indices[idx], delta.z);
				}
			}
			
		}

		AllMemoryBarrierWithGroupSync();

		if (threadIdx < computeUBOBlock.numVertices && clothDataBlock.constraintCount[threadIdx] > 0)
		{
			// float x = FromInt(clothDataBlock.deltaX[threadIdx], MAX_FLOAT_RANGE, MIN_FLOAT_RANGE);
			// float y = FromInt(clothDataBlock.deltaY[threadIdx], MAX_FLOAT_RANGE, MIN_FLOAT_RANGE);
			// float z = FromInt(clothDataBlock.deltaZ[threadIdx], MAX_FLOAT_RANGE, MIN_FLOAT_RANGE);

			float x = asfloat(clothDataBlock.deltaX[threadIdx]);
			float y = asfloat(clothDataBlock.deltaY[threadIdx]);
			float z = asfloat(clothDataBlock.deltaZ[threadIdx]);

			// float x = (float(clothDataBlock.deltaX[threadIdx]) / 1000.0) - 10.0;
			// float y = (float(clothDataBlock.deltaY[threadIdx]) / 1000.0) - 10.0;
			// float z = (float(clothDataBlock.deltaZ[threadIdx]) / 1000.0) - 10.0;

			float3 averaged = (float3(x, y, z)) / float(clothDataBlock.constraintCount[threadIdx]);
			// float3 averaged = (clothDataBlock.delta[threadIdx].xyz) / float(clothDataBlock.constraintCount[threadIdx]);

			clothDataBlock.projectedPos[threadIdx] = clothDataBlock.projectedPos[threadIdx] + float4(averaged, 0.0);
		}

		AllMemoryBarrierWithGroupSync();

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

	AllMemoryBarrierWithGroupSync();

	if (threadIdx < computeUBOBlock.numVertices) {
		clothDataBlock.velocities[threadIdx] = (clothDataBlock.projectedPos[threadIdx] - clothDataBlock.vertices[threadIdx]) / timeDelta;
		// Write Projected Points to Buffer
		clothDataBlock.vertices[threadIdx] = clothDataBlock.projectedPos[threadIdx];
	}
}