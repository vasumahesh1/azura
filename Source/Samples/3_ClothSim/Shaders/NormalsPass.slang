static const uint BLOCK_SIZE_X = 512;

struct NormalsUBO {
  uint numVertices;
	uint numTriangles;
	float pad[2];
};

struct ROBlock {
  StructuredBuffer<uint> indexBuffer;
  StructuredBuffer<float3> vertices;
};

struct RWBlock {
  RWStructuredBuffer<uint> normalX;
  RWStructuredBuffer<uint> normalY;
  RWStructuredBuffer<uint> normalZ;
};

ParameterBlock<NormalsUBO> uboBlock;
ParameterBlock<ROBlock> roBlock;
ParameterBlock<RWBlock> rwBlock;
