#pragma once
#include <glm.hpp>
#include <vector>

struct ColourBufferInit {
	unsigned int internalFormat;
	unsigned int format;
	unsigned int type;
};

struct FrameBufferInit {
	std::initializer_list<ColourBufferInit> colourBuffers;
	bool hasDepth;
	bool depthTexture;
	unsigned int depthBuffer;
};

class FrameBuffer
{
private:
	unsigned int FBO;
	glm::ivec2 dimentions;
	std::vector<unsigned int> colourBuffers;
	unsigned int depthBuffer;
public:
	FrameBuffer();
	FrameBuffer(const glm::ivec2& dim);
	~FrameBuffer();
	
	void setUp(const FrameBufferInit& init);

	void cleanUp();

	void bind() const;
	void unBind() const;
	const unsigned int getColourTex(const unsigned int index) const;
};

