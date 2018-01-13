#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <string>
#include <GL/glew.h>
#include <vector>

namespace renderer{

	struct TextureData{
		int w;
		int h;
		GLuint id;
	};

	class TextureLoader{
	public:
		static TextureData loadTexture(const std::string& fileName);
		static GLuint loadCubeMapTexture(std::vector<std::string>& fileNames);

	private:
		TextureLoader(){};
	};

	#endif // TEXTURE_LOADER_H

}