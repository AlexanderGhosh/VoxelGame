#include "VoxelMesh.h"
#include <glad/glad.h>
#include "../../Block.h"
#include "../Functions.h"
#include "../../GeomRendering/GeomData.h"
#include "../../Shaders/Shader.h"
#include "../../Mangers/PhysicsManager.h"
#include "../../EntityComponent/Components/Transform.h"
#include "../../GeomRendering/DrawData.h"
#include "VoxelModel.h"
#include "../../GreedyRendering/GreedyData.h"

VoxelMesh::VoxelMesh() : relativePos_(), _buffer(), rigidBody_(), parent(nullptr)
{
#ifdef ALWAYS_USE_GREEDY_MESH
	greedyBuffer = BufferGreedy();
#endif
}

GreedyColliderData from(const GreedyData& d) {
	// all works unless otherwise specified
	GreedyColliderData res;
	res._center = (d._corner0 + d._corner3) * .5f;
#ifndef GREEDY MESH WITH COLLIDERS
	res._center -= HALF_VOXEL_SIZE;
	res._center.y +=VOXEL_SIZE;
#endif
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

VoxelMesh::VoxelMesh(const glm::vec3& relativePos, const std::vector<Block>& cloud, const std::unordered_map<glm::ivec3, std::vector<Block>>& neighbours) : VoxelMesh()
{
	relativePos_ = relativePos * CHUNK_SIZE_F;
	auto index = [](unsigned int x, unsigned int y, unsigned int z) -> unsigned int { return x + y * CHUNK_SIZE_F + z * CHUNK_AREA; };
	int size = CHUNK_SIZE_F * CHUNK_AREA;
	//std::vector<Block> cloud(size, B_AIR); // -1 if empty
	//for (unsigned int i = 0; i < points.size(); i++) {
	//	PointColourIndex& point = points[i];

	//	const int idx = index(point.x, point.y, point.z);
	//	cloud[idx] = point.block;
	//}

	std::vector<GeomData> bufferData;
	bufferData.reserve(size);

	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
			for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
				const Block current = cloud[index(x, y, z)];

				if (current == B_AIR) continue;

				GeomData data;
				data.setPos({ x, y, z });
				data.textureIndex_ = current.materialIndex;
				for (unsigned int i = 0; i < OFFSETS_3D.size(); i++) {
					glm::vec3 currentPos(x, y, z);
					glm::vec3 neighbourPos = currentPos + OFFSETS_3D[i];
					Block neighbour = B_AIR;
					if (glm::all(glm::greaterThanEqual(neighbourPos, glm::vec3(0))) && glm::all(glm::lessThan(neighbourPos, glm::vec3(CHUNK_SIZE)))) {
						neighbour = cloud[index(neighbourPos.x, neighbourPos.y, neighbourPos.z)];
					}
					else {
						// is outside this mesh
						const glm::ivec3 key(OFFSETS_3D[i] + relativePos);
						if (neighbours.contains(key)) {
							const glm::vec3 neihbourPosLocal = neighbourPos - OFFSETS_3D[i] * CHUNK_SIZE_F;
							unsigned int neihboutIdx = index(neihbourPosLocal.x, neihbourPosLocal.y, neihbourPosLocal.z);
							neighbour = neighbours.at(key).at(neihboutIdx);
						}
					}

					if (neighbour.isTransparent && current != neighbour) {
						markSlot(data.cubeType_, i);
					}
				}
				bufferData.push_back(data);
			}
		}
	}
	bufferData.shrink_to_fit();
	_buffer.setUp(bufferData.data(), bufferData.size());
#ifdef ALWAYS_USE_GREEDY_MESH
	auto greedyMeshData = std::move(greedyMesh(cloud));
	greedyBuffer.setUp(greedyMeshData.data(), greedyMeshData.size());
#endif

