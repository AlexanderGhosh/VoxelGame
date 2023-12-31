#pragma once
#include "Manager.h"
#include <string>
#include "../Helpers/ModelLoaders/Model.h"
#include "../Helpers/ModelLoaders/VoxelModel.h"

class ModelManager : public Manager<ModelManager>
{
private:
	std::list<Model> _allModels;
	std::list<VoxelModel> _allVoxelModels;
public:
	ModelManager();

	Model& load(const std::string& file);
	VoxelModel& loadVoxel(const std::string& file);
	VoxelModel& loadVoxel(const std::string& file, Block block);

	void destroy() override;
};

