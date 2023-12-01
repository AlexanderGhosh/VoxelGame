#pragma once
#include "Manager.h"
#include <string>
#include "../Helpers/ModelLoaders/Model.h"

class ModelManager : public Manager<ModelManager>
{
private:
	std::list<Model> _allModels;
public:
	ModelManager();

	Model& load(const std::string& file);

	void destroy() override;
};

