#include "UI_Renderer.h"

UI_Renderer::UI_Renderer()
{
}

UI_Renderer::UI_Renderer(Shader* shader) : shader(shader)
{
	setupQuadVAO();
}

UI_Element UI_Renderer::popWhere(std::string name)
{
	UI_Element res;
	res.getName() = "null";
	if (std::find(elements.begin(), elements.end(), name) == elements.end()) {
		return	res;
	}
	res = getWhere(name);
	std::vector<UI_Element>::const_iterator found = std::find(elements.begin(), elements.end(), res);
	if (found != elements.end()) {
		elements.erase(found);  
	}
	return res;
}

void UI_Renderer::appendElement(const UI_Element& element)
{
	elements.push_back(element);
}

void UI_Renderer::prependElement(const UI_Element& element)
{
	elements.insert(elements.begin(), element);
}

UI_Element& UI_Renderer::getWhere(std::string name)
{
	UI_Element null({ 0, 0 }, { 1, 1 }, nullptr, "null");
	for (auto& element : elements) {
		if (element.getName() == name) {
			return element;
		}
	}
	return elements.front();
}

void UI_Renderer::render()
{
	glDisable(GL_DEPTH_TEST);
	shader->bind();
	glBindVertexArray(quadVAO);
	for (UI_Element& element : elements) {
		element.bind(shader);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		element.unBind();
	}
	shader->unBind();
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
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
}
