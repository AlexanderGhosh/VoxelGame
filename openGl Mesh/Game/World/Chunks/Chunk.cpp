#include "Chunk.h"
Chunk::Chunk() {
	null = GL_TRUE;
}
Chunk::Chunk(glm::vec3 pos, GLboolean create) {
	position = pos;
	null = GL_FALSE;
	blocks.fill(Blocks::AIR);
	if (create) {
		this->create();
	}
}
void Chunk::create() {
	createBlocksFlat(Blocks::AIR);
	createMesh(std::vector<Chunk*>());
}
void Chunk::createBlocksFlat(Blocks block) {
	if (position.y > -CHUNK_SIZE) block = Blocks::AIR;
	blocks.fill(block);

	if (position == glm::vec3(0, -2, 0) || position == glm::vec3(-2, -2, -2)) {
		//blocks[getBlockIndex({ 0, 0, 0 })] = 0;
		blocks[getBlockIndex({ 0, -1, 0 })] = Blocks::AIR;
		blocks[getBlockIndex({ 1, -1, 0 })] = Blocks::AIR;
		blocks[getBlockIndex({ 1, -1, 1 })] = Blocks::AIR;
		blocks[getBlockIndex({ 0, -1, 1 })] = Blocks::AIR;
	}
}
void Chunk::createBlocks(HeightMapChunk heightMap) {
	blocks.fill(Blocks::AIR);
	for (GLubyte x = 0; x < CHUNK_SIZE; x++) {
		for (GLubyte z = 0; z < CHUNK_SIZE; z++) {
			glm::vec3 position = glm::abs(this->position);
			GLubyte height = heightMap[x][z];
			if (height < 2) {
				blocks[getBlockIndex({ x, 0, z })] = Blocks::WATER;
				continue;
			}
			for (GLubyte y = 0; y < height; y++) {
				Blocks block = Blocks::GRASS; // grass
				if (y < height - 3) {
					block = Blocks::STONE; // stone
				}
				else if (y < height - 1) {
					block = Blocks::DIRT; // dirt
				}
				blocks[getBlockIndex({ x, y, z })] = block;
			}
		}
	}
}
void Chunk::createMesh(std::vector<Chunk*> chunks) {
	meshes.clear();
	for (GLint x = 0; x < CHUNK_SIZE; x++) {
		for (GLint y = 0; y < CHUNK_SIZE; y++) {
			for (GLint z = 0; z < CHUNK_SIZE; z++) {
				glm::vec3 pos = glm::vec3(x, y, z) + position;
				Blocks block = getBlock_safe({ x, y, z }, chunks);
				if (block == Blocks::AIR) continue;
				addBlock(block, chunks, pos, 1);
				
			}
		}
	}
}
void Chunk::cleanUp() {
	for (auto& face : meshes) {
		face = {};
	}
}

std::vector<Face*> Chunk::getMeshes() {
	std::vector<Face*> res;
	for (auto& mesh : meshes) {
		res.push_back(&mesh);
	}
	return res;
}
Blocks Chunk::getBlock_unsafe(glm::ivec3 pos) {
	auto p = pos;
	while (pos.x%CHUNK_SIZE != 0 && !(std::abs(pos.x) > CHUNK_SIZE || std::abs(pos.x) < CHUNK_SIZE)) {
		pos -= glm::ivec3(CHUNK_SIZE);
	}
	try {
		return blocks.at(getBlockIndex(pos));
	}
	catch (std::exception e) {
		return Blocks::AIR;
	}
}
Blocks& Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks) {
	Chunk* pointer;
	return getBlock_safe(inChunkPosition, chunks, pointer);
}
GLboolean Chunk::isNull() {
	return null;
}

void Chunk::addBlock(Blocks block, std::vector<Chunk*>& chunks, glm::vec3 absPos, GLboolean skip) {
	if (skip && block == Blocks::AIR) return;
	auto relativePos = absPos - position;
	auto& x = relativePos.x;
	auto& y = relativePos.y;
	auto& z = relativePos.z;
	GLuint tex_index = (GLuint)getTexture(block);
	if (getBlock_safe({ x - 1, y, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[LEFT], TEXTURES[tex_index], absPos });
	}
	if (getBlock_safe({ x + 1, y, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[RIGHT], TEXTURES[tex_index], absPos });
	}

	if (getBlock_safe({ x, y - 1, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[BOTTOM], TEXTURES[tex_index], absPos });
	}
	if (getBlock_safe({ x, y + 1, z }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[TOP], TEXTURES[tex_index], absPos });
	}

	if (getBlock_safe({ x, y, z - 1 }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[BACK], TEXTURES[tex_index], absPos });
	}
	if (getBlock_safe({ x, y, z + 1 }, chunks) == Blocks::AIR) {
		meshes.push_back({ FACES[FRONT], TEXTURES[tex_index], absPos });
	}
}

