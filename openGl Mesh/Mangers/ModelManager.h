#pragma once
#include <string>
#include "Manager.h"
#include "../IndexedBuffer.h"


class ModelManager : public Manager<ModelManager>
{
private:
	objl::Loader loader;
public:
	ModelManager();

	IndexedBuffer load(const std::string& file, bool& success);

	void destroy() override;
};

