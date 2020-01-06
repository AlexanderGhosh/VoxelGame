#pragma warning (disable : 4996)
#include "meshRender.h"
MeshRender::MeshRender(glm::vec3 pos) :position(pos), shader("block") {

}
void MeshRender::create() {
	std::vector<GLfloat> vertices = mesh.getVertices();
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data() , GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal atributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// texture attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	//loadTexture(std::string("Textures/grass.png").c_str(), std::string("Textures/grass_specular.png").c_str());
}
void MeshRender::render(Camera p1, glm::mat4 projection) {
	shader.bind();

	glm::mat4 view(1);
	view = p1.GetViewMatrix();

	GLuint modelLoc = shader.getLocation("model");
	GLuint viewLoc = shader.getLocation("view");
	GLuint projLoc = shader.getLocation("projection");

	shader.setLocation(viewLoc, view);
	shader.setLocation(projLoc, projection);

	// texture binding
	/*
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texMaps[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texMaps[0]);*/


	glBindVertexArray(VAO);

	glm::mat4 model(1);
	model = glm::translate(model, position);

	shader.setLocation(modelLoc, model);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);//unbind VAO
}

void MeshRender::loadTexture(std::string diffuse, std::string specular) {
	return;
	diffuse = "Shaders/" + diffuse + ".png";
	specular = "Shaders/" + specular + ".png";
	int textureWidth, textureHeight;
	glGenTextures(1, &texMaps[0]);
	glGenTextures(1, &texMaps[1]);
	//glGenTextures(1, &this->emissionMap);
	// diffuse
	unsigned char* image = SOIL_load_image(diffuse.c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
	glBindTexture(GL_TEXTURE_2D, texMaps[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// specular
	image = SOIL_load_image(specular.c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
	glBindTexture(GL_TEXTURE_2D, texMaps[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	/*this->shader.Bind();
	glUniform1i(glGetUniformLocation(this->shader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(this->shader.Program, "material.specular"), 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void MeshRender::loadMesh(BlockMesh& m) {
	mesh = m;
}
void MeshRender::destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}