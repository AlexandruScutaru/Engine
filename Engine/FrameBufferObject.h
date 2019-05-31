#ifndef FRAME_BUFFER_OBJECT_H
#define FRAME_BUFFER_OBJECT_H

#include <GL/glew.h>


namespace renderer{

	class FrameBufferObject{
	public:
		FrameBufferObject();
		~FrameBufferObject();

		void init(int w, int h);
		void reset(int w, int h);
		void bind(); //call before rendering to this FBO
		void unbind(); //call to switch to default frame buffer

		GLuint getTexture(){ return m_texture; }
		GLuint getDepthTexture(){ return m_depthTexture; }

	private:
		void freeData();
		void initFrameBuffer();

		GLuint createFrameBuffer();
		GLuint createTextureAttachment();
		GLuint createDepthStencilBufferAttachment();
		GLuint createDepthTextureAttachment();

		GLuint m_frameBuffer;
		GLuint m_texture;
		GLuint m_depthStencilBuffer;
		GLuint m_depthTexture;

		int m_texW;
		int m_texH;
	};
}

#endif
