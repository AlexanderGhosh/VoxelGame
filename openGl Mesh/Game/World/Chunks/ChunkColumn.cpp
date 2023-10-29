#include "ChunkColumn.h"
#include <algorithm>
#include "../../../Buffer.h"
#include "../../../GeomRendering/GeomData.h"

#pragma region Public
#pragma region Constructors
ChunkColumn::ChunkColumn() : position(0), highest_natural_point(-1), mesh(), blockStore(), isFlat(1), fromFile(0), needsSave(0), stage(200)
{
	// blocks.fill(Blocks::AIR);
}

ChunkColumn::ChunkColumn(glm::vec2 pos) : position(pos), highest_natural_point(-1), mesh(), isFlat(0), stage(0), fromFile(0), needsSave(0)
{
	blockStore = new BlockStore(pos);
}

ChunkColumn::ChunkColumn(std::string fileName) : fromFile(1), needsSave(0)
{
	fileName = "Chunks/" + fileName + ".dat";

	Savable_ s;
	std::ifstream in;
	if (!in) {
		std::cout << "File not found: " + fileName + "\n";
		return;
	}
	in.open(fileName, std::ios::binary);

	in.read(reinterpret_cast<char*>(&s), sizeof(s));
	position = { s.x, s.z };

	blockStore = new BlockStore(position);

	unsigned size = 0;
	in.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
	if (size != 0) {
		std::vector<BlockData> blocks(size);
		in.read(reinterpret_cast<char*>(&blocks[0]), size * sizeof(BlockData));
		blockStore->getEditedBlocks().clear();
		for (auto& data : blocks) {
			blockStore->getEditedBlocks().insert(data.toBlockD());
		}
	}
	size = 0;
	in.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
	std::vector<Tuple_> elements(size);
	for (GLuint i = 0; i < size; i++) {
		Tuple1& element = elements[i].data;
		in.read(reinterpret_cast<char*>(&element), sizeof(element));
		std::vector<glm::mat4> models(element.size);
		for (GLuint j = 0; j < element.size; j++) {
			glm::mat4& model = models[j];
			in.read(reinterpret_cast<char*>(&model), sizeof(model));
		}
		elements[i].models = models;
	}
	in.close();

	for (auto& element : elements) {
		FaceB_p data = element.toFace();
		Buffer* b = std::get<0>(data);
		Texture* t = std::get<1>(data);
		mesh.insert({ std::pow(b->type + 1, 2) * std::pow(t->getTexMap() + 1, 3), data });
	}
	// hasCaves = 0;
	// GLuint seed = world_generation::seed;
	// world_generation::seed = s.seed;
	blockStore->setHeightMap(world_generation::createHeightMap(position, 32));
	// world_generation::seed = seed;
	isFlat = 0;
}

ChunkColumn::ChunkColumn(glm::vec2 pos, WorldMap* worldMap) : position(pos), highest_natural_point(-1), mesh(), isFlat(0), stage(0), fromFile(0), needsSave(0) {
	blockStore = &(*worldMap)[pos];
	if (!blockStore->isInitilised()) {
		(*worldMap)[pos] = BlockStore(pos);
	}
}
#pragma endregion

