#include "RenderMesh.h"
#include "../../Helpers/Constants.h"
#include "../../Shaders/Shader.h"
#include "../../Helpers/ModelLoaders/Model.h"
#include "Transform.h"


Shader* Components::RenderMesh::_defaultShader = &SHADERS[MODEL];

Components::RenderMesh::RenderMesh() : _model(), _overrideShader(), _transform()
{
}

Shader* Components::RenderMesh::getShader() const
{
	if (_overrideShader) {
		return _overrideShader;
	}
	else {
		return _defaultShader;
	}
}

void Components::RenderMesh::render()
{
	if (!_model) {
		return;
	}
	Shader* shader = getShader();
	shader->bind();
	shader->setValue("model", _transform->getModel());
	_model->render();
}

void Components::RenderMesh::setShader(Shader* shader)
{
	_overrideShader = shader;
}

void Components::RenderMesh::setTransform(Transform* transform)
{
	_transform = transform;
}

void Components::RenderMesh::setModel(Model* model)
{
	_model = model;
}
