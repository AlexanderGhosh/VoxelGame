#include "UI_Renderer.h"

UI_Renderer::UI_Renderer()
{
}

UI_Renderer::UI_Renderer(Shader* shader) : shader(shader)
{
	setupQuadVAO();
}

void UI_Renderer::addElement(const UI_Element& element)
{
	elements.push_back(element);
}

void UI_Renderer::render()
{
	shader->bind();
	glBindVertexArray(quadVAO);
	for (UI_Element& element : elements) {
		element.bind(shader);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		element.unBind();
	}
	shader->unBind();
	glBindVertexArray(0);
}

void UI_Renderer::setupQuadVAO()
{
	
	GLfloat vertices[] = {
		-1.0f/16.0f, -1.0f / 9.0f,  0, 0,
		1.0f / 16.0f, -1.0f / 9.0f, 1, 0,
		-1.0f / 16.0f, 1.0f / 9.0f, 0, 1,
		1.0f / 16.0f, 1.0f / 9.0f,  1, 1
	};
	
	GLuint vbo;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &vbo);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	/*GLfloat vertices[] = {
		0.25f,  0.45f,  1, 1,
		0.25f, -0.45f,  1, 0,
		-0.25f,  0.45f, 0, 1,

		0.25f, -0.45f,  1, 0,
		-0.25f, -0.45f, 0, 0,
		-0.25f,  0.45f, 0, 1
	};
	GLuint vbo;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &vbo);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);*/

}