#pragma region Creation
void ChunkColumn::createMeshNew(WorldMap* worldMap) {
	const std::list<glm::vec3> offsets = {
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),

		glm::vec3(-1, 0, 0),
		glm::vec3(1, 0, 0),

		glm::vec3(0, -1, 0),
		glm::vec3(0, 1, 0)

	};

	std::vector<GeomData> bufferData;
	for (int z = 0; z < CHUNK_SIZE; z++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			GeomData d{};
			d.worldPos_ = glm::vec3(x, 20, z) + position;
			d.cubeType_ = TOP;
			bufferData.push_back(d);
			continue;

			std::vector<Block_Count>& encodes = blockStore->getBlocksAt(x, z);
			int height = 0;
			for (const Block_Count& bc : encodes) {
				height += bc.second;
			}

			for (auto itt = encodes.rbegin(); itt != encodes.rend(); itt++) {
				Block_Count& bc = *itt;
				GeomData data{};
				data.worldPos_ = glm::vec3(x, height--, z) + position;

				const BlockDet& blockDets = getDets(bc.first);
				data.textureIndex_ = (unsigned int)bc.first;

				int i = 0;
				for (const glm::vec3& off : offsets) {
					const glm::vec3 p = data.worldPos_ + off;
					const Blocks& b = getBlock(p, true, true, worldMap);

					const BlockDet& blockDets2 = getDets(b);

					if (blockDets2.isTransparant && blockDets.Name != "water" && blockDets2.Name != "water") {
						data.cubeType_ = i;
						bufferData.push_back(data);
					}
					i++;
				}
			}
		}
	}

	buffer.setUp(bufferData.data(), bufferData.size());
	stage = PARTS_PER_CHUNK;
}
void ChunkColumn::createMesh(WorldMap* worldMap)
{
	try {
		if (stage >= 101) stage = 0;
	}
	catch (std::exception e) {
		return;
	}
	// for 4
	/*
	GLubyte startX[] = {
		0, 8, 0, 8
	};
	GLubyte startZ[] = {
		0, 8, 8, 0
	};
	GLubyte endX[] = {
		8, 16, 8, 16
	};
	GLubyte endZ[] = {
		8, 16, 16, 8
	};*/


	// for 8
	GLubyte startX[] = {
		0, 0, 0, 0, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 12, 12
	};
	GLubyte startZ[] = {
		0, 4, 8, 12, 0, 4, 8, 12,0, 4, 8, 12, 0, 4, 8, 12
	};
	GLubyte endX[] = {
		4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 12, 12, 16, 16, 16, 16
	};
	GLubyte endZ[] = {
		4, 8, 12, 16, 4, 8, 12, 16,4, 8, 12, 16, 4, 8, 12, 16
	};

	endX[0] = 16;
	endZ[0] = 16;

	GLubyte lookDepth = 5;
	for (GLubyte x = startX[stage]; x < endX[stage]; x++)
	{
		for (GLubyte z = startZ[stage]; z < endZ[stage]; z++)
		{
			std::vector<Block_Count>& encodes = blockStore->getBlocksAt(x, z);
			GLuint y = 0;
			GLint height = 0;
			Blocks block = Blocks::AIR;
			std::vector<Blocks> blks;
			for (Block_Count& encoded : encodes) {
				height += encoded.second;
			}
			GLint maxHeigh = height-1;
			height = 0;
			for (Block_Count& encoded : encodes) {
				if (encoded.second > WORLD_HEIGHT) continue;
				for (GLuint i = 0; i < encoded.second; i++) {
					height++;
					if (height >= maxHeigh - lookDepth - 1 || true) {
						blks.insert(blks.begin(), (encoded.first));
					}
				}
			}
			y = height - 1;
			GLuint lookD = /*blks.size() >= lookDepth ? lookDepth : */blks.size();
			for (GLubyte i = 0; i < lookD; i++) {
				block = blks[i];
				addBlock({ x, y - i, z }, 0, block, worldMap);
			}
		}
	}
	stage++;
	stage = 16;
}

void ChunkColumn::createMesh(AdjacentMap& adjacentCunks, HeightMap heighMap)
{
}

