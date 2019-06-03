#ifndef LIGHT_DEPTH_SHADER_H
#define LIGHT_DEPTH_SHADER_H

#include <Engine/IShaderProgram.h>


class LightDepthShader : public renderer::IShaderProgram{
public:
	LightDepthShader();
	~LightDepthShader();

	void initShader(const std::string& shader);
	void getAllUniformLocations();
	void connectTextureUnits(){}

	void loadLightSpace(glm::mat4& lightSpace);
	void loadModel(glm::mat4& model);
	
private:
	enum{
		LIGHT_SPACE_U,
		MODEL_U,

		NUM_UNIFORMS
	};

	GLint m_uniforms[NUM_UNIFORMS];

};

#endif // LIGHT_DEPTH_SHADER_H
