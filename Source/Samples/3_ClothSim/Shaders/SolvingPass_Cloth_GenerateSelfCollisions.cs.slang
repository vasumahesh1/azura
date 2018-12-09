import SolvingPass;

[numthreads(BLOCK_SIZE_X, 1, 1)]
void main(
    uint3 groupID : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex,
    uint3 dispatchThreadId : SV_DispatchThreadID
)
{
	const uint threadIdx = dispatchThreadId.x;

	// Update all triangles
	if (threadIdx >= computeUBOBlock.numTriangles) {
		return;
	}

	uint id0 = constraintBlock.indexBuffer[3 * threadIdx];
	uint id1 = constraintBlock.indexBuffer[3 * threadIdx + 1];
	uint id2 = constraintBlock.indexBuffer[3 * threadIdx + 2];

	// Don't Compute Aliases
	if (constraintBlock.vertexAliases[id0] != -1) {
		id0 = constraintBlock.vertexAliases[id0];
	}

	if (constraintBlock.vertexAliases[id1] != -1) {
		id1 = constraintBlock.vertexAliases[id1];
	}

	if (constraintBlock.vertexAliases[id2] != -1) {
		id2 = constraintBlock.vertexAliases[id2];
	}

	float3 v0 = clothDataBlock.vertices[id0];
	float3 v1 = clothDataBlock.vertices[id1];
	float3 v2 = clothDataBlock.vertices[id2];

	float3 p0 = clothDataBlock.projectedPos[id0];
	float3 p1 = clothDataBlock.projectedPos[id1];
	float3 p2 = clothDataBlock.projectedPos[id2];

	float3 minPoint = min(min(min(min(min(v0, v1), v2), p0), p1), p2);
	float3 maxPoint = max(max(max(max(max(v0, v1), v2), p0), p1), p2);

	float3 boundMin = float3(clothDataBlock.properties[0].minX, clothDataBlock.properties[0].minY, clothDataBlock.properties[0].minZ);
	float3 boundMax = float3(clothDataBlock.properties[0].maxX, clothDataBlock.properties[0].maxY, clothDataBlock.properties[0].maxZ);

	int minGridX = floor(((minPoint.x - boundMin.x) / (boundMax.x - boundMin.x + 0.0001)) * CLOTH_GRID_X);
	int minGridY = floor(((minPoint.y - boundMin.y) / (boundMax.y - boundMin.y + 0.0001)) * CLOTH_GRID_Y);
	int minGridZ = floor(((minPoint.z - boundMin.z) / (boundMax.z - boundMin.z + 0.0001)) * CLOTH_GRID_Z);

	int maxGridX = floor(((maxPoint.x - boundMin.x) / (boundMax.x - boundMin.x + 0.0001)) * CLOTH_GRID_X);
	int maxGridY = floor(((maxPoint.y - boundMin.y) / (boundMax.y - boundMin.y + 0.0001)) * CLOTH_GRID_Y);
	int maxGridZ = floor(((maxPoint.z - boundMin.z) / (boundMax.z - boundMin.z + 0.0001)) * CLOTH_GRID_Z);

	for(uint x = minGridX; x <= maxGridX; ++x) {
		for(uint y = minGridY; y <= maxGridY; ++y) {
			for(uint z = minGridZ; z <= maxGridZ; ++z) {
				int targetIdx = LinearizeIndex(x, y, z);
				uint binCount = clothDataBlock.binCount[targetIdx];

				for(uint p = 0; p < binCount; ++p) {
					uint vertIdx = clothDataBlock.binVertices[MAX_VERTICES_PER_BIN * targetIdx + p];
					if (id0 == vertIdx || id1 == vertIdx || id2 == vertIdx)
					{
						continue;
					}

					const float3 vP = clothDataBlock.vertices[vertIdx];

					float3 normal = cross(v2 - v0, v1 - v0);
					const float3 normCap = normalize(normal);
					const float normMag = length(normal);

					float D = dot(normCap, vP - v0);
					if (D < 0) {
						normal = -normal;
					}

					float3 pC = vP - (normCap * D);

					const float baryAlpha = length(cross(v1 - pC, v2 - pC)) / normMag;
					const float baryBeta = length(cross(v2 - pC, v0 - pC)) / normMag;
					const float baryGamma = length(cross(v0 - pC, v1 - pC)) / normMag;
					const float barySum = baryAlpha + baryBeta + baryGamma;

					if ((barySum < 0.0 - EPSILON) || (barySum > 1.0 + EPSILON)) {
						continue;
					}

					const float3 projP = clothDataBlock.projectedPos[vertIdx];

					const float3 d = projP - vP;
					const float3 d0 = p0 - v0;
					const float3 d1 = p1 - v1;
					const float3 d2 = p2 - v2;
					const float3 dC = (d0 * baryAlpha) + (d1 * baryBeta) + (d2 * baryGamma);
					const float3 lDir = normalize(pC - vP);

					const float dcProj = dot(dC, lDir);
					const float dProj = dot(d, lDir);

					if (dProj + POINT_RADIUS < dcProj) {
						continue;
					}

					uint offset;
					InterlockedAdd(clothDataBlock.properties[0].numSelfCollisionConstraints, 1, offset);

					clothDataBlock.selfCollisionConstraints[offset].pIdx = vertIdx;
					clothDataBlock.selfCollisionConstraints[offset].p0 = id0;
					clothDataBlock.selfCollisionConstraints[offset].p1 = id1;
					clothDataBlock.selfCollisionConstraints[offset].p2 = id2;
					clothDataBlock.selfCollisionConstraints[offset].normal = normal;

					const float3 nCap = normalize(normal);
				    const float3 nMag = length(normal);

				    const float a = nCap.x;
				    const float b = nCap.y;
				    const float c = nCap.z;

				    const float a2 = nCap.x * nCap.x;
				    const float b2 = nCap.y * nCap.y;
				    const float c2 = nCap.z * nCap.z;

				    float3 nVec = float3(
				      a - (a2 * a) - (a * b2) - (a * c2),
				      (-a2 * b) + b - (b2 * b) - (b * c2),
				      (-a2 * c) - (b2 * c) + c - (c2 * c)
				    );

				    nVec = nVec / nMag;

				    clothDataBlock.selfCollisionConstraints[offset].debug = nVec;
				}
			}
		}
	}
}