void ChunkColumn::createMesh_flat(AdjacentMap& adjacentCunks)
{
	isFlat = 1;
	for (GLubyte x = 0; x < CHUNK_SIZE; x++)
	{
		for (GLuint y = 0; y < WORLD_HEIGHT; y++)
		{
			for (GLubyte z = 0; z < CHUNK_SIZE; z++)
			{
				Blocks block = Blocks::GRASS;
				// addBlock({ x, y, z }, 0, block, adjacentCunks);
			}
		}
	}
}
void ChunkColumn::addTrees() {
	if (blockStore->doesHaveTrees()) return;
	blockStore->setHaveTrees(1);
	AdjacentMap_p adjacent;
	std::vector<glm::vec2> treePos = world_generation::getTreePositions(position);
	for (auto& pos : treePos) {
		GLubyte x = pos.x;
		GLubyte z = pos.y;
		auto t = getHeightAt(pos, GL_TRUE, adjacent);
		if (getBlock({ x, std::get<1>(t) - 1, z }, 0, 0) != Blocks::GRASS) {
			continue;
		}
		std::vector<Block_Count>* encoded = std::get<0>(t);
		GLuint maxHeight = std::get<1>(t);
		ChunkColumn* column = std::get<2>(t);
		if (!column) continue;
		encoded->push_back({ Blocks::LOG, 4 });
		encoded->push_back({ Blocks::LEAF, 1 });
		GLuint row = maxHeight + 2;
		/*column->editBlock({ x, row - 1, z }, 0, Blocks::LOG, adjacentPtr);
		column->editBlock({ x, row, z }, 0, Blocks::LOG, adjacentPtr);
		column->editBlock({ x, row + 1, z }, 0, Blocks::LOG, adjacentPtr);
		column->editBlock({ x, row + 2, z }, 0, Blocks::LOG, adjacentPtr);

		column->editBlock({ x, row + 3, z }, 0, Blocks::LEAF, adjacentPtr);
		column->editBlock({ x, row + 4, z }, 0, Blocks::LEAF, adjacentPtr);*/
		// return;
		GLuint diff = 0;
		t = getHeightAt({ x + 1, z }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x - 1, z }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x, z + 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x, z - 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 3/* < 0 ? diff : 0*/ });
		}
		// corners
		t = getHeightAt({ x + 1, z + 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x + 1, z - 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x - 1, z + 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}

		t = getHeightAt({ x - 1, z - 1 }, GL_TRUE, adjacent);
		encoded = std::get<0>(t);
		maxHeight = std::get<1>(t);
		column = std::get<2>(t);
		if (column) {
			diff = row - maxHeight;
			if (diff > 0) {
				encoded->push_back({ Blocks::AIR, diff });
			}
			encoded->push_back({ Blocks::LEAF, 2/* < 0 ? diff : 0*/ });
		}
	}
}
#pragma endregion

#pragma region Getters
std::unordered_map<GLuint, FaceB_p>& ChunkColumn::getMesh()
{
	return mesh;
}
glm::vec2 ChunkColumn::getPosition()
{
	return position;
}

std::pair<Blocks, ChunkColumn*> ChunkColumn::getBlock_ChunkPos(glm::vec3 worldPosition, AdjacentMap_p& allChunks)
{
	glm::vec3 reduced = reduceToMultiple(worldPosition, CHUNK_SIZE);
	glm::vec2 chunkPos = { reduced.x, reduced.z };
	GLuint size = allChunks.size();
	ChunkColumn*& chunk = allChunks[chunkPos];
	if (allChunks.size() == size) {
		return { chunk->getBlock(worldPosition, 1), chunk };
	}
	else {
		allChunks.erase(chunkPos);
	}
	return { Blocks::AIR, nullptr };
}

