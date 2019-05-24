#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "ModelDataStructs.h"
#include <GL/glew.h>

#include <string>
#include <vector>


namespace utilities{

	class TextureLoader{
	public:
		static renderer::TextureData loadTexture(const std::string& fileName, bool flip);
		static GLuint loadCubeMapTexture(std::vector<std::string>& fileNames);

	private:
		TextureLoader(){}
	};

}

#endif // TEXTURE_LOADER_H