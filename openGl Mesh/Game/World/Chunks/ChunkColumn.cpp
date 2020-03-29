#include "ChunkColumn.h"

#pragma region Public
#pragma region Constructors
ChunkColumn::ChunkColumn() : position(0), highest_natural_point(-1), mesh(), heightMap(), isFlat(1)
{
	// blocks.fill(Blocks::AIR);
}

ChunkColumn::ChunkColumn(glm::vec2 pos) : position(pos), highest_natural_point(-1), mesh(), heightMap(), isFlat(0)
{
	// blocks.fill(Blocks::GRASS);
}

ChunkColumn::ChunkColumn(std::string fileName) 
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
	unsigned size = 0;
	in.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
	if (size != 0) {
		std::vector<BlockData> blocks(size);
		in.read(reinterpret_cast<char*>(&blocks[0]), size * sizeof(BlockData));
		blocksEdited.clear();
		for (auto& data : blocks) {
			blocksEdited.insert(data.toBlockD());
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
		mesh.insert({ (int)b * (int)t, data });
	}
	/*for (GLuint i = 0; i < chunks.size(); i++) {
		chunks[i].blocks.empty();
		for (GLuint j = i * 4096; j < (i + 1) * 4096; j++) {
			chunks[i].blocks[j - i * 4096] = toBlock(blocks[j]);
		}
	}*/
	// hasCaves = 0;
	GLuint seed = world_generation::seed;
	world_generation::seed = s.seed;
	heightMap = world_generation::createHeightMap(position, 0);
	world_generation::seed = seed;
	isFlat = 0;
}

ChunkColumn::ChunkColumn(glm::vec2 pos, HeightMap heightMap) : position(pos), highest_natural_point(-1), mesh(), heightMap(heightMap), isFlat(0)
{
	// blocks.fill(Blocks::AIR);
}
#pragma endregion

#pragma region Creation
void ChunkColumn::createMesh(AdjacentMap& adjacentCunks)
{
	GLubyte lookDepth = 3;
	for (GLubyte x = 0; x < CHUNK_SIZE; x++)
	{
		for (GLubyte z = 0; z < CHUNK_SIZE; z++)
		{
			std::vector<Block_Count>& encodes = heightMap[x][z];
			GLuint y = 0;
			GLuint height = 0;
			Blocks block = Blocks::AIR;
			std::vector<Blocks> blks;
			for (Block_Count& encoded : encodes) {
				height += encoded.second;
			}
			GLuint maxHeigh = height-1;
			height = 0;
			for (Block_Count& encoded : encodes) {
				for (GLuint i = 0; i < encoded.second; i++) {
					height++;
					if (height >= maxHeigh - lookDepth - 1) {
						blks.insert(blks.begin(), (encoded.first));
					}
				}
			}
			y = height - 1;
			for (GLubyte i = 0; i < lookDepth; i++) {
				block = blks[i];
				addBlock({ x, y - i, z }, 0, block, adjacentCunks);
			}
		}
	}
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
				addBlock({ x, y, z }, 0, block, adjacentCunks);
			}
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

Blocks ChunkColumn::getBlock(glm::vec3 pos, GLboolean worldPos)
{
	glm::vec3 relativePos = worldPos ? getRelativePosition(pos) : pos;
	// check changed blocks
	GLuint size = blocksEdited.size();
	Blocks t = blocksEdited[relativePos];
	if (blocksEdited.size() > size) {
		blocksEdited.erase(relativePos);
	}
	else {
		return t;
	}

	// get from height map
	std::vector<Block_Count> encodes;
	glm::vec2 hPos(relativePos.x, relativePos.z);
	if (glm::all(glm::lessThan(hPos, glm::vec2(CHUNK_SIZE)) && glm::greaterThanEqual(hPos, { 0, 0 }))) {
		encodes = heightMap[hPos.x][hPos.y];
	}
	for (Block_Count&  encoded : encodes) {
		relativePos.y -= encoded.second;
		if (relativePos.y < 0) {
			return encoded.first;
		}
	}
	return Blocks::AIR;
}
glm::vec3 ChunkColumn::getRelativePosition(glm::vec3 worldPos)
{
	return worldPos - position;
}
glm::vec3 ChunkColumn::getWorldPosition(glm::vec3 relativePos)
{
	return relativePos + position;
}
#pragma endregion

