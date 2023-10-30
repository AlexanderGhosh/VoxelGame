#include "Texture.h"

#include <iostream>
#include <vector>

// #define GLEW_STATIC
#include <glad/glad.h>

Texture::Texture() :created(false), dimentions(), texMap(), is2D(), name() { }

Texture::Texture(std::string name, std::string overload) : Texture() {
	this->name = name;
}

Texture::Texture(std::string name) : Texture() {
	this->name = name;
	this->is2D = true;
	created = load2D(name);
}
Texture::Texture(bool loadTex) : Texture() {
	if (loadTex) {
		created = load3D("grass");
	}
}
bool Texture::load2D(const std::string& n) {
	std::string name = "./Textures/" + n + ".png";
	is2D = true;
	glGenTextures(1, &texMap);
	// diffuse
	// "C:\Users\ghosh\Desktop\openGl Mesh\openGl Mesh\Textures\hearts\live_heart.png"
	unsigned char* image = SOIL_load_image(name.c_str(), &dimentions.x, &dimentions.y, 0, SOIL_LOAD_RGBA);
	if (!image) {
		std::cout << "texture error | " << name << std::endl;
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, texMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimentions.x, dimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST_MIPMAP_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST_MIPMAP_NEAREST
	created = true;
	return true;
}
bool Texture::load3D(const std::string& name) {
	this->name = name;
	is2D = false;
	std::vector<std::string> faces = {
		"Textures/" + name + "/left.png",	// left
		"Textures/" + name + "/front.png",	// front
		"Textures/" + name + "/bottom.png", // bottom
		"Textures/" + name + "/top.png",	// top
		"Textures/" + name + "/right.png",	// right
		"Textures/" + name + "/back.png"	// back
	};
	glGenTextures(1, &texMap);
	// glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);
	// texMap = texMap;
	unsigned char* data;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = SOIL_load_image(faces[i].c_str(), &dimentions.x, &dimentions.y, 0, SOIL_LOAD_RGBA);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, dimentions.x, dimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
	created = true;
	return true;
}
void Texture::bind() const {
	if (!created) return;
	glActiveTexture(GL_TEXTURE0);
	if (is2D) {
		glBindTexture(GL_TEXTURE_2D, texMap);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);
	}
}
void Texture::unBind() const {
	if (is2D) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
const std::string& Texture::getName() const {
	return name;
}
const unsigned int& Texture::getTexMap() const {
	return texMap;
}
const bool Texture::get2D() const {
	return is2D;
}
const glm::ivec2& Texture::getDimentions() const {
	return dimentions;
}