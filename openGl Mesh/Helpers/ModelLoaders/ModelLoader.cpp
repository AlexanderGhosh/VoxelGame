#include "ModelLoader.h"
#include <assimp/postprocess.h>
#include <fstream>
#include <unordered_map>
#include "Mesh.h"
#include "Model.h"
#include "../Functions.h"
#include "../Constants.h"
#include "VoxelModel_Base.h"
#include "../../Material.h"

Assimp::Importer ModelLoader::importer;

Mesh ModelLoader::createMesh(const aiMesh& mesh)
{
	Mesh res;
	res.vertexPoss.resize(mesh.mNumVertices);
	memcpy(res.vertexPoss.data(), mesh.mVertices, mesh.mNumVertices * sizeof(glm::vec3));
	
	res.vertexNorms.resize(mesh.mNumVertices);
	memcpy(res.vertexNorms.data(), mesh.mNormals, mesh.mNumVertices * sizeof(glm::vec3));

	res.vertexTexCoords.resize(mesh.mNumVertices);
	for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
		auto t = mesh.mTextureCoords[0][i];
		res.vertexTexCoords[i] = glm::vec2(t.x, t.y);
	}

	for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
		aiFace face = mesh.mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			res.indices.push_back(face.mIndices[j]);
		}
	}

	return res;
}

void ModelLoader::processNode(const aiNode& node, const aiScene& scene, std::list<Mesh>& meshes)
{
	for (unsigned int i = 0; i < node.mNumMeshes; i++) {
		meshes.push_back(createMesh(*scene.mMeshes[node.mMeshes[i]]));
	}
	for (unsigned int i = 0; i < node.mNumChildren; i++) {
		processNode(*node.mChildren[i], scene, meshes);
	}
}

Model ModelLoader::LoadAssimp(const std::string& file)
{
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	aiNode* root = scene->mRootNode;
	std::list<Mesh> meshes;
	processNode(*root, *scene, meshes);
	Model model;
	model.setUp(meshes);
	return model;
}

VoxelModel_Static ModelLoader::LoadPointCloud(const std::string& fileName, bool withCollider)
{
	std::ifstream file(fileName);
	std::vector<PointColourIndex> points;
	std::unordered_map<glm::vec4, Block> colours;

	std::string line;
	// reads the header
	while (std::getline(file, line) && line != "end_header") {
		if (contains(line, "element")) {
			auto _split = split(line, " ");
			points.reserve(stoi(_split.back()));
		}
	}
	glm::vec3 maxSize(0);
	glm::vec3 minSize(0);

	// the body
	while (std::getline(file, line)) {
		auto _split = split(line, " ");
		if (_split.size() < 6) break;
		glm::vec4 colour(0, 0, 0, 1);
		colour.x = RRC(stof(_split[3]));
		colour.y = RRC(stof(_split[4]));
		colour.z = RRC(stof(_split[5]));

		PointColourIndex point{};
		point.x = stof(_split[0]);
		point.y = stof(_split[2]);
		point.z = stof(_split[1]);
		if (colours.contains(colour)) {
			point.block = colours[colour];
		}
		else {
			Block newBlock;
			newBlock.isTransparent = colour.a < 1;
			newBlock.isDynamic = false;

			Material mat(colour, colour);
			newBlock.materialIndex = MATERIALS.size();

			colours[colour] = newBlock;

			MATERIALS.push_back(mat);
			BLOCKS.push_back(newBlock);
			point.block = newBlock;
		}

		maxSize.x = std::max(maxSize.x, point.x);
		maxSize.y = std::max(maxSize.y, point.y);
		maxSize.z = std::max(maxSize.z, point.z);

		minSize.x = std::min(minSize.x, point.x);
		minSize.y = std::min(minSize.y, point.y);
		minSize.z = std::min(minSize.z, point.z);

		points.push_back(point);
	}
	file.close();

	std::vector<glm::vec3> cols(colours.size());
	int i = 0;
	for (auto& [c, _] : colours)
		cols[i++] = glm::vec3(c);
	return VoxelModel_Static(points, cols, maxSize, minSize, withCollider);
}