Blocks ChunkColumn::getBlock(glm::vec3 pos, GLboolean worldPos, GLboolean checkEdited)
{
	return blockStore->getBlock(pos, worldPos, checkEdited);
}
glm::vec3 ChunkColumn::getRelativePosition(glm::vec3 worldPos)
{
	return worldPos - position;
}
glm::vec3 ChunkColumn::getWorldPosition(glm::vec3 relativePos)
{
	return relativePos + position;
}
std::tuple<std::vector<Block_Count>*, GLuint, ChunkColumn*> ChunkColumn::getHeightAt(glm::vec2 pos, GLboolean safe, AdjacentMap_p& adjacent)
{
	std::tuple<std::vector<Block_Count>*, GLuint, ChunkColumn*> t = { nullptr, 0, nullptr };
	if (!this) return t;
	GLuint maxHeight = 0;
	if (glm::all(glm::greaterThanEqual(pos, { 0, 0 }) && glm::lessThan(pos, { CHUNK_SIZE, CHUNK_SIZE }))) {
		auto& encoded = blockStore->getBlocksAt(pos);
		std::for_each(encoded.begin(), encoded.end(), [&](Block_Count& encode) { maxHeight += encode.second; });
		return { &encoded, maxHeight, this };
	}
	if (!safe) return t;
	glm::vec2 chunkPositionToLookAt = position;
	if (pos.x < 0) {
		pos.x += CHUNK_SIZE;
		chunkPositionToLookAt.x -= CHUNK_SIZE;
	}
	else if (pos.x > CHUNK_SIZE - 1) {
		pos.x -= CHUNK_SIZE;
		chunkPositionToLookAt.x += CHUNK_SIZE;
	}

	if (pos.y < 0) {
		pos.y += CHUNK_SIZE;
		chunkPositionToLookAt.y -= CHUNK_SIZE;
	}
	else if (pos.y > CHUNK_SIZE - 1) {
		pos.y -= CHUNK_SIZE;
		chunkPositionToLookAt.y += CHUNK_SIZE;
	}
	if (adjacent.size() > 0) {

		return adjacent[chunkPositionToLookAt]->getHeightAt(pos, 0, adjacent);
	}
	return t;
}
GLboolean ChunkColumn::needsSaving()
{
	return needsSave;
}
#pragma endregion

