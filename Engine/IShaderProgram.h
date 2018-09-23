#ifndef	I_SHADER_PROGRAM_H
#define	I_SHADER_PROGRAM_H

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include <string>
#include <vector>

namespace renderer{

	class IShaderProgram{
	public:
		IShaderProgram();
		virtual ~IShaderProgram();

		void init(const std::string& fileName, bool hasGeometry = false);
		void use();
		void unuse();

	protected:
		GLint getUniformLocation(const std::string& uniformName);
		virtual void getAllUniformLocations() = 0;
		
		void loadBool (GLint location, bool value);
		void loadInt  (GLint location, int value);
		void loadFloat(GLint location, float value);
		void loadVec2 (GLint location, glm::vec2& value);
		void loadVec3 (GLint location, glm::vec3& value);
		void loadVec4 (GLint location, glm::vec4& value);
		void loadMat2 (GLint location, glm::mat2& value);
		void loadMat3 (GLint location, glm::mat3& value);
		void loadMat4 (GLint location, glm::mat4& value);

	private:
		void checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
		std::string loadShader(const std::string& fileName);
		GLuint createShader(const std::string& text, GLuint shaderType);

		//OpenGL's way of keeping track of where the shader program is 
		GLuint m_program;

	};

}

#endif // I_SHADER_PROGRAM_H