void Chunk::editBlock(glm::vec3 pos, Blocks block, std::vector<Chunk*> chunks) {
	std::vector<Face> victims;
	if (block == Blocks::AIR) {
		// break block
		for (GLuint i = 0; i < meshes.size(); i++) {
			auto& face = meshes[i];
			auto& p = std::get<2>(face);
			if (p == pos) {
				victims.push_back(face);
			}
		}
		for (auto& victim : victims) {
			auto found = std::find(meshes.begin(), meshes.end(), victim);
			meshes.erase(found);
		}
		glm::vec3 deltas[] = {
			glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
			glm::vec3( 1, 0, 0), glm::vec3(-1, 0, 0),
			glm::vec3(0, -1, 0), glm::vec3(0, 1, 0)
		};
		
		for (GLubyte i = 0; i < 6; i++) {
			auto& delta = deltas[i];
			auto p = pos + delta;
			Chunk* chunk = this;
			block = getBlock_safe(p - position, chunks, chunk);
			if (block == Blocks::AIR) continue;
			GLubyte tex = (GLuint)getTexture(block);
			Face face = { FACES[i], TEXTURES[tex], p };
			chunk->meshes.push_back(face);
		}
	}
	else {
		std::vector<Chunk*> chunks_;
		glm::vec3 deltas[] = {
			glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0),
			glm::vec3(0, -1, 0), glm::vec3(0, 1, 0)
		}; 

		for (GLubyte i = 0; i < 6; i++) {
			glm::vec3 delta = deltas[i];
			Chunk* chunk = this;
			Blocks b = getBlock_safe((pos + delta) - position, chunks, chunk);
			if (!chunk || b == Blocks::AIR) continue;
			std::vector<Face>& meshs = chunk->meshes;
			for (auto& face : meshs) {
				GLubyte b = std::get<0>(face)->type;
				glm::vec3& p = std::get<2>(face);
				if (p == delta + pos && b == FACES[i]->type) {
					victims.push_back(face);
					chunks_.push_back(chunk);
					break;
				}
			}
		}
		for (GLubyte i = 0; i < victims.size(); i++) {
			Face& victim = victims[i];
			std::vector<Face>& meshs = chunks_[i]->meshes;
			auto found = std::find(meshs.begin(), meshs.end(), victim);
			meshs.erase(found);
		}

		
		addBlock(block, chunks, pos, 1);
		getBlock_safe(pos - position, chunks) = block;
	}
}
std::vector<Face*> Chunk::getPointerMesh() {
	std::vector<Face*> res;
	for (auto& face : meshes) {
		res.push_back(&face);
	}
	return res;
}

Blocks& Chunk::getBlock_safe(const glm::vec3 inChunkPosition, std::vector<Chunk*> chunks, Chunk*& chunk_) {
	auto air = Blocks::AIR;
	if (glm::all(glm::greaterThanEqual(inChunkPosition, glm::vec3(0)))) {
		if (glm::all(glm::lessThan(inChunkPosition, glm::vec3(CHUNK_SIZE)))) {
			auto x = getBlockIndex(inChunkPosition);
			return blocks[x];
		}
	}
	glm::vec3 toLookAt = inChunkPosition;
	glm::vec3 chunkPositionToLookAt = position;

	glm::vec3 delta = inChunkPosition + glm::vec3(CHUNK_SIZE);
	glm::bvec3 comp = glm::lessThan(inChunkPosition, glm::vec3(0));
	delta *= comp;

	for (GLuint i = 0; i < 3; i++) {
		if (delta[i] != 0) {
			toLookAt[i] = delta[i];
		}
	}

	chunkPositionToLookAt -= glm::vec3(CHUNK_SIZE) * (glm::vec3)comp;

	auto t = glm::greaterThan(inChunkPosition, glm::vec3(CHUNK_SIZE - 1));
	comp = glm::not_(comp) && t;
	delta = glm::vec3(CHUNK_SIZE);
	toLookAt -= delta * (glm::vec3)comp;
	chunkPositionToLookAt += delta * (glm::vec3)comp;

	int index = getBlockIndex(toLookAt);

	if (index < 0) return air;
	for (auto& chunk : chunks) {
		if (chunk->position == chunkPositionToLookAt) {
			chunk_ = chunk;
			return chunk->blocks[index];
		}
	}
	return air;
}