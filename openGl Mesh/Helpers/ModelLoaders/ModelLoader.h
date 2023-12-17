#pragma once
#include <string>
#include <list>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Model;
class Mesh;
class ModelLoader
{
private:
	ModelLoader();
	static Assimp::Importer importer;
	static Mesh createMesh(const aiMesh& mesh);
	static void processNode(const aiNode& node, const aiScene& scene, std::list<Mesh>& meshes);
public:
	static Model LoadAssimp(const std::string& file);
	static void LoadPointCloud(const std::string& file);
};

