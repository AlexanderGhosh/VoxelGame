#pragma once
#include <string>
#include <list>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Model;
class Mesh;
class VoxelModel_Static;

class ModelLoader
{
	friend class ModelManager;
private:
	ModelLoader() = delete;
	static Assimp::Importer importer;
	static Mesh createMesh(const aiMesh& mesh);
	static void processNode(const aiNode& node, const aiScene& scene, std::list<Mesh>& meshes);

	static Model LoadAssimp(const std::string& file);
	// doesnt load colours correctly
	static VoxelModel_Static LoadPointCloud(const std::string& file);
};

