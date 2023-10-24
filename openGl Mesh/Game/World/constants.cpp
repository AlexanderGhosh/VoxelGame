#include "constants.h"
#include "../../Buffer.h"
#include "../../Textures/Texture.h"
#include "../../Mesh.h"

const std::array<Buffer*, 6> FACES = {
	Mesh::FaceMesh({ glm::vec3(0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0.5) },   // FRONT
	glm::vec3(0, 0, 1),
	{ glm::vec3(-1, -1, 1), glm::vec3(-1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	Mesh::FaceMesh({ glm::vec3(-0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5) },  // BACK
	glm::vec3(0, 0, -1),
	{ glm::vec3(-1, 1, -1), glm::vec3(-1), glm::vec3(1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1) }).getBuffer(),

	Mesh::FaceMesh({  glm::vec3(-0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, 0.5), /**/ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5)},  // LEFT
	glm::vec3(-1, 0, 0),
	{ glm::vec3(1, -1, -1), glm::vec3(1, -1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1) }).getBuffer(),

	Mesh::FaceMesh({ glm::vec3(0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5) },     // RIGHT
	glm::vec3(1, 0, 0),
	{ glm::vec3(-1, -1, 1), glm::vec3(-1, 1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	Mesh::FaceMesh({ glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5),glm::vec3(0.5), glm::vec3(0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, -0.5) },	  // TOP
	glm::vec3(0, 1, 0),
	{ glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1), glm::vec3(-1, -1, -1), glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, -1, 1) }).getBuffer(),

	Mesh::FaceMesh({ glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(-0.5), /**/ glm::vec3(-0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), },   // BOTTOM
	glm::vec3(0, -1, 0),
	{ glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1), glm::vec3(1), glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1) }).getBuffer()
}; 
const std::vector<Texture*>TEXTURES = {
	new Texture("grass", ""),
	new Texture("player/bottom", ""),
	new Texture("player/top", ""),
	new Texture("skybox", ""),
	new Texture("stone", ""),
	new Texture("dirt", ""),
	new Texture("water", ""),
	new Texture("error", ""),
	new Texture("log", ""),
	new Texture("leaf", ""),
	new Texture("vampire/bottom", ""),
	new Texture("vampire/top", ""),
	new Texture("sand", "")
};
const std::vector<Texture*>TEXTURES2D = {
	new Texture("crosshair", ""),
	new Texture("boarders/normal", ""),
	new Texture("boarders/selected", ""),
	new Texture("hearts/live_heart", ""),
	new Texture("hearts/dead_heart", "")
};
const std::vector<Shader*>SHADERS = {
	new Shader("block2"),
	new Shader("block3"),
	new Shader("skybox"),
	new Shader("crosshair"),
	new Shader("glyph"),
	new Shader("ray"),
	new Shader("depth")
};
std::map<Blocks, BlockDet> BLOCKS = {
	{ Blocks::AIR, {} },
	{ Blocks::GRASS, {} },
	{ Blocks::DIRT, {} },
	{ Blocks::STONE, {} },
	{ Blocks::WATER, {} },
	{ Blocks::LOG, { } },
	{ Blocks::LEAF, { } },
	{ Blocks::SAND, { } }
};
const std::vector<Material> MATERIALS = {
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(0), 0 }, // grass
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // player bottom
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // player top
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // skybox
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(1), 32 }, // stone
	{ glm::vec3(0.3), glm::vec3(1), glm::vec3(0), 0 }, // dirt
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(1), 64 }, // water
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // error
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(0), 0 }, // log
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(1), 64 }, // leaf
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // vampire bottom
	{ glm::vec3(1), glm::vec3(1), glm::vec3(1), 1 }, // vampire top
	{ glm::vec3(0.4), glm::vec3(1), glm::vec3(0), 0 }, // sand
};
glm::vec3 LIGHTPOSITION = glm::vec3(-8, 80, -8);
glm::vec3 LIGHTPOSITIONOrigin = glm::vec3(-8, 80, -8);

