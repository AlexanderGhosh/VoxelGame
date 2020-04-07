#include "UI_Element.h"

UI_Element::UI_Element() : pos(0), size(1), texture(nullptr)
{
}

UI_Element::UI_Element(glm::vec2 pos, glm::vec2 size, Texture* tex) : pos(pos), size(size), texture(tex)
{
}

Texture* UI_Element::getTexture()
{
	return texture;
}

void UI_Element::bind(Shader*& shader)
{
	texture->bind();
	shader->setValue("texture0", 0);
	shader->setValue("modelPos", pos);
	shader->setValue("scale", size);
}

void UI_Element::unBind()
{
	texture->unBind();
}
