#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>


namespace renderer{

	ShaderProgram::ShaderProgram(){}

	ShaderProgram::~ShaderProgram(){
		unuse();
		glDeleteProgram(m_program);
	}


	void ShaderProgram::init(const char* vertexPath, const char* fragmentPath, const char* geometryPath){
		//create the shader program
		m_program = glCreateProgram();

		//create and attach the shaders
		GLuint vS, fS, gS;
		vS = createShader(loadShader(vertexPath), GL_VERTEX_SHADER);
		fS = createShader(loadShader(fragmentPath), GL_FRAGMENT_SHADER);
		//add the shaders to the program
		glAttachShader(m_program, vS);
		glAttachShader(m_program, fS);
		//check if we need to also add a geometry shader
		if(geometryPath != nullptr){
			gS = createShader(loadShader(geometryPath), GL_GEOMETRY_SHADER);
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
		checkShaderError(m_program, GL_LINK_STATUS, true, "Error! Program linking failed: \n");

		//validate the shaders
		glValidateProgram(m_program);
		checkShaderError(m_program, GL_VALIDATE_STATUS, true, "Error! Program is invalid: \n");

		//remove the shaders as they are attached to the program
		glDeleteShader(vS);
		glDeleteShader(fS);
		//glDeleteShader(gS);
	}

	void ShaderProgram::use(){
		glUseProgram(m_program);
	}

	void ShaderProgram::unuse(){
		glUseProgram(0);
	}

	void ShaderProgram::loadBool(const std::string& name, bool value){
		glUniform1i(getUniformLocation(name), (int)value);
	}

	void ShaderProgram::loadInt(const std::string& name, int value){
		glUniform1i(getUniformLocation(name), value);
	}

	void ShaderProgram::loadFloat(const std::string& name, float value){
		glUniform1f(getUniformLocation(name), value);
	}

	void ShaderProgram::loadVec2(const std::string& name, glm::vec2& value){
		glUniform2fv(getUniformLocation(name), 1, &value[0]);
	}

	void ShaderProgram::loadVec3(const std::string& name, glm::vec3& value){
		glUniform3fv(getUniformLocation(name), 1, &value[0]);
	}

	void ShaderProgram::loadVec4(const std::string& name, glm::vec4& value){
		glUniform4fv(getUniformLocation(name), 1, &value[0]);
	}

	void ShaderProgram::loadMat2(const std::string& name, glm::mat2& value){
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	void ShaderProgram::loadMat3(const std::string& name, glm::mat3& value){
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	void ShaderProgram::loadMat4(const std::string& name, glm::mat4& value){
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	//helper methods
	GLuint ShaderProgram::getUniformLocation(const std::string& name){
		GLuint location;
		if(-1 == (location = glGetUniformLocation(m_program, name.c_str()))){
			std::cout << "Uniform location: '" << name << "'  error!\n" << std::endl;
			exit(EXIT_FAILURE);
		}
		return location;
	}

	void ShaderProgram::checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string & errorMessage){
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

			std::cout << errorMessage << error << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	std::string ShaderProgram::loadShader(const char* fileName){
		std::string shaderText;
		std::ifstream file;

		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try{
			// open files
			file.open(fileName);
			std::stringstream shaderStream;
			// read file's buffer contents into streams
			shaderStream << file.rdbuf();
			// close file handlers
			file.close();
			// convert stream into string
			shaderText = shaderStream.str();

		} catch(std::ifstream::failure e){
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		return shaderText;
	}

	GLuint ShaderProgram::createShader(const std::string& text, GLuint shaderType){
		GLuint shader = glCreateShader(shaderType);
		if(shader == 0){
			std::cout << "Error: Shader creation error!" << std::endl;
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

		if(shaderType == GL_VERTEX_SHADER)
			checkShaderError(shader, GL_COMPILE_STATUS, false, "Error! Vertex shader compilation failed: \n");
		else if(shaderType == GL_FRAGMENT_SHADER)
			checkShaderError(shader, GL_COMPILE_STATUS, false, "Error! Fragment shader compilation failed: \n");
		else
			checkShaderError(shader, GL_COMPILE_STATUS, false, "Error! Geometry shader compilation failed: \n");

		return shader;
	}

}