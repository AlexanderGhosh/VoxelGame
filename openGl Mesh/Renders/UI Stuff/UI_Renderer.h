#pragma once
#include "UI_Element.h"
class UI_Renderer
{
private: // properties
	unsigned int quadVAO;
	Shader* shader;
	std::vector<UI_Element> elements;
public: // functions
	UI_Renderer();
	UI_Renderer(Shader* shader);

	UI_Element popWhere(std::string name);
	void appendElement(const UI_Element& element);
	void prependElement(const UI_Element& element);
	UI_Element& getWhere(std::string name);


	void render();

private: // functions
	void setupQuadVAO();
};

