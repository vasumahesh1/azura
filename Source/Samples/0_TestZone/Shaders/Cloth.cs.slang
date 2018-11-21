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

// void InterlockedAddFloat<T>(uint addr, float value)
// {
//   uint i_val = asuint(value);
//   uint tmp0 = 0;
//   uint tmp1;

//   [allow_uav_condition] while(true)
//   {
// 		InterlockedCompareExchange(T[addr], tmp0, i_val, tmp1);

// 		if (tmp1 == tmp0)
// 		  break;

// 		tmp0 = tmp1;
// 		i_val = asuint(value + asfloat(tmp1));
//   }
// }

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

			// [allow_uav_condition] while (true) {
			// 	uint swappedValue;
			// 	InterlockedCompareExchange(clothDataBlock.deltaLock[dist.indexA], 0, 1, swappedValue);
			// 	bool isSet = (swappedValue == 0);
			// 	if (isSet) {
			// 		clothDataBlock.delta[dist.indexA] += float4(delta1, 0.0);
			// 		uint dummy;
			// 		InterlockedCompareExchange(clothDataBlock.deltaLock[dist.indexA], 1, 0, dummy);
			// 		break;
			// 	}
			// }

			// do {
			// 	uint swappedValue;
			// 	InterlockedCompareExchange(clothDataBlock.deltaLock[dist.indexB], 0, 1, swappedValue);
			// 	bool isSet = (swappedValue == 0);
			// 	if (isSet) {
			// 		clothDataBlock.delta[dist.indexB] += float4(delta2, 0.0);
			// 		clothDataBlock.deltaLock[dist.indexB] = 0;
			// 		break;
			// 	}
			// } while (true);

			// InterlockedAdd(clothDataBlock.deltaX[dist.indexA], ToInt(delta1.x, MAX_FLOAT_RANGE, MIN_FLOAT_RANGE));
			// InterlockedAdd(clothDataBlock.deltaY[dist.indexA], ToInt(delta1.y, MAX_FLOAT_RANGE, MIN_FLOAT_RANGE));
			// InterlockedAdd(clothDataBlock.deltaZ[dist.indexA], ToInt(delta1.z, MAX_FLOAT_RANGE, MIN_FLOAT_RANGE));

			// InterlockedAdd(clothDataBlock.deltaX[dist.indexB], ToInt(delta2.x, MAX_FLOAT_RANGE, MIN_FLOAT_RANGE));
			// InterlockedAdd(clothDataBlock.deltaY[dist.indexB], ToInt(delta2.y, MAX_FLOAT_RANGE, MIN_FLOAT_RANGE));
			// InterlockedAdd(clothDataBlock.deltaZ[dist.indexB], ToInt(delta2.z, MAX_FLOAT_RANGE, MIN_FLOAT_RANGE));

			// delta1.x = (delta1.x + 10.0) * 1000.0;
			// delta1.y = (delta1.y + 10.0) * 1000.0;
			// delta1.z = (delta1.z + 10.0) * 1000.0;

			// delta2.x = (delta2.x + 10.0) * 1000.0;
			// delta2.y = (delta2.y + 10.0) * 1000.0;
			// delta2.z = (delta2.z + 10.0) * 1000.0;

			// InterlockedAdd(clothDataBlock.deltaX[dist.indexA], uint(delta1.x));
			// InterlockedAdd(clothDataBlock.deltaY[dist.indexA], uint(delta1.y));
			// InterlockedAdd(clothDataBlock.deltaZ[dist.indexA], uint(delta1.z));

			// InterlockedAdd(clothDataBlock.deltaX[dist.indexB], uint(delta2.x));
			// InterlockedAdd(clothDataBlock.deltaY[dist.indexB], uint(delta2.y));
			// InterlockedAdd(clothDataBlock.deltaZ[dist.indexB], uint(delta2.z));

			uint i_val = asuint(delta1.x);
			uint tmp0 = 0;

			[allow_uav_condition] while(true)
			{
				uint tmp1;
				InterlockedCompareExchange(clothDataBlock.deltaX[dist.indexA], tmp0, i_val, tmp1);

				if (tmp1 == tmp0)
				  break;

				tmp0 = tmp1;
				i_val = asuint(delta1.x + asfloat(tmp1));
			}

			i_val = asuint(delta1.y);
			tmp0 = 0;

			[allow_uav_condition] while(true)
			{
				uint tmp1;
				InterlockedCompareExchange(clothDataBlock.deltaY[dist.indexA], tmp0, i_val, tmp1);

				if (tmp1 == tmp0)
				  break;

				tmp0 = tmp1;
				i_val = asuint(delta1.y + asfloat(tmp1));
			}

			i_val = asuint(delta1.z);
			tmp0 = 0;

			[allow_uav_condition] while(true)
			{
				uint tmp1;
				InterlockedCompareExchange(clothDataBlock.deltaZ[dist.indexA], tmp0, i_val, tmp1);

				if (tmp1 == tmp0)
				  break;

				tmp0 = tmp1;
				i_val = asuint(delta1.z + asfloat(tmp1));
			}

			// Index B

			i_val = asuint(delta2.x);
			tmp0 = 0;

			[allow_uav_condition] while(true)
			{
				uint tmp1;
				InterlockedCompareExchange(clothDataBlock.deltaX[dist.indexB], tmp0, i_val, tmp1);

				if (tmp1 == tmp0)
				  break;

				tmp0 = tmp1;
				i_val = asuint(delta2.x + asfloat(tmp1));
			}

			i_val = asuint(delta2.y);
			tmp0 = 0;

			[allow_uav_condition] while(true)
			{
				uint tmp1;
				InterlockedCompareExchange(clothDataBlock.deltaY[dist.indexB], tmp0, i_val, tmp1);

				if (tmp1 == tmp0)
				  break;

				tmp0 = tmp1;
				i_val = asuint(delta2.y + asfloat(tmp1));
			}

			i_val = asuint(delta2.z);
			tmp0 = 0;

			[allow_uav_condition] while(true)
			{
				uint tmp1;
				InterlockedCompareExchange(clothDataBlock.deltaZ[dist.indexB], tmp0, i_val, tmp1);

				if (tmp1 == tmp0)
				  break;

				tmp0 = tmp1;
				i_val = asuint(delta2.z + asfloat(tmp1));
			}


			// InterlockedAddFloat<clothDataBlock.deltaX>(dist.indexA, delta1.x);
			// InterlockedAddFloat<clothDataBlock.deltaY>(dist.indexA, delta1.y);
			// InterlockedAddFloat<clothDataBlock.deltaZ>(dist.indexA, delta1.z);

			// InterlockedAddFloat<clothDataBlock.deltaX>(dist.indexB, delta2.x);
			// InterlockedAddFloat<clothDataBlock.deltaY>(dist.indexB, delta2.y);
			// InterlockedAddFloat<clothDataBlock.deltaZ>(dist.indexB, delta2.z);
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