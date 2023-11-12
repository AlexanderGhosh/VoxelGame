#pragma once
#include <list>

class IndexedBuffer;
class Shader;

class ModelRenderer
{
private:
	std::list<IndexedBuffer> buffers;
public:
	ModelRenderer();
	~ModelRenderer();

	void add(IndexedBuffer& buffer);
	
	void render(Shader* shader);

	void cleanUp();
};

