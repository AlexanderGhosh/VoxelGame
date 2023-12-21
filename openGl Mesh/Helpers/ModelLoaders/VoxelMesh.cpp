#include "VoxelMesh.h"
#include <glad/glad.h>
#include "../Functions.h"
#include "../../GeomRendering/GeomData.h"
#include "../../Shaders/Shader.h"
#include "../../Mangers/PhysicsManager.h"
#include "../../EntityComponent/Components/Transform.h"
#include "../../GeomRendering/DrawData.h"
#include "VoxelModel_Base.h"
#include "../../GreedyRendering/GreedyData.h"

VoxelMesh::VoxelMesh() : relativePos_(), buffer_(), rigidBody_(), parent(nullptr)
{
#ifdef ALWAYS_USE_GREEDY_MESH
	greedyBuffer = BufferGreedy();
#endif
}

GreedyColliderData from(const GreedyData& d) {
	// all works unless otherwise specified
	GreedyColliderData res;
	res._center = (d._corner0 + d._corner3) * .5f;
	if (d._normal.y != 0) {
		res._xSpan = (d._corner3.x - d._corner0.x) * .5f;
		res._zSpan = HALF_VOXEL_SIZE;
	}
	if (d._normal.x != 0) {
		// creates extra faces
		res._zSpan = HALF_VOXEL_SIZE;
		res._ySpan = HALF_VOXEL_SIZE;
	}
	if (d._normal.z != 0) {
		res._xSpan = (d._corner3.x - d._corner0.x) * .5f;
		res._ySpan = HALF_VOXEL_SIZE;
	}
	return res;
}

VoxelMesh::VoxelMesh(const glm::vec3& relativePos, std::vector<PointColourIndex>& points, const std::vector<glm::vec3>& colours, bool hasCollider) : VoxelMesh()
{
	relativePos_ = relativePos;
	auto index = [](unsigned int x, unsigned int y, unsigned int z) -> unsigned int { return x + y * CHUNK_SIZE_F + z * CHUNK_AREA; };
	int size = CHUNK_SIZE_F * CHUNK_AREA;
	std::vector<Block> cloud(size, Block::AIR); // -1 if empty
	for (unsigned int i = 0; i < points.size(); i++) {
		PointColourIndex& point = points[i];

		const int idx = index(point.x, point.y, point.z);
		cloud[idx] = Block::STONE;
	}

	std::vector<GeomData> bufferData;
	bufferData.reserve(size);

	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
			for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
				const Block current = cloud[index(x, y, z)];
				const BlockDetails& currentDetails = getDetails(current);

				if (current == Block::AIR) continue;

				GeomData data;
				data.setPos({ x, y, z });
				data.textureIndex_ = (unsigned char)current;
				for (unsigned int i = 0; i < OFFSETS_3D.size(); i++) {
					glm::vec3 currentPos(x, y, z);
					glm::vec3 neighbourPos = currentPos + OFFSETS_3D[i];
					Block neighbour = Block::AIR;
					if (glm::all(glm::greaterThanEqual(neighbourPos, glm::vec3(0))) && glm::all(glm::lessThan(neighbourPos, glm::vec3(CHUNK_SIZE)))) {
						neighbour = cloud[index(neighbourPos.x, neighbourPos.y, neighbourPos.z)];
					}

					const BlockDetails& neighboursDetails = getDetails(neighbour);

					if (neighboursDetails.isTransparant && current != neighbour) {
						markSlot(data.cubeType_, i);
					}
				}
				// data.cubeType_ = 63;
				bufferData.push_back(data);
			}
		}
	}
	bufferData.shrink_to_fit();
	buffer_.setUp(bufferData.data(), bufferData.size());
#ifdef ALWAYS_USE_GREEDY_MESH
	auto greedyMeshData = std::move(greedyMesh(cloud));
	greedyBuffer.setUp(greedyMeshData.data(), greedyMeshData.size());
#endif

	if (hasCollider) {
#ifndef ALWAYS_USE_GREEDY_MESH
		auto greedyMeshData = std::move(greedyMesh(cloud));
#endif // ALWAYS_USE_GREEDY_MESH

		PhysicsManager& manager = PhysicsManager::getInstance();
		Components::Transform rbPosition;
		rbPosition.position = relativePos_;
		rigidBody_ = manager.createRigidBody(&rbPosition);
		rigidBody_->enableGravity(false);
		rigidBody_->setType(reactphysics3d::BodyType::STATIC);
		
		for (const auto& d : greedyMeshData) {
			auto data = from(d);
			auto shape = manager.createBoxShape(glm::vec3(data._xSpan, data._ySpan, data._zSpan));
			reactphysics3d::Transform relativePos;

			const glm::vec3 pos = relativePos_ + data._center;
			memcpy(&relativePos, &pos, sizeof(glm::vec3)); // can copy straight in because positoin is at the top of transform

			rigidBody_->addCollider(shape, relativePos);
		}
	}
}