#pragma region Operations
void ChunkColumn::editBlock(glm::vec3 pos, GLboolean worldPos, Blocks block, WorldMap* worldmap, AdjacentMap_p& allChunks)
{
	glm::vec3 worldPosition = worldPos ? pos : getWorldPosition(pos);
	glm::vec3 relativePos = getRelativePosition(worldPosition);

	std::vector<Face> toKill;
	std::vector<Face> toAdd;
	GLfloat x = relativePos.x;
	GLfloat y = relativePos.y;
	GLfloat z = relativePos.z;
	Blocks block_at = getBlock(relativePos, 0);
	if (block == Blocks::AIR) { // breaking block
		if (block_at == Blocks::AIR) return;
		GLubyte tex_index = (GLubyte)getTexture(block_at);

		for (GLubyte i = 0; i < 6; i++)
		{
			removeFromMesh({ FACES[i], TEXTURES[tex_index], worldPosition });
		}

		#pragma region Checking adjacent blocks
		// x
		Blocks found = getBlock({ x - 1, y, z }, 0, 1, worldmap);
		if (found != Blocks::AIR) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(-1, 0, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[RIGHT], TEXTURES[tex_index2], worldPosition + glm::vec3(-1, 0, 0) });
		}
		found = getBlock({ x + 1, y, z }, 0, 1, worldmap);
		if (found != Blocks::AIR) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(1, 0, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[LEFT], TEXTURES[tex_index2], worldPosition + glm::vec3(1, 0, 0) });
		}

		// y
		
		found = getBlock({ x, y - 1, z }, 0, 1, worldmap);
		if (found != Blocks::AIR) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, -1, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[TOP], TEXTURES[tex_index2], worldPosition + glm::vec3(0, -1, 0) });
		}

		found = getBlock({ x, y + 1, z }, 0, 1, worldmap);
		if (found != Blocks::AIR) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 1, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[BOTTOM], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 1, 0) });
		}

		// z
		found = getBlock({ x, y, z - 1 }, 0, 1, worldmap);
		if (found != Blocks::AIR) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, -1), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[FRONT], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, -1) });
		}
		found = getBlock({ x, y, z + 1 }, 0, 1, worldmap);
		if (found != Blocks::AIR) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, 1), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[BACK], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, 1) });
		}
		#pragma endregion
	}
	else {
		toAdd.clear();
		toKill.clear();
		if (block_at != Blocks::AIR) return;
		GLubyte tex_index = (GLubyte)getTexture(block);
		#pragma region Checking adjacent blocks
		Blocks found = getBlock({ x - 1, y, z }, 0, 1, worldmap);
		GLboolean placed = 0;
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh({ FACES[LEFT], TEXTURES[tex_index], worldPosition });
				placed = 1;
			}
		}
		if (!placed) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(-1, 0, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[RIGHT], TEXTURES[tex_index2], worldPosition + glm::vec3(-1, 0, 0) });
		}
		placed = 0;

		found = getBlock({ x + 1, y, z }, 0, 1, worldmap);
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh({ FACES[RIGHT], TEXTURES[tex_index], worldPosition });
				placed = 1;
			}
		}
		if (!placed) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(1, 0, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[LEFT], TEXTURES[tex_index2], worldPosition + glm::vec3(1, 0, 0) });
		}
		placed = 0;

		// y
		found = getBlock({ x, y - 1, z }, 0, 1, worldmap);
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh({ FACES[BOTTOM], TEXTURES[tex_index], worldPosition });
				placed = 1;
			}
		}
		if (!placed) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, -1, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[TOP], TEXTURES[tex_index2], worldPosition + glm::vec3(0, -1, 0) });
		}
		placed = 0;

		found = getBlock({ x, y + 1, z }, 0, 1, worldmap);
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh({ FACES[TOP], TEXTURES[tex_index], worldPosition });
				placed = 1;
			}
		}
		if (!placed) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 1, 0), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[BOTTOM], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 1, 0) });
		}
		placed = 0;

		// z
		found = getBlock({ x, y, z - 1 }, 0, 1, worldmap);
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh({ FACES[BACK], TEXTURES[tex_index], worldPosition });
				placed = 1;
			}
		}
		if (!placed) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, -1), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[FRONT], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, -1) });
		}
		placed = 0;

		found = getBlock({ x, y, z + 1 }, 0, 1, worldmap);
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh({ FACES[FRONT], TEXTURES[tex_index], worldPosition });
				placed = 1;
			}
		}
		if (!placed) {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, 1), allChunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[BACK], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, 1) });
		}
		placed = 0;
		#pragma endregion
	}
	GLuint size = blockStore->getEditedBlocks().size();
	auto& blockStored = blockStore->getEditedBlocks()[relativePos];
	if (blockStore->getEditedBlocks().size() > size) {
		blockStored = block;
	}
	else {
		if (blockStore->getBlock(relativePos, 0, 0) == block) {
			blockStore->getEditedBlocks().erase(relativePos);
		}
		else {
			blockStored = block;
		}
	}
	needsSave = 1;
}

