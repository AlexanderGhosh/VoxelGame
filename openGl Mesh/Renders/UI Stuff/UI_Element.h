#pragma once
#include "../../Game/World/constants.h"
class UI_Element
{
private: // properties
	glm::vec2 pos, size;
	Texture* texture;
public: // functions
	UI_Element();
	UI_Element(glm::vec2 pos, glm::vec2 size, Texture* tex);

	Texture* getTexture();
	void bind(Shader*& shader);
	void unBind();
};

