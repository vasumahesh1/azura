
static const uint BLOCK_SIZE_X = 512;
static const uint SOLVER_ITERATIONS = 64;

static const float EPSILON = 0.00001;

struct ComputeUBO {
	float stretchStiffness;
	float bendStiffness;
	float timeDelta;
	uint numStretchConstraints;

	uint numBendConstraints;
	uint numVertices;
	uint numBlocks;
	float pad;
};

struct DistanceConstraint {
  uint indexA;
  uint indexB;
  float restLength;
};

struct BendConstraint {
  uint indices[4];
  float4x4 Q;
};

struct Constraints {
    StructuredBuffer<DistanceConstraint> distanceConstraints;
    StructuredBuffer<BendConstraint> bendConstraints;
    StructuredBuffer<float> invMasses;
};

struct VertexBlock {
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

ParameterBlock<ComputeUBO> computeUBOBlock;
ParameterBlock<Constraints> constraintBlock;
ParameterBlock<VertexBlock> clothDataBlock;
