#include "TextureLoader.h"
#include "Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <iostream>


namespace utilities{

	renderer::TextureData TextureLoader::loadTexture(const std::string& fileName, bool flip){
		stbi_set_flip_vertically_on_load(flip); //due to how opengl reads and maps texels

		renderer::TextureData data;
		int numComponents;

		//loads an image file from hdd
		//parameters:
			//1st: string containing path to the file
			//2nd: pointer to the image width
			//3rd: pointer to the image height
			//4th: pointer to the number of components
			//5th: required components 
		unsigned char* imageData = stbi_load(fileName.c_str(), &data.w, &data.h, &numComponents, 4);

		if(imageData == nullptr){
			LOG_E_ERROR_TRACEABLE("stbi_load:: failed loading texture '{}'", fileName);
			exit(EXIT_FAILURE);
		}

		//tell OpenGL to generate space for the texture
		//paramenters:
			//1st: number of textures
			//2nd: pointer to the texture handle
		glGenTextures(1, &data.id);

		//bind the texture
		//target flag specifies what type of texture we want - basic 2D texture
		glBindTexture(GL_TEXTURE_2D, data.id);


		//parameters:
			//1st: type of the texture
			//2nd: name of the parameter to be set
			//3rd: parameter value
				//these parameters tell OpenGL that when we access a pixel at coords(x, y) 
				//outside the actual texture the pixel OpenGL reads is the one at the 
				//coords(textureWidth % x, textureHeight % y)
				//if those are not set to repeat, the pixel returned will have a specified color
		///glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		///glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//these determine how to handle interpolation or extrapolation of the pixels
		///glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		///glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//parameters
			//1st: flag telling OpenGl how to interpret the texture data
			//2nd: the level: part of the technique called mip mapping
				//that means that the a higher resolution texture is used closer to the camera
				//and lower resolution when the object is further from the camera
				//0 is the default level
			//3rd: the internal format: how to store individual pixels on GPU
			//4th: width of the texture
			//5th: height of the texture
			//6th: the border: not much info
			//7th: the input format sent to the GPU
			//8th: the texture data type - how are the pixels stored(series of integers, bytes)
			//9th: the pointer to the actual pixel data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.w, data.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

		stbi_image_free(imageData);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

		return data;
	}

	GLuint TextureLoader::loadCubeMapTexture(std::vector<std::string>& fileNames){
		stbi_set_flip_vertically_on_load(true); //due to how opengl reads and maps texels

		GLuint textureID;

		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		for(size_t i = 0; i < fileNames.size(); i++){
			int width;
			int height;
			int numComponents;

			unsigned char* imageData = stbi_load(fileNames[i].c_str(), &width, &height, &numComponents, 4);
			if(imageData == nullptr){
				LOG_E_ERROR_TRACEABLE("stbi_load:: failed loading texture '{}'", fileNames[i]);
				exit(EXIT_FAILURE);
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		return textureID;
	}

}