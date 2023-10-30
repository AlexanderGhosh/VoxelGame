#include "UI_Element.h"
#include <gtc/matrix_transform.hpp>
#include "../../Textures/Texture.h"
#include "../../Shaders/Shader.h"


UI_Element::UI_Element() : pos(0), size(1), texture(nullptr)
{
}

UI_Element::UI_Element(glm::vec2 pos, glm::vec2 size, Texture* tex, std::string name) : pos(pos), size(size), texture(tex), name(name)
{
}

Texture* UI_Element::getTexture()
{
	return texture;
}

glm::vec2& UI_Element::getPos()
{
	return pos;
}

glm::vec2& UI_Element::getSize()
{
	return size;
}

void UI_Element::bind(Shader*& shader) 
{
	texture->bind();
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(pos, 0));
	model = glm::scale(model, glm::vec3(size, 0));
	shader->setValue("model", model);
}

void UI_Element::unBind()
{
	texture->unBind();
}

std::string& UI_Element::getName()
{
	return name;
}

bool UI_Element::operator==(const UI_Element& element)
{
	return element.name == name;
}

bool UI_Element::operator==(const std::string name)
{
	return this->name == name;
}
