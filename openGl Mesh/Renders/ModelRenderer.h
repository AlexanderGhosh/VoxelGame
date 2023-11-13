#pragma once
#include <list>

class Model;
class Shader;

class ModelRenderer
{
private:
	std::list<Model> models;
public:
	ModelRenderer();
	~ModelRenderer();

	void add(Model& buffer);
	
	void render(Shader* shader);

	void cleanUp();
};

