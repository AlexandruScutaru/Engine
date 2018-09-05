#ifndef MODEL_DATA_STRUCTS_H
#define MODEL_DATA_STRUCTS_H

#include <GL/glew.h>

namespace renderer{

	//enum for all attribute lists used by the VAO
	enum vbos{
		POSITION_VB,
		TEXCOORDS_VB,
		NORMAL_VB,
		INDEX_VB, //vertex index buffer is used to reuse vertices to draw other triangles
		NUM_BUFFERS
	};

	//OpenGL reffers to data on GPU theough buffers (blocks of data)	
	//Modern OpenGL way of referring to mesh data on gpu	
	//struct containing handles for the VAO and every VBO in it
	//aswell as the number of indices (how much of the VAO to draw)
	struct MeshData{
		GLuint vertexArrayObject;
		GLuint vertexArrayBuffers[NUM_BUFFERS];
		int indexCount;
	};

	struct TextureData{
		int w;
		int h;
		GLuint id;
	};

}

#endif