//	if (hasCollider) {
//#ifndef ALWAYS_USE_GREEDY_MESH
//		auto greedyMeshData = std::move(greedyMesh(cloud));
//#endif // ALWAYS_USE_GREEDY_MESH
//
//		PhysicsManager& manager = PhysicsManager::getInstance();
//		Components::Transform rbPosition;
//		rbPosition.position = relativePos_;
//		rigidBody_ = manager.createRigidBody(&rbPosition);
//		rigidBody_->enableGravity(false);
//		rigidBody_->setType(reactphysics3d::BodyType::STATIC);
//		
//		for (const auto& d : greedyMeshData) {
//			auto data = from(d);
//			auto shape = manager.createBoxShape(glm::vec3(data._xSpan, data._ySpan, data._zSpan));
//			reactphysics3d::Transform relativePos;
//
//			const glm::vec3 pos = relativePos_ + data._center;
//			memcpy(&relativePos, &pos, sizeof(glm::vec3)); // can copy straight in because positoin is at the top of transform
//
//			rigidBody_->addCollider(shape, relativePos);
//		}
//	}
}

DrawData VoxelMesh::getDrawData() const
{
	DrawData res;
	res._buffer = const_cast<BufferGeom*>(&_buffer);
	res._drawOrigin = relativePos_ + parent->worldPos_;
	res._type = DrawData::MODEL;
	return res;
}

