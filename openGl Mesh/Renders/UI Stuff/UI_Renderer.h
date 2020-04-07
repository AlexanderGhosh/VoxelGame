#pragma once
#include "UI_Element.h"
class UI_Renderer
{
private: // properties
	GLuint quadVAO;
	Shader* shader;
	std::vector<UI_Element> elements;
public: // functions
	UI_Renderer();
	UI_Renderer(Shader* shader);

	void addElement(const UI_Element& element);

	void render();

private: // functions
	void setupQuadVAO();
};

