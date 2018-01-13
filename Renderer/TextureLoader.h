#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <string>
#include <GL/glew.h>
#include <vector>
#include "ModelDataStructs.h"

namespace renderer{

	class TextureLoader{
	public:
		static TextureData loadTexture(const std::string& fileName);
		static GLuint loadCubeMapTexture(std::vector<std::string>& fileNames);

	private:
		TextureLoader(){}
	};

}

#endif // TEXTURE_LOADER_H