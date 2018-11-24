import NormalsPass;

void AtomicAddNormalX(uint addr, float value)
{
  uint i_val = asuint(value);
  uint tmp0 = 0;
  uint tmp1;

  [allow_uav_condition] while(true)
  {
    InterlockedCompareExchange(rwBlock.normalX[addr], tmp0, i_val, tmp1);

    if (tmp1 == tmp0)
      break;

    tmp0 = tmp1;
    i_val = asuint(value + asfloat(tmp1));
  }
}

void AtomicAddNormalY(uint addr, float value)
{
  uint i_val = asuint(value);
  uint tmp0 = 0;
  uint tmp1;

  [allow_uav_condition] while(true)
  {
    InterlockedCompareExchange(rwBlock.normalY[addr], tmp0, i_val, tmp1);

    if (tmp1 == tmp0)
      break;

    tmp0 = tmp1;
    i_val = asuint(value + asfloat(tmp1));
  }
}

void AtomicAddNormalZ(uint addr, float value)
{
  uint i_val = asuint(value);
  uint tmp0 = 0;
  uint tmp1;

  [allow_uav_condition] while(true)
  {
    InterlockedCompareExchange(rwBlock.normalZ[addr], tmp0, i_val, tmp1);

    if (tmp1 == tmp0)
      break;

    tmp0 = tmp1;
    i_val = asuint(value + asfloat(tmp1));
  }
}

[numthreads(BLOCK_SIZE_X, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
  const uint threadIdx = dispatchThreadId.x;

  if (threadIdx < uboBlock.numVertices) {
    rwBlock.normalX[threadIdx] = 0;
    rwBlock.normalY[threadIdx] = 0;
    rwBlock.normalZ[threadIdx] = 0;
  }

  AllMemoryBarrierWithGroupSync();

  if (threadIdx < uboBlock.numTriangles) {
    const uint id0 = roBlock.indexBuffer[3 * threadIdx];
    const uint id1 = roBlock.indexBuffer[3 * threadIdx + 1];
    const uint id2 = roBlock.indexBuffer[3 * threadIdx + 2];

    const float3 v0 = roBlock.vertices[id0];
    const float3 v1 = roBlock.vertices[id1];
    const float3 v2 = roBlock.vertices[id2];

    float3 normal = cross(v2 - v0, v1 - v0);

    AtomicAddNormalX(id0, normal.x);
    AtomicAddNormalX(id1, normal.x);
    AtomicAddNormalX(id2, normal.x);

    AtomicAddNormalY(id0, normal.y);
    AtomicAddNormalY(id1, normal.y);
    AtomicAddNormalY(id2, normal.y);

    AtomicAddNormalZ(id0, normal.z);
    AtomicAddNormalZ(id1, normal.z);
    AtomicAddNormalZ(id2, normal.z);
  }

  AllMemoryBarrierWithGroupSync();

  if (threadIdx < uboBlock.numVertices) {
    float x = asfloat(rwBlock.normalX[threadIdx]);
    float y = asfloat(rwBlock.normalY[threadIdx]);
    float z = asfloat(rwBlock.normalZ[threadIdx]);
    rwBlock.normals[threadIdx] = normalize(float3(x, y, z));
  }
}