DrawData VoxelMesh::getDrawData() const
{
	DrawData res;
	res.buffer_ = const_cast<BufferGeom*>(&buffer_);
	res.drawOrigin_ = relativePos_ + parent->worldPos_;
	res.type_ = DrawData::MODEL;
	return res;
}

std::vector<GreedyData> VoxelMesh::greedyMesh(std::vector<Block>& cloud)
{
	auto index = [](unsigned int x, unsigned int y, unsigned int z) -> unsigned int { return x + y * CHUNK_SIZE + z * CHUNK_AREA; };

	auto show = [&](Block a, Block b) {
		auto& d = getDetails(b);
		return d.isTransparant && a != b;
		};

	// unscaled local space
	auto isVisiblePY = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (++y == CHUNK_SIZE) {
			return true;
		}
		b = cloud[index(x, y, z)];
		return show(a, b);
	};
	auto isVisibleNY = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (y == 0) {
			return true;
		}
		b = cloud[index(x, --y, z)];
		return show(a, b);
		};

	auto isVisiblePZ = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (++z == CHUNK_SIZE) {
			return true;
		}
		b = cloud[index(x, y, z)];
		return show(a, b);
	};
	auto isVisibleNZ = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (z == 0) {
			return true;
		}
		b = cloud[index(x, y, --z)];
		return show(a, b);
	};

	auto isVisiblePX = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (++x == CHUNK_SIZE) {
			return true;
		}
		b = cloud[index(x, y, z)];
		return show(a, b);
	};
	auto isVisibleNX = [&](Block a, int x, int y, int z) -> bool {
		Block b;
		if (x == 0) {
			return true;
		}
		b = cloud[index(--x, y, z)];
		return show(a, b);
	};

	std::vector<GreedyData> greedyBufferData;
	auto addPY = [&](Block b, int mkPoint, int x, int y, int z) {
		//return;
		if (b != Block::AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y, z);
			glm::vec3 faceMax(x, y, z + 1);

			GreedyData data;
			data._normal = glm::vec3(0, 1, 0);
			data._materialIdx = (unsigned int)b;
			data._corner0 = faceMin;
			data._corner1 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner2 = { faceMin.x, faceMin.y, faceMax.z };
			data._corner3 = faceMax;

			greedyBufferData.push_back(data);
		}
		};
	auto addNY = [&](Block b, int mkPoint, int x, int y, int z) {
		//return;
		if (b != Block::AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y, z);
			glm::vec3 faceMax(x, y, z + 1);
			faceMin.y -= 1.f;
			faceMax.y -= 1.f;


			GreedyData data;
			data._normal = glm::vec3(0, -1, 0);
			data._materialIdx = (unsigned int)b;
			data._corner0 = faceMin;
			data._corner1 = { faceMin.x, faceMin.y, faceMax.z };
			data._corner2 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner3 = faceMax;

			greedyBufferData.push_back(data);
		}
		};
	auto addPZ = [&](Block b, int mkPoint, int x, int y, int z) {
		// add face
		if (b != Block::AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y - 1, z);
			glm::vec3 faceMax(x, y, z);
			faceMin.z += 1.f;
			faceMax.z += 1.f;

			GreedyData data;
			data._normal = glm::vec3(0, 0, 1);
			data._materialIdx = (unsigned int)b;
			data._corner0 = faceMin;
			data._corner1 = { faceMin.x, faceMax.y, faceMax.z };
			data._corner2 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner3 = faceMax;

			greedyBufferData.push_back(data);
		}
		};
	auto addNZ = [&](Block b, int mkPoint, int x, int y, int z) {
		if (b != Block::AIR && mkPoint != x) {
			// add face
			glm::vec3 faceMin(mkPoint, y - 1, z);
			glm::vec3 faceMax(x, y, z);

			GreedyData data;
			data._normal = glm::vec3(0, 0, -1);
			data._materialIdx = (unsigned int)b;
			data._corner0 = faceMin;
			data._corner1 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner2 = { faceMin.x, faceMax.y, faceMax.z };
			data._corner3 = faceMax;
			greedyBufferData.push_back(data);
		}
		};

	//////////////////////////////////////////

	for (int y = CHUNK_SIZE - 1; y >= 0; y--) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				Block prevBlock = cloud[index(x, y, z)];
				if (prevBlock == Block::AIR) continue;
				int mkPointPY = x; // the start point of a run
				int mkPointNY = x;
				int mkPointPZ = x;
				int mkPointNZ = x;
				// visited_px[idx] = true;
				for (; x < CHUNK_SIZE; x++) {
					// visited_px[idx] = true;
					const Block currentBlock = cloud[index(x, y, z)];

					bool pyVisible = isVisiblePY(currentBlock, x, y, z);
					bool nyVisible = isVisibleNY(currentBlock, x, y, z);
					bool pzVisible = isVisiblePZ(currentBlock, x, y, z);
					bool nzVisible = isVisibleNZ(currentBlock, x, y, z);

					if (!pyVisible && !nyVisible && !pzVisible && !nzVisible) break;

					if (prevBlock == currentBlock) {
						// PY
						if (pyVisible) {
							// doesnt change mark spot
						}
						else {
							addPY(currentBlock, mkPointPY, x, y, z);
							mkPointPY = x + 1;
						}
						// NY
						if (nyVisible) {
							// doesnt change mark spot
						}
						else {
							addNY(currentBlock, mkPointNY, x, y, z);
							mkPointNY = x + 1;
						}

						// PZ
						if (pzVisible) {
							// doesnt change mark spot
						}
						else {
							addPZ(currentBlock, mkPointPZ, x, y, z);
							mkPointPZ = x + 1;
						}
						// NZ
						if (nzVisible) {
							// doesnt change mark spot
						}
						else {
							addNZ(currentBlock, mkPointNZ, x, y, z);
							mkPointNZ = x + 1;
						}
					}
					if (prevBlock != currentBlock) {
						// PY
						if (pyVisible) {
							addPY(prevBlock, mkPointPY, x, y, z);
							mkPointPY = x;
						}
						else {
							addPY(prevBlock, mkPointPY, x, y, z);
							mkPointPY = x + 1;
						}
						// NY
						if (nyVisible) {
							addNY(prevBlock, mkPointNY, x, y, z);
							mkPointNY = x;
						}
						else {
							addNY(prevBlock, mkPointNY, x, y, z);
							mkPointNY = x + 1;
						}

						// PZ
						if (pzVisible) {
							addPZ(prevBlock, mkPointPZ, x, y, z);
							mkPointPZ = x;
						}
						else {
							addPZ(prevBlock, mkPointPZ, x, y, z);
							mkPointPZ = x + 1;
						}

						// NZ
						if (nzVisible) {
							addNZ(prevBlock, mkPointNZ, x, y, z);
							mkPointNZ = x;
						}
						else {
							addNZ(prevBlock, mkPointNZ, x, y, z);
							mkPointNZ = x + 1;
						}
					}
					prevBlock = currentBlock;
				}

				// add traing faces
				addPY(prevBlock, mkPointPY, x, y, z);
				addNY(prevBlock, mkPointNY, x, y, z);
				addPZ(prevBlock, mkPointPZ, x, y, z);
				addNZ(prevBlock, mkPointNZ, x, y, z);
			}

			// X faces
			for (int x = 0; x < CHUNK_SIZE; x++) {
				// this is what causes the extra colliders in the ridgidbody 
				Block currentBlock = cloud[index(x, y, z)];
				if (isVisiblePX(currentBlock, x, y, z) && currentBlock != Block::AIR) {

					glm::vec3 faceMin(x, y - 1, z);
					glm::vec3 faceMax(x, y, z + 1);
					faceMin.x += 1.f;
					faceMax.x += 1.f;

					GreedyData data;
					data._normal = glm::vec3(1, 0, 0);
					data._materialIdx = (unsigned int)currentBlock;
					data._corner0 = faceMin;
					data._corner1 = { faceMin.x, faceMin.y, faceMax.z };
					data._corner2 = { faceMin.x, faceMax.y, faceMin.z };
					data._corner3 = faceMax;
					greedyBufferData.push_back(data);
				}

				if (isVisibleNX(currentBlock, x, y, z) && currentBlock != Block::AIR) {
					glm::vec3 faceMin(x, y - 1, z);
					glm::vec3 faceMax(x, y, z + 1);

					GreedyData data;
					data._normal = glm::vec3(-1, 0, 0);
					data._materialIdx = (unsigned int)currentBlock;
					data._corner0 = faceMin;
					data._corner1 = { faceMin.x, faceMax.y, faceMin.z };
					data._corner2 = { faceMin.x, faceMin.y, faceMax.z };
					data._corner3 = faceMax;
					greedyBufferData.push_back(data);
				}
			}
		}
	}
	return greedyBufferData;
}
