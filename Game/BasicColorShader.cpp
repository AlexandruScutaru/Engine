#include "BasicColorShader.h"



BasicColorShader::BasicColorShader() :
	renderer::IShaderProgram()
{}

BasicColorShader::~BasicColorShader(){}


void BasicColorShader::initShader(const std::string & shader){
	init(shader);
}

void BasicColorShader::getAllUniformLocations(){
	m_uniforms[COLOR_U] = getUniformLocation("color");
	m_uniforms[PROJECTION_MATRIX_U] = getUniformLocation("projection");
	m_uniforms[VIEW_MATRIX_U] = getUniformLocation("view");
	m_uniforms[MODEL_MATRIX_U] = getUniformLocation("model");
}

void BasicColorShader::loadColor(glm::vec3 & color){
	loadVec3(m_uniforms[COLOR_U], color);
}

void BasicColorShader::loadModelMatrix(glm::mat4 & model){
	loadMat4(m_uniforms[MODEL_MATRIX_U], model);
}

void BasicColorShader::loadViewMatrix(glm::mat4 & view){
	loadMat4(m_uniforms[VIEW_MATRIX_U], view);
}

void BasicColorShader::loadProjectionMatrix(glm::mat4 & projection){
	loadMat4(m_uniforms[PROJECTION_MATRIX_U], projection);
}
