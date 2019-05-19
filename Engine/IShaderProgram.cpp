#include "IShaderProgram.h"
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>


namespace renderer{

	IShaderProgram::IShaderProgram() :
		m_inited(false)
	{}

	IShaderProgram::~IShaderProgram(){
		unuse();
		glDeleteProgram(m_program);
	}


	void IShaderProgram::init(const std::string& fileName, bool hasGeometry){
		//create the shader program
		m_program = glCreateProgram();

		//create and attach the shaders
		GLuint vS=0, gS=0, fS=0;
		vS = createShader(loadShader(fileName + ".vs"), GL_VERTEX_SHADER);
		fS = createShader(loadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);
		
		//add the shaders to the program
		glAttachShader(m_program, vS);
		glAttachShader(m_program, fS);

		//check if we need to also add a geometry shader
		if(hasGeometry){
			gS = createShader(loadShader(fileName + ".gs"), GL_GEOMETRY_SHADER);
			glAttachShader(m_program, gS);
		}

		//link the shaders
		glLinkProgram(m_program);
		//check for errors
		//parameters
			//1st: the program or the shader we are cheking for errors on
			//2nd: flag, GL_LINK_STATUS or GL_VALIDATE_STATUS
			//3rd: boolean value telling if we want to check error on a program or individual shader
				//true => we need error cheking on program
				//false => we need error cheking on individual shader
			//4th: message to be concatenated with the error
		checkShaderError(m_program, GL_LINK_STATUS, true, "error! shader program link failed:\n");
		//validate
		glValidateProgram(m_program);
		checkShaderError(m_program, GL_VALIDATE_STATUS, true, "error! shader program validate failed:\n");

		//remove the shaders as they are attached to the program
		glDeleteShader(vS);
		glDeleteShader(fS);
		if(hasGeometry) glDeleteShader(gS);

		use();
		getAllUniformLocations();
		m_inited = true;
	}

	void IShaderProgram::use(){
		glUseProgram(m_program);
	}

	void IShaderProgram::unuse(){
		glUseProgram(0);
	}

	void IShaderProgram::loadBool(GLint location, bool value){
		glUniform1i(location, (int)value);
	}

	void IShaderProgram::loadInt(GLint location, int value){
		glUniform1i(location, value);
	}

	void IShaderProgram::loadFloat(GLint location, float value){
		glUniform1f(location, value);
	}

	void IShaderProgram::loadVec2(GLint location, glm::vec2& value){
		glUniform2fv(location, 1, &value[0]);
	}

	void IShaderProgram::loadVec3(GLint location, glm::vec3& value){
		glUniform3fv(location, 1, &value[0]);
	}

	void IShaderProgram::loadVec4(GLint location, glm::vec4& value){
		glUniform4fv(location, 1, &value[0]);
	}

	void IShaderProgram::loadMat2(GLint location, glm::mat2& value){
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}

	void IShaderProgram::loadMat3(GLint location, glm::mat3& value){
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}

	void IShaderProgram::loadMat4(GLint location, glm::mat4& value){
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}

	//helper methods
	GLint IShaderProgram::getUniformLocation(const std::string& name){
		GLint location = -1;
		if(-1 == (location = glGetUniformLocation(m_program, name.c_str()))){
			LOG_E_ERROR_TRACEABLE("glGetUniformLocation '{}' error!", name);
			exit(EXIT_FAILURE);
		}
		return location;
	}

	void IShaderProgram::checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage){
		GLint success = 0;
		GLchar error[1024] = {0};

		if(isProgram)
			glGetProgramiv(shader, flag, &success);
		else
			glGetShaderiv(shader, flag, &success);

		if(success == GL_FALSE){
			if(isProgram)
				glGetProgramInfoLog(shader, sizeof(error), nullptr, error);
			else
				glGetShaderInfoLog(shader, sizeof(error), nullptr, error);

			LOG_E_ERROR_TRACEABLE("{} {}", errorMessage, error);
			exit(EXIT_FAILURE);
		}
	}

	std::string IShaderProgram::loadShader(const std::string& fileName){
		std::string shaderText;
		std::ifstream file;

		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try{
			// open files
			file.open(fileName);
			std::stringstream shaderStream;
			// read file's buffer contents into stream
			shaderStream << file.rdbuf();
			// close file handlers
			file.close();
			// convert stream into string
			shaderText = shaderStream.str();
		} catch(std::ifstream::failure e){
			LOG_E_ERROR_TRACEABLE("couldn't read file '{}'", fileName);
			exit(EXIT_FAILURE);
		}

		return shaderText;
	}

	GLuint IShaderProgram::createShader(const std::string& text, GLuint shaderType){
		GLuint shader = glCreateShader(shaderType);
		std::string shaderName = "";
		if(shader == 0){
			switch(shaderType){
				case GL_VERTEX_SHADER:
					shaderName = "vertex";
					break;
				case GL_GEOMETRY_SHADER:
					shaderName = "geometry";
					break;
				case GL_FRAGMENT_SHADER:
					shaderName = "fragment";
					break;
				default:
					shaderName = "unknown";
					break;
			}
			LOG_E_ERROR_TRACEABLE("glCreateShader '{}' error", shaderName);
			exit(EXIT_FAILURE);
		}

		//convert c++ string type to a list of one C string
		const GLchar* shaderSourceStrings[1];
		GLint shaderSourceStringsLengths[1];

		shaderSourceStrings[0] = text.c_str();
		shaderSourceStringsLengths[0] = text.length();

		//get OpenGL to compile the shader
		//send shader source code to OpenGL
		//the way this method is implemented is based on the GPU
		//parameters:
			//1st: the shader
			//2nd: the number of different source strings
			//3rd: the actual source strings list
			//4th: the list of lengths
		glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringsLengths);
		glCompileShader(shader);

		switch(shaderType){
		case GL_VERTEX_SHADER:
			shaderName = "vertex";
			break;
		case GL_GEOMETRY_SHADER:
			shaderName = "geometry";
			break;
		case GL_FRAGMENT_SHADER:
			shaderName = "fragment";
			break;
		default:
			shaderName = "unknown";
			break;
		}
		checkShaderError(shader, GL_COMPILE_STATUS, false, "error! " + shaderName + " shader compilation failed:\n");

		return shader;
	}

}