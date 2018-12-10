import SolvingPass;

static const float3 GRAVITY = {0, -9.8, 0};

groupshared int3 groupBoundingMin = int3(10000, 10000, 10000);
groupshared int3 groupBoundingMax = int3(-10000, -10000, -10000);

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
	if (threadIdx >= computeUBOBlock.numVertices) {
		return;
	}

	// Don't Compute Aliases
	if (constraintBlock.vertexAliases[threadIdx] != -1) {
		return;
	}

	const float3 vertexPosition = clothDataBlock.vertices[threadIdx];

	clothDataBlock.deltaX[threadIdx] = 0;
	clothDataBlock.deltaY[threadIdx] = 0;
	clothDataBlock.deltaZ[threadIdx] = 0;
	clothDataBlock.constraintCount[threadIdx] = 0;

	const float invMass = constraintBlock.invMasses[threadIdx];

	const int modelIdx = constraintBlock.vertexModelMatrices[threadIdx];

	float4x4 modelMatrix = float4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	);

	if (modelIdx > -1) {
		modelMatrix = computeUBOBlock.modelMatrix[modelIdx];
	}


	uint time = computeUBOBlock.frameCount;
	float3 WIND = {0, 0, 0};
	// float3 WIND = {0, 0, 10 * (1 + cos(0.033 * time)) * (1 + sin(0.013 * time))};

	float3 projectedPos;
	if (invMass < EPSILON) // ANCHORS
	{
		// Move Anchors by Model Matrix
		projectedPos = mul(modelMatrix, float4(vertexPosition, 1.0)).xyz;
	}
	else {
		const float3 updatedVelocity = 0.98 * (clothDataBlock.velocities[threadIdx] + timeDelta * (GRAVITY + WIND));
		// Projected Pos of that Vertex
		projectedPos = vertexPosition + updatedVelocity * timeDelta;
	}

	clothDataBlock.projectedPos[threadIdx] = projectedPos;

	float3 maxVector = max(vertexPosition, projectedPos);
	float3 minVector = min(vertexPosition, projectedPos);
	maxVector = ceil(maxVector);
	minVector = floor(minVector);

	InterlockedMin(clothDataBlock.properties[0].minX, int(minVector.x));
	InterlockedMin(clothDataBlock.properties[0].minY, int(minVector.y));
	InterlockedMin(clothDataBlock.properties[0].minZ, int(minVector.z));

	InterlockedMax(clothDataBlock.properties[0].maxX, int(maxVector.x));
	InterlockedMax(clothDataBlock.properties[0].maxY, int(maxVector.y));
	InterlockedMax(clothDataBlock.properties[0].maxZ, int(maxVector.z));
}