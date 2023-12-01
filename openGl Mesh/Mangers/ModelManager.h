#pragma once
#include <string>
#include "Manager.h"

class Model;

class ModelManager : public Manager<ModelManager>
{
private:
	std::list<Model> _allModels;
public:
	ModelManager();

	Model& load(const std::string& file);

	void destroy() override;
};

