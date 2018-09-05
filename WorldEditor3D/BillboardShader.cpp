#include "BillboardShader.h"



BillboardShader::BillboardShader(){}

BillboardShader::~BillboardShader(){}


void BillboardShader::initShader(const std::string & shader){
	init(shader);
}

void BillboardShader::bindAttributes(){}

void BillboardShader::getAllUniformLocations(){
	m_uniforms[TEX_DIFF_U] = getUniformLocation("diffuse");
	m_uniforms[PROJECTION_MATRIX_U] = getUniformLocation("projection");
	m_uniforms[VIEW_MATRIX_U] = getUniformLocation("view");
	m_uniforms[MODEL_MATRIX_U] = getUniformLocation("model");
}
