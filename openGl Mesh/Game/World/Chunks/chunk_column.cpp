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
chunk_column::chunk_column(std::string fileName) {
	fileName = "Chunks/" + fileName + ".dat";
	Savable s;
	std::ifstream in;
	if (!in) {
		std::cout << "File not found: " + fileName + "\n";
		return;
	}
	in.open(fileName, std::ios::binary);

	in.read(reinterpret_cast<char*>(&s), sizeof(s));
	pos = { s.x, s.z };
	createChunks();

	std::vector<GLubyte> blocks(4096*8);
	in.read(reinterpret_cast<char*>(&blocks[0]), 4096 * 8 * sizeof(GLubyte));
	unsigned size = 0;
	in.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
	std::vector<Tuple> elements(size);
	for (GLuint i = 0; i < size; i++) {
		Tuple& element = elements[i];
		in.read(reinterpret_cast<char*>(&element), sizeof(element));
	}
	in.close();


	for (auto& chunk : chunks) {
		chunk.meshes.clear();
	}
	for (auto& element : elements) {
		getSubchunk_unsafe(element.y)->meshes.push_back(element.toFace());
	}
	for (GLuint i = 0; i < chunks.size(); i++) {
		chunks[i].blocks.empty();
		for (GLuint j = i * 4096; j < (i + 1) * 4096; j++) {
			chunks[i].blocks[j - i * 4096] = toBlock(blocks[j]);
		}
	}
	hasCaves = 0;
}

chunk_column::~chunk_column() {
	/*for (Face* face : faces) {
		delete face;
	}
	faces.clear();*/
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
void chunk_column::createMesh(std::vector<chunk_column*> columns) {
	std::vector<Chunk*> chunks;
	Faces before;
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
}
void chunk_column::createMesh(std::vector<Chunk*> chunks, GLboolean create) {
	faces.clear();
	for (auto& chunk : this->chunks) {
		if(create) chunk.createMesh(chunks);
		auto t = chunk.getMeshes();
		faces.insert(faces.end(), t.begin(), t.end());
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
Chunk* chunk_column::getSubchunk_unsafe(GLint yPos) {
	// to do
	if (yPos >= 0) yPos += CHUNK_SIZE;
	yPos /= CHUNK_SIZE;
	if (yPos < 0) yPos *= -1;
	yPos *= -1;
	yPos += 4;
 	return &chunks[yPos];
}

void chunk_column::save(std::string name, GLuint seed) {
	name = "Chunks/" + name + ".dat";
	remove(name.c_str()); // deletes it
	std::vector<GLubyte> blocks;

	std::ofstream out(name.c_str(), std::ios::binary | std::ios::app);
	if (!out) {
		std::cout << "Cannot open file: " << name << std::endl;
		return;
	}
	for (auto& chunk : chunks) {
		for (auto& block : chunk.blocks) {
			blocks.push_back(toIndex(block));
		}
	}
	Savable s = {
		pos.x, pos.y,
		seed
	};
	out.write((char*)&s, sizeof(s)); // pos and seed
	out.write(reinterpret_cast<char*>(&blocks[0]), blocks.size() * sizeof(GLubyte)); // block data

	// mesh
	std::vector<Tuple> meshes;
	for (Face*& f : faces) {
		Buffer*& b = std::get<0>(*f);
		Texture*& t = std::get<1>(*f);
		glm::vec3& p = std::get<2>(*f);
		meshes.push_back({ (GLubyte)b->type, (GLubyte)t->getTexMap(), p.x, p.y, p.z });
	} // convert to useable data
	unsigned size = meshes.size();
	out.write(reinterpret_cast<char*>(&size), sizeof(unsigned)); // size of mesh
	if (meshes.size() == 0) {
		out.close();
		return;
	}
	out.write(reinterpret_cast<char*>(&meshes[0]), meshes.size() * sizeof(Tuple)); // mesh data
	out.close();
}

void chunk_column::destroy() {
	pos = { 0, 0 };
	hasCaves = 0;
	// delete[] heightMap.data();
	heightMap.empty();
	heightMap = {};
	/*for (Face*& face : faces) {
		delete face;
	}*/
	faces.clear();
	faces = {};
}

std::vector<Chunk*> chunk_column::getSubChunkPointers() {
	std::vector<Chunk*> res;
	for (Chunk& chunk : chunks) {
		res.push_back(&chunk);
	}
	return res;
}

void chunk_column::editBlock(glm::vec3 pos, GLboolean is_abs, Blocks block, std::vector<Chunk*> subs) {
	pos = glm::round(pos);
	if (is_abs) {
		pos -= glm::vec3(this->pos.x, 0, this->pos.y);
	}
	Chunk* sub = getSubchunk_unsafe(pos.y);
	sub->editBlock(pos, block, subs);
}

Face Tuple::toFace() {
	Texture* tex = nullptr;
	for (auto& t : TEXTURES) {
		if (t->getTexMap() == textureMap) {
			tex = t;
			break;
		}
	}if (!tex) tex = TEXTURES[0];
	Face face = { FACES[bufferType], tex, glm::vec3(x, y, z) };
	return face;
}


#pragma region Operators
bool chunk_column::operator ==(chunk_column chunk2) {
	return pos == chunk2.pos;
}
bool chunk_column::operator !=(chunk_column chunk2) {
	return pos != chunk2.pos;
}
bool chunk_column::operator ==(glm::vec2 pos2) {
	return pos == pos2;
}
bool chunk_column::operator !=(glm::vec2 pos2) {
	return pos != pos2;
}
#pragma endregion