void ChunkColumn::save(std::string name, GLuint seed) {
	if (!needsSave) return;
	name = "Chunks/" + name + ".dat";
	remove(name.c_str()); // deletes it

	std::ofstream out(name.c_str(), std::ios::binary | std::ios::app);
	if (!out) {
		std::cout << "Cannot open file: " << name << std::endl;
		return;
	}
	Savable_ s = {
		position.x, position.y,
		seed
	};
	out.write((char*)&s, sizeof(s)); // pos and seed
	unsigned size = blockStore->getEditedBlocks().size();
	out.write(reinterpret_cast<char*>(&size), sizeof(unsigned));
	if(size != 0){
		std::vector<BlockData> blockData;
		for (auto& edit : blockStore->getEditedBlocks()) {
			blockData.push_back({ edit.first, toIndex(edit.second) });
		}
		out.write(reinterpret_cast<char*>(&blockData[0]), blockData.size() * sizeof(BlockData)); // block data
	}

	// mesh
	std::vector<Tuple_> meshes;
	for (auto& pair : mesh) {
		FaceB_p& faces = pair.second;
		Buffer*& b = std::get<0>(faces);
		Texture*& t = std::get<1>(faces);
		std::vector<glm::mat4>& modles = std::get<2>(faces);
		meshes.push_back({ { (GLubyte)b->type, (GLubyte)t->getTexMap(), (GLuint) modles.size() }, modles });
	} // convert to useable data
	size = meshes.size();
	out.write(reinterpret_cast<char*>(&size), sizeof(unsigned)); // size of mesh
	if (meshes.size() == 0) {
		out.close();
		return;
	}
	for (auto& mesh : meshes) {
		size = mesh.data.size;
		if (size == 0) continue;
		out.write((char*)&mesh.data, sizeof(mesh.data)); // mesh data
		out.write(reinterpret_cast<char*>(&mesh.models[0]), mesh.models.size() * sizeof(glm::mat4)); // model data
	}
	// out.write(reinterpret_cast<char*>(&meshes[0]), meshes.size() * sizeof(Tuple_)); // mesh data
	out.close();
}

void ChunkColumn::save(GLuint seed) {
	save(getFileName(), seed);
}

void ChunkColumn::addToMesh(Face face)
{
	Buffer* b = std::get<0>(face);
	Texture* t = std::get<1>(face);
	glm::vec3 pos = std::get<2>(face);
	GLuint key = std::pow(b->type + 1, 2) * std::pow(t->getTexMap() + 1, 3);
	glm::mat4 model = glm::translate(glm::mat4(1), pos);
	try {
		mesh.at(key);
	}
	catch (std::exception e) {
		mesh.insert({ key, { b, t, { } } });
	}
	FaceB_p& faces = mesh.at(key);
	std::vector<glm::mat4>& models = std::get<2>(faces);
	std::vector<glm::mat4>::iterator found = std::find(models.begin(), models.end(), model);
	if (found == models.end()) {
		models.push_back(model);
	}
}

void ChunkColumn::removeFromMesh(Face face)
{
	Buffer* b = std::get<0>(face);
	Texture* t = std::get<1>(face);
	glm::vec3 pos = std::get<2>(face);
	GLuint key = std::pow(b->type + 1, 2) * std::pow(t->getTexMap() + 1, 3);
	glm::mat4 model = glm::translate(glm::mat4(1), pos);
	try {
		mesh.at(key);
	}
	catch (std::exception e) {
		return;
	}
	FaceB_p& faces = mesh.at(key);
	std::vector<glm::mat4>& models = std::get<2>(faces);
	
	std::vector<glm::mat4>::const_iterator found = std::find(models.begin(), models.end(), model);
	if (found != models.end()) {
		models.erase(found);
		if (!models.size()) {
			mesh.erase(key);
			return;
		}
	}
}

bool ChunkColumn::operator==(ChunkColumn chunk) {
	return chunk.position == position;
}
bool ChunkColumn::operator==(glm::vec2 pos)
{
	return position == pos;
}
#pragma endregion

#pragma endregion

