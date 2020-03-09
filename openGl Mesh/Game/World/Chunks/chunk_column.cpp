#include "chunk_column.h"
chunk_column::chunk_column() :pos(0), hasCaves(0){
	genHeightMap(0);
}
chunk_column::chunk_column(GLboolean gen) : pos(0), hasCaves(0) {
	if(gen) genHeightMap(0);
}
chunk_column::chunk_column(ChunkPosition pos, GLboolean isFlat) : hasCaves(0) {
	this->pos = pos;
	createChunks();
	genHeightMap(isFlat);
}
chunk_column::chunk_column(ChunkPosition pos, ChunkHeightMap hm) : hasCaves(0) {
	this->pos = pos;
	this->heightMap = hm;
	createChunks();
	applyHeightMap();
}
void chunk_column::genHeightMap(GLboolean isFlat) {
	if (isFlat) {
		for (auto& chunk : chunks) {
			chunk.createBlocksFlat(Blocks::GRASS);
		}
	}
	else {
		heightMap = world_generation::genHeightMap(chunks[0].position);
		applyHeightMap();
	}
}
std::vector<Chunk*> getChunks(std::vector<Chunk>& chunks) {
	std::vector<Chunk*> res;
	for (auto& chunk : chunks) {
		res.push_back(&chunk);
	}
	return res;
}
void chunk_column::createMesh(std::vector<chunk_column*> columns, GLboolean getChanged, std::vector<std::pair<Face*, GLboolean>>& changed) {
	std::vector<Chunk*> chunks;
	Faces before;
	if (getChanged) {
		before = faces;
	}
	faces.clear();
	for (auto& column : columns) {
		std::vector<Chunk*> subChunks = getChunks(column->chunks);
		chunks.insert(chunks.end(), subChunks.begin(), subChunks.end());
	}
	for (auto& chunk : this->chunks) {
		chunk.createMesh(chunks);
		auto t = chunk.getMeshes();
		faces.insert(faces.end(), t.begin(), t.end());
	}
	if (getChanged) {
		std::vector<GLuint> indices;
		for (auto& face : before) {
			Faces::iterator found = std::find(faces.begin(), faces.end(), face);
			GLuint index = std::distance(faces.begin(), found);
			if (found == faces.end()) { // not found				changed.push_back({ face, 0 });
			}
			else { // found
				indices.push_back(index);
			}
		}
		for (GLuint i = 0; i < faces.size(); i++) {
			if (std::find(indices.begin(), indices.end(), i) == indices.end()) { // not found
				changed.push_back({ faces[i], 1 });
			}
		}
	}
}
glm::vec3 chunk_column::fromIndex(GLushort index) {
	glm::vec3 pos(this->pos.x, 0, this->pos.y);
	pos.y = -(index + 1) * CHUNK_SIZE;
	return pos;
}
void chunk_column::applyHeightMap() {
	if (hasCaves) {
		// deal with caves
	}
	else {
		for (GLubyte i = 0; i < chunks.size(); i++) {
			auto& chunk = chunks[i];
			if (i == 4) {
				chunk.createBlocks(heightMap);
			}
			else if (i < 4){
				chunk.createBlocksFlat(Blocks::AIR);
			}
			else {
				chunk.createBlocksFlat(Blocks::STONE);
			}
		}
	}
}
void chunk_column::addChunk(Chunk chunk, GLboolean sort) {
	chunks.push_back(chunk);
	if (sort) sortChunks();
}
void chunk_column::addChunks(std::vector<Chunk> chunks, GLboolean sort) {
	this->chunks.insert(this->chunks.end(), chunks.begin(), chunks.end());
	if (sort) sortChunks();
}
void chunk_column::sortChunks() {
	std::vector<std::pair<Chunk*, GLubyte>> chunkIndices;
	for (auto& chunk : chunks) {
		chunkIndices.push_back({ &chunk, std::abs(chunk.position.y / CHUNK_SIZE + 1) });
	}
	std::vector<Chunk> sorted;
	sorted.resize(chunks.size());
	for (auto& pair : chunkIndices) {
		sorted[pair.second] = *pair.first;
	}
	chunks = sorted;
}
std::pair<GLushort, GLushort> chunk_column::getHeightRange() {
	GLushort largest = 0, smallest = 0;
	for (GLushort x = 0; x < CHUNK_SIZE; x++) {
		for (GLushort z = 0; z < CHUNK_SIZE; z++) {
			if (hasCaves) {
				// deal with caves
			}
			else {
				Height height = heightMap[x][z];
				if (height > largest) largest = height;
				else if (height < smallest) smallest = height;
			}
		}
	}
	return { largest, smallest };
}
Faces chunk_column::getMesh(GLboolean reGen) {
	if(!reGen) return faces;
	faces.clear();
	for (auto& chunk : chunks) {
		auto fac = chunk.getPointerMesh();
		faces.insert(faces.end(), fac.begin(), fac.end());
	}
	return faces;
}
void chunk_column::createChunks() {
	chunks.clear();
	GLubyte number = 64 / CHUNK_SIZE + 4;
	for (GLubyte i = 0; i < number; i++) {
		chunks.push_back(Chunk({ pos.x, (-(i + 1) + 4) * CHUNK_SIZE, pos.y }, 0));
	}
}
ChunkPosition& chunk_column::getPosition() {
	return pos;
}
Blocks& chunk_column::getBlock(glm::ivec3 blockPos) {
	blockPos.y += CHUNK_SIZE;
	GLbyte index = std::abs(blockPos.y) - 1;
	if (index % 2 != 0) index -= 1; // if odd
	index /= CHUNK_SIZE;

	blockPos.y %= CHUNK_SIZE;

	index = 4;
	return chunks[index].blocks[getBlockIndex(blockPos)];
}
Chunk* chunk_column::getSubchunk_unsafe(GLuint yPos) {
	// to do
	return &chunks[4];
}