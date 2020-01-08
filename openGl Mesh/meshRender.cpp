#pragma warning (disable : 4996)
#include "meshRender.h"
MeshRender::MeshRender(glm::vec3 pos) :position(pos), shader("block") {

}
void MeshRender::create() {
	mesh.getVertices();
	buffers.loadBuffers();

	/*glGenBuffers(1, &VBO);
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

	glBindVertexArray(0);*/

	//loadTexture(std::string("Textures/grass.png").c_str(), std::string("Textures/grass_specular.png").c_str());
}
void MeshRender::render(Camera p1, glm::mat4 projection) {
	shader.bind();

	glm::mat4 view(1);
	view = p1.GetViewMatrix();

	shader.setValue("view", view);
	shader.setValue("projection", projection);

	glm::vec3 objCol(1, 0.5, 0.31), lightCol(1);
	shader.setValue("objCol", objCol);
	shader.setValue("lightCol", lightCol);


	// texture binding
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texMaps[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texMaps[0]);
	loadTexture("cobblestone");

	glBindVertexArray(VAO);

	glm::mat4 model(1);
	model = glm::translate(model, position);

	shader.setValue("model", model);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);//unbind VAO
}

void MeshRender::loadTexture(std::string name) {
	std::string diffuse = "Textures/" + name + "_diff.png";
	std::string specular = "Textures/" + name + "_spec.png";
	glm::ivec2 dim;
	glGenTextures(1, &texMaps[0]);
	glGenTextures(1, &texMaps[1]);
	// diffuse
	unsigned char* image = SOIL_load_image(diffuse.c_str(), &dim.x, &dim.y, 0, SOIL_LOAD_RGBA);
	glBindTexture(GL_TEXTURE_2D, texMaps[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.x, dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// specular
	image = SOIL_load_image(specular.c_str(), &dim.x, &dim.y, 0, SOIL_LOAD_RGBA);
	glBindTexture(GL_TEXTURE_2D, texMaps[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.x, dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	shader.bind();
	glm::vec2 mat(0, 1);
	shader.setValue("mat_diff", mat.x);
	// shader.setValue("mat_spec", mat.y);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshRender::loadMesh(BlockMesh& m) {
	mesh = m;
}
void MeshRender::destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}