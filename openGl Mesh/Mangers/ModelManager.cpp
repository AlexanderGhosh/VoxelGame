#include "ModelManager.h"
#include "../Helpers/ModelLoaders/ModelLoader.h"

ModelManager::ModelManager()
{
}
Model& ModelManager::load(const std::string& file)
{
	_allModels.push_back(ModelLoader::Load(file));
	return _allModels.back();
}

void ModelManager::destroy()
{
	for (Model& m : _allModels) {
		m.cleanUp();
	}
	_allModels.clear();
}
