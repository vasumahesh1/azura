static const uint BLOCK_SIZE_X = 512;

struct VertexBlock {
    RWStructuredBuffer<float4> vertices;
};

struct ComputeUBO {
	float stretchStiffness;
	float bendStiffness;
	float timeDelta;
	uint numStretchConstraints;

	uint numBendConstraints;
	uint numVertices;
	float pad[2];
};

ParameterBlock<ComputeUBO> computeUBOBlock;
ParameterBlock<VertexBlock> clothDataBlock;