#include "Texture.h"
Texture::Texture(std::string name, GLboolean is2D) {
	this->is2D = is2D;
	if (is2D) {
		created = load2D(name);
	}
	else {
		created = load3D(name);
	}
}
GLboolean Texture::load2D(std::string& name) {
	std::string diffuse = "Textures/" + name + "_diff.png";
	std::string specular = "Textures/" + name + "_spec.png";
	glm::ivec2 dim;
	glGenTextures(1, &texMap);
	glGenTextures(1, &texMap);
	// diffuse
	unsigned char* image = SOIL_load_image(diffuse.c_str(), &dim.x, &dim.y, 0, SOIL_LOAD_RGBA);
	glBindTexture(GL_TEXTURE_2D, texMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.x, dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// specular
	image = SOIL_load_image(specular.c_str(), &dim.x, &dim.y, 0, SOIL_LOAD_RGBA);
	glBindTexture(GL_TEXTURE_2D, texMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.x, dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	/*
	shader.bind();
	glm::vec2 mat(0, 1);
	shader.setValue("mat_diff", mat.x);*/
	return GL_TRUE;
}
GLboolean Texture::load3D(std::string& name) {
	std::vector<std::string> faces = {
		"Textures/" + name + "/left.png",	// left
		"Textures/" + name + "/front.png",	// front
		"Textures/" + name + "/bottom.png", // bottom
		"Textures/" + name + "/top.png",	// top
		"Textures/" + name + "/right.png",	// right
		"Textures/" + name + "/back.png"	// back
	};
	glGenTextures(1, &texMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);

	unsigned char* data;

	for (GLuint i = 0; i < faces.size(); i++)
	{
		data = SOIL_load_image(faces[i].c_str(), &dimentions.x, &dimentions.y, 0, SOIL_LOAD_RGBA);
		if (data)
		{

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, dimentions.x, dimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			SOIL_free_image_data(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	/*
	shader.bind();
	glm::vec2 mat(0, 1);
	shader.setValue("mat_diff", mat.x);
	int t = 0;
	shader.setValue("skybox", t);*/
	return GL_TRUE;
}
void Texture::bind() {
	if (is2D) {
		glBindTexture(GL_TEXTURE_2D, texMap);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);
	}
}