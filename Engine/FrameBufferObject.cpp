#include "FrameBufferObject.h"
#include "Logger.h"
#include "Window.h"

namespace renderer{

	FrameBufferObject::FrameBufferObject(){}

	FrameBufferObject::~FrameBufferObject(){
		freeData();
	}


	void FrameBufferObject::init(int w, int h){
		m_texW = w;
		m_texH = h;
		
		initFrameBuffer();
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
			LOG_E_ERROR_TRACEABLE("OpenGL frame buffer object creation error: '{}'!", glGetError());
			exit(EXIT_FAILURE);
		} else{
			LOG_E_INFO("OpenGL frame buffer object creation succeeded!");
		}
	}

	void FrameBufferObject::bind(){
		//To make sure the texture isn't bound
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		glViewport(0, 0, m_texW, m_texH);
	}

	void FrameBufferObject::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Window::getW(), Window::getH());
	}

	void FrameBufferObject::reset(int w, int h){
		freeData();
		init(w, h);
	}

	void FrameBufferObject::freeData(){
		glDeleteFramebuffers(1, &m_frameBuffer);
		glDeleteTextures(1, &m_texture);
		glDeleteRenderbuffers(1, &m_depthBuffer);
		glDeleteTextures(1, &m_depthTexture);
	}

	void FrameBufferObject::initFrameBuffer(){
		m_frameBuffer = createFrameBuffer();
		m_texture = createTextureAttachment();
		m_depthBuffer = createDepthBufferAttachment();
		m_depthTexture = createDepthTextureAttachment();
		unbind();
	}

	GLuint FrameBufferObject::createFrameBuffer(){
		GLuint frameBuffer;
		//generate 1 frame buffer
		glGenFramebuffers(1, &frameBuffer);
		//create the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		//indicate that we will always render to colour attachment 0
		//glDrawBuffer(GL_COLOR_ATTACHMENT0);
		return frameBuffer;
	}

	GLuint FrameBufferObject::createTextureAttachment(){
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texW, m_texH, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		return texture;
	}

	GLuint FrameBufferObject::createDepthBufferAttachment(){
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_texW, m_texH);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		return depthBuffer;
	}

	GLuint FrameBufferObject::createDepthTextureAttachment(){
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_texW, m_texH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
		return texture;
	}

}