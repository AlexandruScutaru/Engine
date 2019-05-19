#include "BillboardShader.h"



BillboardShader::BillboardShader(){}

BillboardShader::~BillboardShader(){}


void BillboardShader::initShader(const std::string & shader){
	if(m_inited) return;
	init(shader);
	connectTextureUnits();
}

void BillboardShader::loadSelected(bool value){
	loadBool(m_uniforms[SELECTED_U], value);
}

void BillboardShader::loadModelMatrix(glm::mat4 & model){
	loadMat4(m_uniforms[MODEL_MATRIX_U], model);
}

void BillboardShader::loadViewMatrix(glm::mat4 & view){
	loadMat4(m_uniforms[VIEW_MATRIX_U], view);
}

void BillboardShader::loadProjectionMatrix(glm::mat4 & projection){
	loadMat4(m_uniforms[PROJECTION_MATRIX_U], projection);
}

void BillboardShader::getAllUniformLocations(){
	m_uniforms[TEX_DIFF_U] = getUniformLocation("diffuse");
	m_uniforms[SELECTED_U] = getUniformLocation("selected");
	m_uniforms[PROJECTION_MATRIX_U] = getUniformLocation("projection");
	m_uniforms[VIEW_MATRIX_U] = getUniformLocation("view");
	m_uniforms[MODEL_MATRIX_U] = getUniformLocation("model");
}

void BillboardShader::connectTextureUnits(){
	loadInt(m_uniforms[TEX_DIFF_U], 0);
}