#pragma region Private
// editors
void ChunkColumn::addBlock(glm::vec3 position, GLboolean worldPos, Blocks block, WorldMap* worldMap) {
	if (block == Blocks::AIR) return;
	glm::vec3 relativePos = worldPos ? getRelativePosition(position) : position;
	const GLfloat& x = relativePos.x;
	const GLfloat& y = relativePos.y;
	const GLfloat& z = relativePos.z;

	glm::vec3 worldPosition = worldPos ? position : getWorldPosition(position);
	auto addToMesh = [](Buffer* buffer, Texture* texture, glm::vec3 position, std::unordered_map<GLuint, FaceB_p>& mesh) {
		GLuint key = std::pow(buffer->type + 1, 2) * std::pow(texture->getTexMap() + 1, 3);
		glm::mat4 model = translate(glm::mat4(1), position);
		auto& faces = mesh[key];
		if (std::get<0>(faces) == nullptr) {
			faces = { buffer, texture, { model } };
		}
		else {
			std::get<2>(faces).push_back(model);
		}
	};

	GLuint tex_index = (GLuint)getTexture(block);

	glm::vec3 delta(1);
	Buffer* faces[] = { 
		FACES[RIGHT],
		FACES[TOP],
		FACES[FRONT],
		FACES[LEFT],
		FACES[BOTTOM],
		FACES[BACK]
	};
	for (GLubyte i = 0; i < 6; i++) {
		glm::vec3 lookingAt = relativePos;
		Buffer* face = faces[i];
		if (i > 2) {
			lookingAt[i - 3] -= delta[i - 3];
		}
		else {
			lookingAt[i] += delta[i];
		}
		Blocks found = getBlock(lookingAt, 0, 1, worldMap);
		if (getDets(found).isTransparant || getDets(block).isTransparant) {
			if (getDets(block).Name == "water" && getDets(found).Name != "water" || getDets(block).Name != "water") {
				addToMesh(face, TEXTURES[tex_index], worldPosition, mesh);
			}
		}
	}
}

// getters
Blocks ChunkColumn::getBlock(glm::vec3 pos, GLboolean worldPos, GLboolean safe, WorldMap* worldMap) {
	glm::vec3 relativePostion = worldPos ? getRelativePosition(pos) : pos;

	if (glm::all(glm::greaterThanEqual(relativePostion, { 0, 0, 0 }))) {
		if (glm::all(glm::lessThan(relativePostion, { CHUNK_SIZE, WORLD_HEIGHT, CHUNK_SIZE }))) {
			if (isFlat) return Blocks::GRASS;
			return getBlock(pos, 0);
		}
	}
	if (!safe) return Blocks::STONE;
	// safe

	glm::vec2 chunkPositionToLookAt = position;
	if (relativePostion.x < 0) {
		relativePostion.x += CHUNK_SIZE;
		chunkPositionToLookAt.x -= CHUNK_SIZE;
	}
	else if (relativePostion.x > CHUNK_SIZE - 1) {
		relativePostion.x -= CHUNK_SIZE;
		chunkPositionToLookAt.x += CHUNK_SIZE;
	}

	if (relativePostion.z < 0) {
		relativePostion.z += CHUNK_SIZE;
		chunkPositionToLookAt.y -= CHUNK_SIZE;
	}
	else if (relativePostion.z > CHUNK_SIZE - 1) {
		relativePostion.z -= CHUNK_SIZE;
		chunkPositionToLookAt.y += CHUNK_SIZE;
	}
	if (worldMap->size() > 0) {
		auto found = worldMap->find(chunkPositionToLookAt);
		if (found == worldMap->end()) return Blocks::STONE;
		return (*found).second.getBlock(relativePostion, 0);
	}
	return Blocks::STONE;
}
std::string ChunkColumn::getFileName()
{
	return "chunk" + std::to_string((int)position.x) + "," + std::to_string((int)position.y);
}
BlockStore* ChunkColumn::getBlockStore()
{
	return blockStore;
}
void ChunkColumn::setBlockStore(BlockStore* bs)
{
	blockStore = bs;
}
#pragma endregion

bool operator==(ChunkColumn* chunk, glm::vec2 pos)
{
	return *chunk == pos;
}
FaceB_p Tuple_::toFace() {
	Texture* tex = nullptr;
	for (auto& t : TEXTURES) {
		if (t->getTexMap() == data.textureMap) {
			tex = t;
			break;
		}
	}if (!tex) tex = TEXTURES[0];
	FaceB_p face = { FACES[data.bufferType], tex, models };
	return face;
}

std::pair<glm::vec3, Blocks> BlockData::toBlockD()
{
	return { pos, toBlock(block) };
}
