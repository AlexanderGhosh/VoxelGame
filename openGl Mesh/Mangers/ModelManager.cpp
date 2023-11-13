#include "ModelManager.h"
#include "../Helpers/ModelLoaders/ModelLoader.h"
#include "../Helpers/ModelLoaders/Model.h"

ModelManager::ModelManager()
{
}
Model ModelManager::load(const std::string& file)
{
	return ModelLoader::Load(file);
}

void ModelManager::destroy()
{
}
