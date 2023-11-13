#include "ModelLoader.h"
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Model.h"

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

Model ModelLoader::Load(const std::string& file)
{
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	aiNode* root = scene->mRootNode;
	std::list<Mesh> meshes;
	processNode(*root, *scene, meshes);
	Model model;
	model.setUp(meshes);
	return model;
}