std::vector<GreedyData> VoxelMesh::greedyMesh(std::vector<Block>& cloud)
{
	struct MergeData {
		Block block;
		int mkPoint;
		int x;
		int idx;
	};
	std::vector<MergeData> prevMergeDataPY;
	std::vector<MergeData> currentMergeDataPY;

	std::vector<MergeData> prevMergeDataPX;
	std::vector<MergeData> currentMergeDataPX;

	std::vector<MergeData> prevMergeDataNX;
	std::vector<MergeData> currentMergeDataNX;

	auto index = [](unsigned int x, unsigned int y, unsigned int z) -> unsigned int { return x + y * CHUNK_SIZE + z * CHUNK_AREA; };

	auto show = [&](Block a, Block b) {
		return b.isTransparent && a != b;
	};

	// unscaled local space
	auto isVisiblePY = [&cloud, index, show](Block a, int x, int y, int z) -> bool {
		Block b;
		if (++y == CHUNK_SIZE) {
			return true;
		}
		b = cloud[index(x, y, z)];
		return show(a, b);
	};
	auto isVisibleNY = [&cloud, index, show](Block a, int x, int y, int z) -> bool {
		Block b;
		if (y == 0) {
			return true;
		}
		b = cloud[index(x, --y, z)];
		return show(a, b);
		};

	auto isVisiblePZ = [&cloud, index, show](Block a, int x, int y, int z) -> bool {
		Block b;
		if (++z == CHUNK_SIZE) {
			return true;
		}
		b = cloud[index(x, y, z)];
		return show(a, b);
	};
	auto isVisibleNZ = [&cloud, index, show](Block a, int x, int y, int z) -> bool {
		Block b;
		if (z == 0) {
			return true;
		}
		b = cloud[index(x, y, --z)];
		return show(a, b);
	};

	auto isVisiblePX = [&cloud, index, show](Block a, int x, int y, int z) -> bool {
		Block b;
		if (++x == CHUNK_SIZE) {
			return true;
		}
		b = cloud[index(x, y, z)];
		return show(a, b);
	};
	auto isVisibleNX = [&cloud, index, show](Block a, int x, int y, int z) -> bool {
		Block b;
		if (x == 0) {
			return true;
		}
		b = cloud[index(--x, y, z)];
		return show(a, b);
	};

	std::vector<GreedyData> greedyBufferData;
	auto addPY = [&greedyBufferData](Block b, int mkPoint, int x, int y, int z) -> bool {
		//return;
		if (b != B_AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y, z);
			glm::vec3 faceMax(x, y, z + 1);

			GreedyData data;
			data._normal = glm::vec3(0, 1, 0);
			data._materialIdx = b.materialIndex;
			data._corner0 = faceMin;
			data._corner1 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner2 = { faceMin.x, faceMin.y, faceMax.z };
			data._corner3 = faceMax;

			greedyBufferData.push_back(data);
			return true;
		}
		return false;
	};
	auto addNY = [&greedyBufferData](Block b, int mkPoint, int x, int y, int z) {
		//return;
		if (b != B_AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y, z);
			glm::vec3 faceMax(x, y, z + 1);
			faceMin.y -= 1.f;
			faceMax.y -= 1.f;


			GreedyData data;
			data._normal = glm::vec3(0, -1, 0);
			data._materialIdx = b.materialIndex;
			data._corner0 = faceMin;
			data._corner1 = { faceMin.x, faceMin.y, faceMax.z };
			data._corner2 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner3 = faceMax;

			greedyBufferData.push_back(data);
		}
		};
	auto addPZ = [&greedyBufferData](Block b, int mkPoint, int x, int y, int z) {
		// add face
		if (b != B_AIR && mkPoint != x) {
			glm::vec3 faceMin(mkPoint, y - 1, z);
			glm::vec3 faceMax(x, y, z);
			faceMin.z += 1.f;
			faceMax.z += 1.f;

			GreedyData data;
			data._normal = glm::vec3(0, 0, 1);
			data._materialIdx = b.materialIndex;
			data._corner0 = faceMin;
			data._corner1 = { faceMin.x, faceMax.y, faceMax.z };
			data._corner2 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner3 = faceMax;

			greedyBufferData.push_back(data);
		}
		};
	auto addNZ = [&greedyBufferData](Block b, int mkPoint, int x, int y, int z) {
		if (b != B_AIR && mkPoint != x) {
			// add face
			glm::vec3 faceMin(mkPoint, y - 1, z);
			glm::vec3 faceMax(x, y, z);

			GreedyData data;
			data._normal = glm::vec3(0, 0, -1);
			data._materialIdx = b.materialIndex;
			data._corner0 = faceMin;
			data._corner1 = { faceMax.x, faceMin.y, faceMin.z };
			data._corner2 = { faceMin.x, faceMax.y, faceMax.z };
			data._corner3 = faceMax;
			greedyBufferData.push_back(data);
		}
		};
	auto addPX = [&greedyBufferData](Block b, int x, int y, int z)  {
		//return;
		glm::vec3 faceMin(x, y - 1, z);
		glm::vec3 faceMax(x, y, z + 1);
		faceMin.x += 1.f;
		faceMax.x += 1.f;

		GreedyData data;
		data._normal = glm::vec3(1, 0, 0);
		data._materialIdx = b.materialIndex;
		data._corner0 = faceMin;
		data._corner1 = { faceMin.x, faceMin.y, faceMax.z };
		data._corner2 = { faceMin.x, faceMax.y, faceMin.z };
		data._corner3 = faceMax;
		greedyBufferData.push_back(data);
		};
	auto addNX = [&greedyBufferData](Block b, int x, int y, int z) {
		//return;

		glm::vec3 faceMin(x, y - 1, z);
		glm::vec3 faceMax(x, y, z + 1);

		GreedyData data;
		data._normal = glm::vec3(1, 0, 0);
		data._materialIdx = b.materialIndex;
		data._corner0 = faceMin;
		data._corner1 = { faceMin.x, faceMax.y, faceMin.z };
		data._corner2 = { faceMin.x, faceMin.y, faceMax.z };
		data._corner3 = faceMax;
		greedyBufferData.push_back(data);
		};

	//////////////////////////////////////////

	for (int y = CHUNK_SIZE - 1; y >= 0; y--) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				Block prevBlock = cloud[index(x, y, z)];
				if (prevBlock == B_AIR) continue;
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
							MergeData data{};
							data.block = prevBlock;
							data.mkPoint = mkPointPY;
							data.x = x;

							bool hit = false;
							for (MergeData& d : prevMergeDataPY) {
								if (d.block == prevBlock && d.mkPoint == mkPointPY && d.x == x) {
									// this hits when the face to be added is the same as in the prev row
									hit = true;
									GreedyData& g = greedyBufferData[d.idx];
									++g._corner2.z;
									++g._corner3.z;
									data.idx = d.idx;
								}
							}
							if (!hit) {
								data.idx = greedyBufferData.size();
								hit = addPY(currentBlock, mkPointPY, x, y, z);
							}
							// hit has being resused to now mean has sdded
							if (hit)
								currentMergeDataPY.push_back(data);
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
							MergeData data{};
							data.block = prevBlock;
							data.mkPoint = mkPointPY;
							data.x = x;

							bool hit = false;
							for (MergeData& d : prevMergeDataPY) {
								if (d.block == prevBlock && d.mkPoint == mkPointPY && d.x == x) {
									// this hits when the face to be added is the same as in the prev row
									hit = true;
									GreedyData& g = greedyBufferData[d.idx];
									++g._corner2.z;
									++g._corner3.z;
									data.idx = d.idx;
								}
							}
							if (!hit) {
								data.idx = greedyBufferData.size();
								hit = addPY(prevBlock, mkPointPY, x, y, z);
							}
							// hit has being resused to now mean has sdded
							if(hit)
								currentMergeDataPY.push_back(data);
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
				// addPY(prevBlock, mkPointPY, x, y, z);
				MergeData data{};
				data.block = prevBlock;
				data.mkPoint = mkPointPY;
				data.x = x;
				bool hit = false;
				for (MergeData& d : prevMergeDataPY) {
					if (d.block == prevBlock && d.mkPoint == mkPointPY && d.x == x) {
						// this hits when the face to be added is the same as in the prev row
						hit = true;
						GreedyData& g = greedyBufferData[d.idx];
						++g._corner2.z;
						++g._corner3.z;
						data.idx = d.idx;
						break;
					}
				}
				if (!hit) {
					data.idx = greedyBufferData.size();
					hit = addPY(prevBlock, mkPointPY, x, y, z);
				}
				if(hit)
					currentMergeDataPY.push_back(data);
				addNY(prevBlock, mkPointNY, x, y, z);
				addPZ(prevBlock, mkPointPZ, x, y, z);
				addNZ(prevBlock, mkPointNZ, x, y, z);
			}
			prevMergeDataPY = std::move(currentMergeDataPY);

			// X faces

			for (int x = 0; x < CHUNK_SIZE; x++) {
				// this is what causes the extra colliders in the ridgidbody 
				Block currentBlock = cloud[index(x, y, z)];
				if (isVisiblePX(currentBlock, x, y, z) && currentBlock != B_AIR) {
					bool added = false;
					for (const auto& d : prevMergeDataPX) {
						if (d.block == currentBlock && d.x == x) {
							GreedyData& data = greedyBufferData[d.idx];
							currentMergeDataPX.push_back(d);
							++data._corner1.z;
							++data._corner3.z;
							added = true;
							break;
						}
					}
					if (!added) {
						MergeData d;
						d.block = currentBlock;
						d.idx = greedyBufferData.size();
						d.x = x;
						currentMergeDataPX.push_back(d);

						addPX(currentBlock, x, y, z);
					}
				}

				if (isVisibleNX(currentBlock, x, y, z) && currentBlock != B_AIR) {
					bool added = false;
					for (const auto& d : prevMergeDataNX) {
						if (d.block == currentBlock && d.x == x) {
							GreedyData& data = greedyBufferData[d.idx];
							currentMergeDataNX.push_back(d);
							++data._corner2.z;
							++data._corner3.z;
							added = true;
							break;
						}
					}
					if (!added) {
						MergeData d;
						d.block = currentBlock;
						d.idx = greedyBufferData.size();
						d.x = x;
						currentMergeDataNX.push_back(d);

						addNX(currentBlock, x, y, z);
					}
				}
			}

			prevMergeDataPX = std::move(currentMergeDataPX);
			prevMergeDataNX = std::move(currentMergeDataNX);
		}
	}
	return greedyBufferData;
}
