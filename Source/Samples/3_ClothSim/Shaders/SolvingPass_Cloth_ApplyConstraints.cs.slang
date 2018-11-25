import SolvingPass;

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
    sum += Q[rowI][idj] * clothDataBlock.projectedPos[indices[idj]];
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

  int bendingOffset = threadIdx - computeUBOBlock.numStretchConstraints;

  // if (bendingOffset >= computeUBOBlock.numBendConstraints)
  // {
  //   return;
  // }

  for (uint solverItr = 0; solverItr < SOLVER_ITERATIONS; ++solverItr) {
    if (threadIdx < computeUBOBlock.numStretchConstraints) {
      DistanceConstraint dist = constraintBlock.distanceConstraints[threadIdx];

      InterlockedAdd(clothDataBlock.constraintCount[dist.indexA], 1);
      InterlockedAdd(clothDataBlock.constraintCount[dist.indexB], 1);

      const float invMassA = constraintBlock.invMasses[dist.indexA];
      const float invMassB = constraintBlock.invMasses[dist.indexB];
      const float invMassSum = (invMassA + invMassB);
      const float invMassFactor1 = invMassA / invMassSum;
      const float invMassFactor2 = invMassB / invMassSum;
      const float3 x12 = (clothDataBlock.projectedPos[dist.indexA] - clothDataBlock.projectedPos[dist.indexB]);

      const float3 delta1 = (computeUBOBlock.stretchStiffness * -1.0 * invMassFactor1 * (length(x12) - dist.restLength)) * normalize(x12);
      const float3 delta2 = (computeUBOBlock.stretchStiffness * invMassFactor2 * (length(x12) - dist.restLength)) * normalize(x12);

      AtomicAddDeltaX(dist.indexA, delta1.x);
      AtomicAddDeltaY(dist.indexA, delta1.y);
      AtomicAddDeltaZ(dist.indexA, delta1.z);

      AtomicAddDeltaX(dist.indexB, delta2.x);
      AtomicAddDeltaY(dist.indexB, delta2.y);
      AtomicAddDeltaZ(dist.indexB, delta2.z);
    }

    if (bendingOffset < computeUBOBlock.numBendConstraints && bendingOffset >= 0) {
      BendConstraint bend = constraintBlock.bendConstraints[bendingOffset];

      float cX = 0.0;

      for (int idx = 0; idx < 4; ++idx) {
        for (int idy = 0; idy < 4; ++idy) {
          cX += bend.Q[idx][idy] * dot(clothDataBlock.projectedPos[bend.indices[idx]], clothDataBlock.projectedPos[bend.indices[idy]]);
        }
      }

      cX = cX / 2.0;

      float3 partialDeltas[4];
      float sum = 0.0;

      for (uint idx = 0; idx < 4; ++idx) {
        partialDeltas[idx] = ComputeBendingGradient(bend.Q, bend.indices, idx);
        sum += (constraintBlock.invMasses[bend.indices[idx]] * dot(partialDeltas[idx], partialDeltas[idx]));
      }

      if (abs(sum) > EPSILON) {
        for (uint idx = 0; idx < 4; ++idx) {
          float3 delta = (computeUBOBlock.bendStiffness * -1.0 * cX * constraintBlock.invMasses[bend.indices[idx]] * partialDeltas[idx]) / sum;
          AtomicAddDeltaX(bend.indices[idx], delta.x);
          AtomicAddDeltaY(bend.indices[idx], delta.y);
          AtomicAddDeltaZ(bend.indices[idx], delta.z);
        }
      }
    }
  }
}