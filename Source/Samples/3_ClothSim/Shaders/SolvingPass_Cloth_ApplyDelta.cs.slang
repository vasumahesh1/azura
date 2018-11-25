import SolvingPass;

static const float3 SPHERE_CENTER = float3(0, -5, 0);
static const float SPHERE_RADIUS = 3.0;
static const float SPHERE_RADIUS_SQ = SPHERE_RADIUS * SPHERE_RADIUS;
static const float SPHERE_RADIUS_DELTA_SQ = (SPHERE_RADIUS + EPSILON) * (SPHERE_RADIUS + EPSILON);
static const float T_DELTA = 0.01;
static const float3 WORLD_UP = float3(0, 1, 0);
static const float3 PLANE_POS = float3(0, -8, 0);

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

  if (threadIdx >= computeUBOBlock.numVertices) {
    return;
  }

  float3 projectedPos = clothDataBlock.projectedPos[threadIdx];

  if (clothDataBlock.constraintCount[threadIdx] > 0)
  {
    float x = asfloat(clothDataBlock.deltaX[threadIdx]);
    float y = asfloat(clothDataBlock.deltaY[threadIdx]);
    float z = asfloat(clothDataBlock.deltaZ[threadIdx]);

    float3 averaged = (10.0 * float3(x, y, z)) / float(clothDataBlock.constraintCount[threadIdx]);
    projectedPos = clothDataBlock.projectedPos[threadIdx] + averaged;
  }
  
  const float3 vi = normalize(projectedPos - clothDataBlock.vertices[threadIdx]);

  const float3 projVec = projectedPos - SPHERE_CENTER;
  float dist = dot(projVec, projVec);
  if (dist < SPHERE_RADIUS_DELTA_SQ) {
    const float3 xi = clothDataBlock.vertices[threadIdx];

    const float3 xixc = xi - SPHERE_CENTER;
    const float vix = dot(vi, xixc);

    const float tPart1 = -1.0 * vix;
    const float D = (vix * vix) - (dot(xixc, xixc) - SPHERE_RADIUS_SQ);

    if (D >= 0) {
      const float tPart2 = sqrt(D);

      float t = 1.0;

      const float t1 = tPart1 - tPart2;
      const float t2 = tPart1 + tPart2;

      if (t1 < t2 && t1 > 0) {
        t = t1;
      }
      else {
        t = t2;
      }

      if (t >= 0) {
        projectedPos = xi + (t * vi) + (T_DELTA * normalize(projectedPos - SPHERE_CENTER));
      }
    }
  }

  if (dot(projectedPos - PLANE_POS, WORLD_UP) < 0) {
    const float3 xi = clothDataBlock.vertices[threadIdx] - PLANE_POS;
    float t = dot(-WORLD_UP, xi) / dot(WORLD_UP, vi);

    if (t >= 0) {
      projectedPos = xi + (t * vi) + (T_DELTA * WORLD_UP) + PLANE_POS;
    }
  }

  clothDataBlock.projectedPos[threadIdx] = projectedPos;
}