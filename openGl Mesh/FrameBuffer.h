#pragma once
#include <glm.hpp>
#include <vector>

struct ColourBufferInit {
	ColourBufferInit();
	unsigned int internalFormat;
	unsigned int format;
	unsigned int type;
};

struct FrameBufferInit {
	FrameBufferInit();
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

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	FrameBuffer(FrameBuffer&&) noexcept;
	FrameBuffer& operator=(FrameBuffer&&) noexcept;
	
	void setUp(const FrameBufferInit& init);

	void bind() const;
	void unBind() const;

	const unsigned int getDepth() const;
	const unsigned int getColourTex(const unsigned int index) const;
};

