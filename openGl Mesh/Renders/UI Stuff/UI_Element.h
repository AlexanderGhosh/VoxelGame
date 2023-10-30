#pragma once
#include <glm.hpp>
#include <string>

class Texture;
class Shader;
class UI_Element
{
private: // properties
	glm::vec2 pos, size;
	Texture* texture;
	std::string name;
public: // functions
	UI_Element();
	UI_Element(glm::vec2 pos, glm::vec2 size, Texture* tex, std::string name);

	void bind(Shader*& shader);
	void unBind();

	std::string& getName();
	Texture* getTexture();
	glm::vec2& getPos();
	glm::vec2& getSize();

	bool operator==(const UI_Element& element);
	bool operator==(const std::string name);
};

