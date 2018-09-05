#include "GameObjectShader.h"
#include <Engine/Lights.h>
#include <iostream>

const unsigned int GameObjectShader::MAX_LIGHTS = 4;

GameObjectShader::GameObjectShader(){}

GameObjectShader::~GameObjectShader(){}


void GameObjectShader::initShader(const std::string& shader){
	init(shader);
	connectTextureUnits();
	loadShininess(32.0f);
}

void GameObjectShader::connectTextureUnits(){
	loadInt(m_uniforms[MAT_TEX_DIFF_U], 0);
	loadInt(m_uniforms[MAT_TEX_SPEC_U], 1);
	
	//loadInt(getUniformLocation("material.diffuse"), 0);
	//loadInt(getUniformLocation("material.specular"), 1);
}

void GameObjectShader::loadShininess(float value){
	loadFloat(m_uniforms[MAT_SHININESS_U], value);
	//loadFloat(getUniformLocation("material.shininess"), value);

}

void GameObjectShader::loadFlashlight(bool value){
	loadBool(m_uniforms[FLASHLIGHT_U], value);
	//loadBool(getUniformLocation("flashlightOn"), value);
}

void GameObjectShader::loadSelected(bool value){
	loadBool(m_uniforms[SELECTED_U], value);
	//loadBool(getUniformLocation("selected"), value);
}

void GameObjectShader::loadModelMatrix(glm::mat4 & model){
	loadMat4(m_uniforms[MODEL_MATRIX_U], model);
	//loadMat4(getUniformLocation("model"), model);
}

void GameObjectShader::loadViewMatrix(glm::mat4 & view){
	loadMat4(m_uniforms[VIEW_MATRIX_U], view);
	//loadMat4(getUniformLocation("view"), view);
}

void GameObjectShader::loadProjectionMatrix(glm::mat4 & projection){
	loadMat4(m_uniforms[PROJECTION_MATRIX_U], projection);
	//loadMat4(getUniformLocation("projection"), projection);

}

void GameObjectShader::loadLights(std::vector<renderer::Light*>& lights){
	// directional light
	renderer::DirLight* dir = static_cast<renderer::DirLight*>(lights[0]);
	loadVec3(m_uniforms[DIR_LIGHT_DIR_U], -dir->direction);
	loadVec3(m_uniforms[DIR_LIGHT_AMB_U], dir->ambient);
	loadVec3(m_uniforms[DIR_LIGHT_DIF_U], dir->diffuse);
	loadVec3(m_uniforms[DIR_LIGHT_SPEC_U], dir->specular);

	if(!(lights.size() > 1))
		return;

	//spot light
	renderer::SpotLight* spot = static_cast<renderer::SpotLight*>(lights[1]);
	loadVec3( m_uniforms[SPOT_LIGHT_AMB_U], spot->ambient);
	loadVec3( m_uniforms[SPOT_LIGHT_DIF_U], spot->diffuse);
	loadVec3( m_uniforms[SPOT_LIGHT_SPEC_U], spot->specular);
	loadVec3( m_uniforms[SPOT_LIGHT_ATT_U], spot->attenuation);
	loadVec3( m_uniforms[SPOT_LIGHT_POS_U], spot->position);
	loadVec3( m_uniforms[SPOT_LIGHT_DIR_U], spot->direction);
	loadFloat(m_uniforms[SPOT_LIGHT_CUT_OFF_U], spot->cutOff);
	loadFloat(m_uniforms[SPOT_LIGHT_OUTER_CUT_OFF_U], spot->outerCutOff);

	//point lights
	size_t num = (lights.size() - 2) > MAX_LIGHTS ? MAX_LIGHTS : lights.size() - 2;
	loadInt(POINT_LIGHTS_NUM_U, (int)num);
	
	for(unsigned int i = 0; i < num; i++){
		renderer::PointLight* point = static_cast<renderer::PointLight*>(lights[i + 2]);
		loadVec3(m_pointLightsLocations[i].position, point->position);
		loadVec3(m_pointLightsLocations[i].ambient, point->ambient);
		loadVec3(m_pointLightsLocations[i].diffuse, point->diffuse);
		loadVec3(m_pointLightsLocations[i].specular, point->specular);
		loadVec3(m_pointLightsLocations[i].att, point->attenuation);
	}
}

void GameObjectShader::loadViewPosition(glm::vec3 & viewPos){
	loadVec3(m_uniforms[VIEW_POSITION_U], viewPos);
	//loadVec3(getUniformLocation("viewPos"), viewPos);
}

void GameObjectShader::getAllUniformLocations(){
	m_uniforms[VIEW_POSITION_U] = getUniformLocation("viewPos");
	
	m_uniforms[MAT_TEX_DIFF_U] = getUniformLocation("material.diffuse");
	m_uniforms[MAT_TEX_SPEC_U] = getUniformLocation("material.specular");
	m_uniforms[MAT_SHININESS_U] = getUniformLocation("material.shininess");
	
	m_uniforms[DIR_LIGHT_DIR_U] = getUniformLocation("dirLight.direction");
	m_uniforms[DIR_LIGHT_AMB_U] = getUniformLocation("dirLight.ambient");
	m_uniforms[DIR_LIGHT_DIF_U] = getUniformLocation("dirLight.diffuse");
	m_uniforms[DIR_LIGHT_SPEC_U] = getUniformLocation("dirLight.specular");
	
	m_uniforms[SPOT_LIGHT_DIR_U] = getUniformLocation("spotLight.direction");
	m_uniforms[SPOT_LIGHT_AMB_U] = getUniformLocation("spotLight.ambient");
	m_uniforms[SPOT_LIGHT_DIF_U] = getUniformLocation("spotLight.diffuse");
	m_uniforms[SPOT_LIGHT_SPEC_U] = getUniformLocation("spotLight.specular");
	m_uniforms[SPOT_LIGHT_ATT_U] = getUniformLocation("spotLight.att");
	m_uniforms[SPOT_LIGHT_POS_U] = getUniformLocation("spotLight.position");
	m_uniforms[SPOT_LIGHT_CUT_OFF_U] = getUniformLocation("spotLight.cutOff");
	m_uniforms[SPOT_LIGHT_OUTER_CUT_OFF_U] = getUniformLocation("spotLight.outerCutOff");
	
	m_uniforms[FLASHLIGHT_U] = getUniformLocation("flashlightOn");
	
	m_uniforms[SELECTED_U] = getUniformLocation("selected");
	
	m_uniforms[PROJECTION_MATRIX_U] = getUniformLocation("projection");
	m_uniforms[VIEW_MATRIX_U] = getUniformLocation("view");
	m_uniforms[MODEL_MATRIX_U] = getUniformLocation("model");
	
	m_uniforms[POINT_LIGHTS_NUM_U] = getUniformLocation("pointLightsNum");
	PointLightUniforms plUniforms;
	for(int i = 0; i < MAX_LIGHTS; i++){
		plUniforms.position = getUniformLocation("pointLights[" + std::to_string(i) + "].position");
		plUniforms.ambient  = getUniformLocation("pointLights[" + std::to_string(i) + "].ambient");
		plUniforms.diffuse  = getUniformLocation("pointLights[" + std::to_string(i) + "].diffuse");
		plUniforms.specular = getUniformLocation("pointLights[" + std::to_string(i) + "].specular");
		plUniforms.att		= getUniformLocation("pointLights[" + std::to_string(i) + "].att");
		m_pointLightsLocations.push_back(plUniforms);
	}
}