void reduceToMultiple(glm::ivec3& victim, GLuint multiple, const char* overload) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0 || victim.y >= 0) {
		victim.y -= 1;
	}
	while (victim.z % multiple != 0) {
		victim.z -= 1;
	}
}
void reduceToMultiple(glm::ivec2& victim, GLuint multiple, const char* overload) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0) {
		victim.y -= 1;
	}
}
glm::ivec3 reduceToMultiple(glm::ivec3 victim, GLuint multiple) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0 || victim.y >= 0) {
		victim.y -= 1;
	}
	while (victim.z % multiple != 0) {
		victim.z -= 1;
	}
	return victim;
}
GLint reduceToMultiple(GLfloat victim, GLuint multiple) {
	victim = std::round(victim);
	while ((GLint)victim % multiple != 0) {
		victim--;
	}
	return victim;
}
Texture_Names getTexture(Blocks block) {
	Texture_Names res = Texture_Names::ERROR;
	switch (block)
	{
	case Blocks::AIR:
		break;
	case Blocks::GRASS:
		res = Texture_Names::GRASS;
		break;
	case Blocks::DIRT:
		res = Texture_Names::DIRT;
		break;
	case Blocks::STONE:
		res = Texture_Names::STONE;
		break;
	case Blocks::WATER:
		res = Texture_Names::WATER;
		break;
	case Blocks::LOG:
		res = Texture_Names::LOG;
		break;
	case Blocks::LEAF:
		res = Texture_Names::LEAF;
		break;
	case Blocks::SAND:
		res = Texture_Names::SAND;
		break;
	}
	if (res == Texture_Names::ERROR) {
		// std::cout << "Error Texture created\n";
	}
	return res;
}
Texture* getBlockTexture(Blocks block) {
	return BLOCKS[block].Tex;
}
GLubyte toIndex(Texture_Names tex) {
	return GLubyte(tex);
}
GLubyte toIndex(Blocks block) {
	return GLubyte(block);
}
Blocks toBlock(GLubyte number) {
	return Blocks(number);
}
std::string getName(Blocks block) {
	std::string name("null");
	switch (block)
	{
	case Blocks::AIR:
		name = "air";
		break;
	case Blocks::GRASS:
		name = "grass";
		break;
	case Blocks::DIRT:
		name = "dirt";
		break;
	case Blocks::STONE:
		name = "stone";
		break;
	case Blocks::WATER:
		name = "water";
		break;
	case Blocks::LOG:
		name = "log";
		break;
	case Blocks::LEAF:
		name = "leaf";
		break;
	case Blocks::SAND:
		name = "sand";
		break;
	}
	return name;
}
glm::vec3 getTranslation(glm::mat4 matrix) {
	return { matrix[3][0], matrix[3][1], matrix[3][2] };
}

BlockDet& getDets(Blocks block) {
	return BLOCKS[block];
}

BlockDet& getDets(Texture* tex) {
	for (auto& b : BLOCKS) {
		if (tex->getName().find(b.second.Name) != std::string::npos) {
			return b.second;
		}
	}
	return BLOCKS[Blocks::AIR];
}

Material getMaterial(const Texture_Names& block)
{
	return MATERIALS[toIndex(block)];
}

Material getMaterial(const std::string& texName)
{
	for (GLuint i = 0; i < TEXTURES.size(); i++) {
		if (texName == TEXTURES[i]->getName()) {
			return getMaterial(Texture_Names(i));
		}
	}
}

glm::mat4 translate(glm::mat4 mat, glm::vec3 vec) {
	for (unsigned char i = 0; i < 3; i++) {
		mat[3][i] = vec[i];
	}
	return mat;
}
void translate(glm::mat4& mat, glm::vec3 vec) {
	for (unsigned char i = 0; i < 3; i++) {
		mat[3][i] = vec[i];
	}
}

Blocks itemToBlock(Item item)
{
	switch (item)
	{
	case 0:
		return Blocks::AIR;
	case 1:
		return Blocks::GRASS;
	case 2:
		return Blocks::DIRT;
	case 3:
		return Blocks::STONE; 
	case 4:
		return Blocks::WATER;
	case 5:
		return Blocks::ERROR;
	case 6:
		return Blocks::LOG;
	case 7:
		return Blocks::LEAF;
	case 8:
		return Blocks::SAND;
	}
}
Blocks toBlock(std::string name) {
	for (auto& b : BLOCKS) {
		if (name == b.second.Name) {
			return b.first;
		}
	}
	return Blocks::ERROR;
}

std::vector<Face> toFaces(FaceB_p face)
{
	std::vector<Face> res;
	Buffer* b = std::get<0>(face);
	Texture* t = std::get<1>(face);
	std::vector<glm::mat4>& models = std::get<2>(face);
	for (glm::mat4& model : models) {
		res.push_back({ b, t, getTranslation(model) });
	}
	return res;
}

glm::vec3 operator+(glm::vec3 p1, glm::vec2 p2)
{
	return p1 + glm::vec3(p2.x, 0, p2.y);
}

glm::vec3 operator+(glm::vec2 p1, glm::vec3 p2)
{
	return p2 + glm::vec3(p1.x, 0, p1.y);
}

glm::vec3 operator-(glm::vec3 p1, glm::vec2 p2)
{
	return p1 - glm::vec3(p2.x, 0, p2.y);
}
glm::vec3 operator-(glm::vec2 p1, glm::vec3 p2)
{
	return glm::vec3(p1.x, 0, p1.y) - p2;
}

void Timer::start()
{
	start_ = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
	stop_ = std::chrono::high_resolution_clock::now();
}

void Timer::end() {
	stop();
}

GLulong Timer::getTime()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(stop_ - start_).count();
}

void Timer::showTime(std::string name, GLboolean inFrames)
{
	std::string unit = " secconds";
	GLfloat time = (GLfloat)getTime() / 1000000.0f;
	if (inFrames) {
		time /= (1.0f / 60.0f);
		unit = " frames";
	}
	std::cout << "Timer " + name + ": " + std::to_string(time) + unit << std::endl;
}
