static const uint BLOCK_SIZE_X = 512;
static const uint SOLVER_ITERATIONS = 128;

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
  float invMass1;
  float invMass2;
};

struct Constraints {
    StructuredBuffer<DistanceConstraint> distanceConstraints;
};

struct VertexBlock {
    RWStructuredBuffer<float4> vertices;
    RWStructuredBuffer<float4> projectedPos;
    RWStructuredBuffer<float4> velocities;
    // RWStructuredBuffer<uint> deltaLock;
    RWStructuredBuffer<uint> constraintCount;
    RWStructuredBuffer<int> deltaX;
    RWStructuredBuffer<int> deltaY;
    RWStructuredBuffer<int> deltaZ;
    // RWStructuredBuffer<float4> delta2;
};

ParameterBlock<ComputeUBO> computeUBOBlock;
ParameterBlock<Constraints> constraintBlock;
ParameterBlock<VertexBlock> clothDataBlock;