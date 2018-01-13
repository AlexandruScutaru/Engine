#ifndef	SHADER_PROGRAM_H
#define	SHADER_PROGRAM_H

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include <string>
#include <vector>

namespace renderer{

	class ShaderProgram{
	public:
		ShaderProgram();
		~ShaderProgram();

		//void init(const std::string& fileName);
		void init(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
		void use();
		void unuse();

		void loadBool(const std::string& name, bool value);
		void loadInt(const std::string& name, int value);
		void loadFloat(const std::string& name, float value);
		void loadVec2(const std::string& name, glm::vec2& value);
		void loadVec3(const std::string& name, glm::vec3& value);
		void loadVec4(const std::string& name, glm::vec4& value);
		void loadMat2(const std::string& name, glm::mat2& value);
		void loadMat3(const std::string& name, glm::mat3& value);
		void loadMat4(const std::string& name, glm::mat4& value);

	private:
		GLuint getUniformLocation(const std::string& name);
		void checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
		std::string loadShader(const char* fileName);
		GLuint createShader(const std::string& text, GLuint shaderType);

		//OpenGL's way of keeping track of where the shader program is 
		GLuint m_program;

	};

}

#endif // SHADER_PROGRAM_H