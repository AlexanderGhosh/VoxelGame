#include "FrameBuffer.h"
#include <glad/glad.h>
#include <iostream>

FrameBuffer::FrameBuffer() : FBO(), dimentions(), depthBuffer()
{
}

FrameBuffer::FrameBuffer(const glm::ivec2& dim) : FrameBuffer()
{
	dimentions = dim;
}

FrameBuffer::~FrameBuffer()
{
	cleanUp();
}

void FrameBuffer::setUp(const FrameBufferInit& init)
{
	glGenFramebuffers(1, &FBO); 
	bind();
	
	colourBuffers.resize(init.colourBuffers.size());
	std::vector<unsigned int> colourAttachments(colourBuffers.size());
	unsigned int i = 0;
	for (const ColourBufferInit& colourInit : init.colourBuffers) {
		unsigned int& tex = colourBuffers[i];
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, colourInit.internalFormat, dimentions.x, dimentions.y, 0, colourInit.type, colourInit.format, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex, 0);
		colourAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
		i++;
	}

	if (init.hasDepth && init.depthTexture) {
		if (init.depthBuffer == 0) {
			glGenTextures(1, &depthBuffer);
			glBindTexture(GL_TEXTURE_2D, depthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, dimentions.x, dimentions.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
		else {
			depthBuffer = init.depthBuffer;
			glBindTexture(GL_TEXTURE_2D, depthBuffer);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: framebuffer is not complete!" << std::endl;

	if (colourAttachments.size()) {
		glDrawBuffers(colourAttachments.size(), colourAttachments.data());
	}
	else {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	unBind();
}

void FrameBuffer::cleanUp()
{
	if (FBO == 0) return;
	glDeleteFramebuffers(1, &FBO);
	FBO = 0;
}

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, dimentions.x, dimentions.y);
}

void FrameBuffer::unBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const unsigned int FrameBuffer::getDepth() const
{
	return depthBuffer;
}

const unsigned int FrameBuffer::getColourTex(const unsigned int index) const
{
	return colourBuffers[index];
}

ColourBufferInit::ColourBufferInit() : internalFormat(0), format(0), type(0)
{
}

FrameBufferInit::FrameBufferInit() : colourBuffers(), depthBuffer(0), depthTexture(), hasDepth()
{
}
