#include "ModelManager.h"
#include "../IndexedBuffer.h"

ModelManager::ModelManager() : loader()
{
}
IndexedBuffer ModelManager::load(const std::string& file, bool& success)
{
	success = loader.LoadFile(file);

	objl::Mesh mesh = loader.LoadedMeshes.back();
	IndexedBuffer res;
	res.setUp(mesh);
	return res;
}

void ModelManager::destroy()
{
}
