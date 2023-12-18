#pragma once
#include "Manager.h"
#include <string>
#include "../Helpers/ModelLoaders/Model.h"
#include "../Helpers/ModelLoaders/VoxelModel_Base.h"

class ModelManager : public Manager<ModelManager>
{
private:
	std::list<Model> _allModels;
	std::list<VoxelModel_Static> _allVoxelModels;
public:
	ModelManager();

	Model& load(const std::string& file);
	VoxelModel_Static& loadVoxel(const std::string& file);

	void destroy() override;
};

