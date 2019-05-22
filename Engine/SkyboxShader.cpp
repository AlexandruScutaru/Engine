#include "SkyboxShader.h"


namespace renderer {

	SkyboxShader::SkyboxShader(){}

	SkyboxShader::~SkyboxShader(){}


	void SkyboxShader::initShader(const std::string& shader){
		init(shader);
		connectTextureUnits();
	}

	void SkyboxShader::getAllUniformLocations(){
		m_uniforms[PROJECTION_MATRIX_U] = getUniformLocation("projection");
		m_uniforms[VIEW_MATRIX_U] = getUniformLocation("view");
		m_uniforms[MODEL_MATRIX_U] = getUniformLocation("model");
		m_uniforms[CUBE_MAP_U] = getUniformLocation("cubeMap");
	}

	void SkyboxShader::connectTextureUnits(){
		loadInt(m_uniforms[CUBE_MAP_U], 0);
	}

	void SkyboxShader::loadProjectionMatrix(glm::mat4 & projection){
		loadMat4(m_uniforms[PROJECTION_MATRIX_U], projection);
	}

	void SkyboxShader::loadViewMatrix(glm::mat4 & view){
		auto viewCopy = view;
		viewCopy[3][0] = 0.0f;
		viewCopy[3][1] = 0.0f;
		viewCopy[3][2] = 0.0f;
		loadMat4(m_uniforms[VIEW_MATRIX_U], viewCopy);
	}

	void SkyboxShader::loadModelMatrix(glm::mat4 & model){
		loadMat4(m_uniforms[MODEL_MATRIX_U], model);
	}

}