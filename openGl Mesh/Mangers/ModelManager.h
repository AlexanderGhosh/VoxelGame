#pragma once
#include <string>
#include "Manager.h"

class Model;

class ModelManager : public Manager<ModelManager>
{
public:
	ModelManager();

	Model load(const std::string& file);

	void destroy() override;
};

