#include "ModelManager.h"
#include "../Helpers/ModelLoaders/ModelLoader.h"

ModelManager::ModelManager()
{
}
Model& ModelManager::load(const std::string& file)
{
	_allModels.push_back(ModelLoader::LoadAssimp(file));
	return _allModels.back();
}

VoxelModel_Static& ModelManager::loadVoxel(const std::string& file)
{
	_allVoxelModels.push_back(std::move(ModelLoader::LoadPointCloud(file)));
	return _allVoxelModels.back();
}

VoxelModel_Static& ModelManager::loadVoxel(const std::string& file, Block block)
{
	_allVoxelModels.push_back(std::move(ModelLoader::LoadPointCloud(file, block)));
	return _allVoxelModels.back();
}

void ModelManager::destroy()
{
	for (Model& m : _allModels) {
		m.cleanUp();
	}
	_allModels.clear();
	// doesnt need to call clean up on _allVoxelsModels becuse the destructor does it
}
