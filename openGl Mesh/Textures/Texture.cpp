#include "Texture.h"

#include <iostream>
#include <vector>


#include <glad/glad.h>

Texture::Texture() : _created(false), _dimentions(), _texMap(), _is2D(), _name() { }

Texture::Texture(std::string name, std::string overload) : Texture() {
	_name = name;
}

Texture::Texture(std::string name) : Texture() {
	_name = name;
	_is2D = true;
	_created = load2D(name);
}
Texture::~Texture()
{
	//glDeleteTextures(1, &_texMap);
}
Texture::Texture(bool loadTex) : Texture() {
	if (loadTex) {
		_created = load3D("grass");
	}
}

bool Texture::load2D(const std::string& n) {
	std::string name = "./Textures/" + n + ".png";
	_is2D = true;
	glGenTextures(1, &_texMap);
	// diffuse
	// "C:\Users\ghosh\Desktop\openGl Mesh\openGl Mesh\Textures\hearts\live_heart.png"
	unsigned char* image = SOIL_load_image(name.c_str(), &_dimentions.x, &_dimentions.y, 0, SOIL_LOAD_RGBA);
	if (!image) {
		std::cout << "texture error | " << name << std::endl;
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, _texMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _dimentions.x, _dimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST_MIPMAP_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST_MIPMAP_NEAREST
	_created = true;
	return true;
}

bool Texture::load3D(const std::string& name) {
	_name = name;
	_is2D = false;
	std::vector<std::string> faces = {
		"Textures/" + name + "/left.png",	// left
		"Textures/" + name + "/front.png",	// front
		"Textures/" + name + "/bottom.png", // bottom
		"Textures/" + name + "/top.png",	// top
		"Textures/" + name + "/right.png",	// right
		"Textures/" + name + "/back.png"	// back
	};
	glGenTextures(1, &_texMap);
	// glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texMap);
	// texMap = texMap;
	unsigned char* data;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = SOIL_load_image(faces[i].c_str(), &_dimentions.x, &_dimentions.y, 0, SOIL_LOAD_RGBA);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, _dimentions.x, _dimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	_created = true;
	return true;
}

//Texture::Texture(Texture&& other) noexcept : Texture()
//{
//	_created = other._created;
//	_dimentions = other._dimentions;
//	_texMap = other._texMap;
//	_is2D = other._is2D;
//	_name = other._name;
//}
//
//Texture& Texture::operator=(Texture&& other) noexcept
//{
//	_created = other._created;
//	_dimentions = other._dimentions;
//	_texMap = other._texMap;
//	_is2D = other._is2D;
//	_name = other._name;
//	return *this;
//}

void Texture::bind() const {
	if (!_created) return;
	glActiveTexture(GL_TEXTURE0);
	if (_is2D) {
		glBindTexture(GL_TEXTURE_2D, _texMap);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texMap);
	}
}
void Texture::unBind() const {
	if (_is2D) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
const std::string& Texture::getName() const {
	return _name;
}
const unsigned int& Texture::getTexMap() const {
	return _texMap;
}
const bool Texture::get2D() const {
	return _is2D;
}
const glm::ivec2& Texture::getDimentions() const {
	return _dimentions;
}