#include "LightDepthShader.h"


LightDepthShader::LightDepthShader(){}

LightDepthShader::~LightDepthShader(){}


void LightDepthShader::initShader(const std::string& shader){
	init(shader);
	connectTextureUnits();
}

void LightDepthShader::getAllUniformLocations(){
	m_uniforms[LIGHT_SPACE_U] = getUniformLocation("lightSpace");
	m_uniforms[MODEL_U] = getUniformLocation("model");
}

void LightDepthShader::loadLightSpace(glm::mat4 & lightSpace){
	loadMat4(m_uniforms[LIGHT_SPACE_U], lightSpace);
}

void LightDepthShader::loadModel(glm::mat4 & model){
	loadMat4(m_uniforms[MODEL_U], model);
}
