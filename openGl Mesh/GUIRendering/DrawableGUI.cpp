#include "DrawableGUI.h"
#include "../Shaders/Shader.h"
#include "../Helpers/Constants.h"
#include <glad/glad.h>

DrawableGUI::DrawableGUI() : _buffers()
{
}

void DrawableGUI::render(const glm::vec2 windowDims)
{
	Shader& shader = SHADERS[NEW_GUI];
	shader.bind();
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (auto& buffer : _buffers) {
		buffer.bind();
		glDrawArrays(GL_TRIANGLES, 0, buffer.size());
	}
	shader.unBind();
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