#pragma region Operations
void ChunkColumn::editBlock(glm::vec3 pos, GLboolean worldPos, Blocks block, AdjacentMap_p& adjacentCunks)
{
	glm::vec3 worldPosition = worldPos ? pos : getWorldPosition(pos);
	glm::vec3 relativePos = getRelativePosition(worldPosition);

	AdjacentMap adjacentCunks_n;
	for (auto& adjacent : adjacentCunks) {
		adjacentCunks_n.insert({ adjacent.first, *adjacent.second });
	}

	if (block == Blocks::AIR) { // breaking block
		std::vector<Face> toKill;
		std::vector<Face> toAdd;
		GLfloat x = relativePos.x;
		GLfloat y = relativePos.y;
		GLfloat z = relativePos.z;
		Blocks block_at = getBlock(relativePos, 0);
		if (block_at == Blocks::AIR) return;
		GLubyte tex_index = (GLubyte)getTexture(block_at);
		#pragma region Checking adjacent blocks
		// x
		if (getBlock({ x - 1, y, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			removeFromMesh({ FACES[LEFT], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(-1, 0, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[RIGHT], TEXTURES[tex_index2], worldPosition + glm::vec3(-1, 0, 0) });
		}
		if (getBlock({ x + 1, y, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			removeFromMesh({ FACES[RIGHT], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(1, 0, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[LEFT], TEXTURES[tex_index2], worldPosition + glm::vec3(1, 0, 0) });
		}

		// y
		if (getBlock({ x, y - 1, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			removeFromMesh({ FACES[BOTTOM], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, -1, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[TOP], TEXTURES[tex_index2], worldPosition + glm::vec3(0, -1, 0) });
		}
		if (getBlock({ x, y + 1, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			removeFromMesh({ FACES[TOP], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 1, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[BOTTOM], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 1, 0) });
		}

		// z
		if (getBlock({ x, y, z - 1 }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			removeFromMesh({ FACES[BACK], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, -1), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[FRONT], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, -1) });
		}
		if (getBlock({ x, y, z + 1 }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			removeFromMesh({ FACES[FRONT], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, 1), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->addToMesh({ FACES[BACK], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, 1) });
		}
		#pragma endregion
	}
	else {
		std::vector<Face> toAdd;
		std::vector<Face> toKill;
		GLfloat x = relativePos.x;
		GLfloat y = relativePos.y;
		GLfloat z = relativePos.z;
		Blocks block_at = getBlock(relativePos, 0);
		if (block_at != Blocks::AIR) return;
		GLubyte tex_index = (GLubyte)getTexture(block);
		#pragma region Checking adjacent blocks
		if (getBlock({ x - 1, y, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			addToMesh({ FACES[LEFT], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(-1, 0, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[RIGHT], TEXTURES[tex_index2], worldPosition + glm::vec3(-1, 0, 0) });
		}
		if (getBlock({ x + 1, y, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			addToMesh({ FACES[RIGHT], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(1, 0, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[LEFT], TEXTURES[tex_index2], worldPosition + glm::vec3(1, 0, 0) });
		}

		// y
		if (getBlock({ x, y - 1, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			addToMesh({ FACES[BOTTOM], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, -1, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[TOP], TEXTURES[tex_index2], worldPosition + glm::vec3(0, -1, 0) });
		}
		if (getBlock({ x, y + 1, z }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			addToMesh({ FACES[TOP], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 1, 0), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[BOTTOM], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 1, 0) });
		}

		// z
		if (getBlock({ x, y, z - 1 }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			addToMesh({ FACES[BACK], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, -1), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[FRONT], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, -1) });
		}
		if (getBlock({ x, y, z + 1 }, 0, 1, adjacentCunks_n) == Blocks::AIR) {
			addToMesh({ FACES[FRONT], TEXTURES[tex_index], worldPosition });
		}
		else {
			auto block_at2 = getBlock_ChunkPos(worldPosition + glm::vec3(0, 0, 1), adjacentCunks);
			GLubyte tex_index2 = (GLubyte)getTexture(block_at2.first);
			block_at2.second->removeFromMesh({ FACES[BACK], TEXTURES[tex_index2], worldPosition + glm::vec3(0, 0, 1) });
		}
#pragma endregion
	}
	blocksEdited[relativePos] = block;
}

void ChunkColumn::save(std::string name, GLuint seed) {
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
	unsigned size = blocksEdited.size();
	out.write(reinterpret_cast<char*>(&size), sizeof(unsigned));
	if(size != 0){
		std::vector<BlockData> blockData;
		for (auto& edit : blocksEdited) {
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
		meshes.push_back({ { (GLubyte)b->type, (GLubyte)t->getTexMap(), modles.size() }, modles });
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
	GLuint key = (int)b * (int)t;
	glm::mat4 model = glm::translate(glm::mat4(1), pos);
	try {
		mesh.at(key);
	}
	catch (std::exception e) {
		mesh.insert({ key, { b, t, { } } });
	}
	FaceB_p& faces = mesh.at(key);
	std::vector<glm::mat4>& models = std::get<2>(faces);

	models.push_back(model);
}

void ChunkColumn::removeFromMesh(Face face)
{
	Buffer* b = std::get<0>(face);
	Texture* t = std::get<1>(face);
	glm::vec3 pos = std::get<2>(face);
	GLuint key = (int)b * (int)t;
	glm::mat4 model = glm::translate(glm::mat4(1), pos);
	try {
		mesh.at(key);
	}
	catch (std::exception e) {
		mesh.insert({ key, { b, t, { } } });
	}
	FaceB_p& faces = mesh.at(key);
	std::vector<glm::mat4>& models = std::get<2>(faces);

	std::vector<glm::mat4>::const_iterator found = std::find(models.begin(), models.end(), model);
	if (found != models.end()) {
		models.erase(found);
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
void ChunkColumn::addBlock(glm::vec3 position, GLboolean worldPos, Blocks block, AdjacentMap& adjacentCunks) {
	if (block == Blocks::AIR) return;
	glm::vec3 relativePos = worldPos ? getRelativePosition(position) : position;
	GLfloat& x = relativePos.x;
	GLfloat& y = relativePos.y;
	GLfloat& z = relativePos.z;

	glm::vec3 worldPosition = getWorldPosition(relativePos);
	auto addToMesh = [](Buffer* buffer, Texture* texture, glm::vec3 position, std::unordered_map<GLuint, FaceB_p>& mesh) {
		GLuint key = (int)buffer * (int)texture;
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
		if (!(int)getBlock(lookingAt, 0, 1, adjacentCunks)) {
			addToMesh(face, TEXTURES[tex_index], worldPosition, mesh);
		}
	}
}

// getters
Blocks ChunkColumn::getBlock(glm::vec3 pos, GLboolean worldPos, GLboolean safe, AdjacentMap& ajacentChunks) {
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
	if (ajacentChunks.size() > 0) {
		
		return ajacentChunks[chunkPositionToLookAt].getBlock(relativePostion, 0);
		
		/*Chunks::iterator found = std::find((ajacentChunks).begin(), (ajacentChunks).end(), chunkPositionToLookAt); 
		if (found != (ajacentChunks).end()) {
			if (isFlat) return Blocks::AIR;
			return (*found).getBlock(relativePostion, 0);
		}*/
	}
	return Blocks::STONE;
}
std::string ChunkColumn::getFileName()
{
	return "chunk" + std::to_string((int)position.x) + "," + std::to_string((int)position.y);
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
