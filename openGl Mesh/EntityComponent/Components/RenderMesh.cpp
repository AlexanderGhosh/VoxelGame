#include "RenderMesh.h"
#include "../../Helpers/Constants.h"
#include "../../Shaders/Shader.h"
#include "../../Helpers/ModelLoaders/Model.h"


Shader* Components::RenderMesh::_defaultShader = &SHADERS[MODEL];

void Components::RenderMesh::bindShader() const
{
	if (_overrideShader) {
		_overrideShader->bind();
	}
	else {
		_defaultShader->bind();
	}
}

Components::RenderMesh::RenderMesh() : _model(), _overrideShader()
{
}

void Components::RenderMesh::render()
{
	bindShader();
	_model->render();
}

void Components::RenderMesh::setShader(Shader* shader)
{
	_overrideShader = shader;
}
