
static const uint BLOCK_SIZE_X = 512;
static const uint SOLVER_ITERATIONS = 32;
static const uint MAX_VERTICES_PER_BIN = 15;

static const float EPSILON = 0.00001;
static const float POINT_RADIUS = 0.1;

static const uint CLOTH_GRID_X = 10;
static const uint CLOTH_GRID_Y = 10;
static const uint CLOTH_GRID_Z = 10;

static const uint MAX_SELF_POINT_TRIANGLE_COLLISIONS = 2048;

struct ComputeUBO {
	float stretchStiffness;
	float bendStiffness;
  float longRangeStiffness;
	float timeDelta;

	uint numStretchConstraints;
	uint numBendConstraints;
  uint numLongRangeConstraints;
	uint numVertices;

  uint numTriangles;
  float pad1;
  float pad2;
  float pad3;

  float4x4 modelMatrix;
};

struct DistanceConstraint {
  uint indexA;
  uint indexB;
  float restLength;
};

struct LongRangeConstraint {
  uint indexA;
  uint indexB;
  float restLength;
};

struct BendConstraint {
  uint indices[4];
  float4x4 Q;
};

struct ClothComputeProperties {
  int minX;
  int minY;
  int minZ;

  int maxX;
  int maxY;
  int maxZ;

  uint numSelfCollisionConstraints;
};

struct SelfCollisionConstraint {
  uint pIdx;
  uint p0;
  uint p1;
  uint p2;

  float3 normal;
  float3 debug;
};

struct Constraints {
    StructuredBuffer<DistanceConstraint> distanceConstraints;
    StructuredBuffer<BendConstraint> bendConstraints;
    StructuredBuffer<LongRangeConstraint> longRangeConstraints;
    StructuredBuffer<float> invMasses;
    StructuredBuffer<int> vertexAliases;
    StructuredBuffer<uint> indexBuffer;
};

struct VertexBlock {
    RWStructuredBuffer<ClothComputeProperties> properties;
    RWStructuredBuffer<uint> binCount;
    RWStructuredBuffer<uint> binVertices;
    RWStructuredBuffer<SelfCollisionConstraint> selfCollisionConstraints;
    RWStructuredBuffer<float3> vertices;
    RWStructuredBuffer<float3> projectedPos;
    RWStructuredBuffer<float3> velocities;
    RWStructuredBuffer<uint> constraintCount;
    RWStructuredBuffer<uint> deltaX;
    RWStructuredBuffer<uint> deltaY;
    RWStructuredBuffer<uint> deltaZ;
    RWStructuredBuffer<uint> tanX;
    RWStructuredBuffer<uint> tanY;
    RWStructuredBuffer<uint> tanZ;
};

int LinearizeIndex(int x, int y, int z)
{
  return (x * CLOTH_GRID_Y * CLOTH_GRID_Z) + (y * CLOTH_GRID_Z) + z;
}

ParameterBlock<ComputeUBO> computeUBOBlock;
ParameterBlock<Constraints> constraintBlock;
ParameterBlock<VertexBlock> clothDataBlock;
