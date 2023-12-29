#include "DrawableGUI.h"
#include "../Shaders/Shader.h"
#include "../Helpers/Constants.h"
#include <glad/glad.h>

DrawableGUI::DrawableGUI() : _buffers()
{
}

void DrawableGUI::render()
{
	Shader& shader = SHADERS[NEW_GUI];
	shader.bind();
	for (auto& buffer : _buffers) {
		buffer.bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, buffer.size());
	}
	shader.unBind();
}

void DrawableGUI::add(GUIBuffer& buffer)
{
	_buffers.push_front(buffer);
	buffer.empty();
}

void DrawableGUI::clear()
{
	_buffers.clear();
}
