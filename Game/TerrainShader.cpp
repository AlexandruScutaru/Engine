#include "TerrainShader.h"

#include <Engine/Lights.h>

#include <stdarg.h>

const unsigned int TerrainShader::MAX_LIGHTS = 4;

TerrainShader::TerrainShader():
	m_fog_color(glm::vec3(1.0f)),
	m_tiling_factor(50.0f),
	m_density(0.0035f),
	m_gradient(5.0f)
{}

TerrainShader::~TerrainShader(){}


void TerrainShader::initShader(const std::string & shader){
	init(shader);
	connectTextureUnits();
}

void TerrainShader::getAllUniformLocations(){
	m_uniforms[BASE_TEXTURE_U] = getUniformLocation("baseTexture");
	m_uniforms[R_TEXTURE_U] = getUniformLocation("rTexture");
	m_uniforms[G_TEXTURE_U] = getUniformLocation("gTexture");
	m_uniforms[B_TEXTURE_U] = getUniformLocation("bTexture");
	m_uniforms[BLEND_TEXTURE_U] = getUniformLocation("blendMap");

	m_uniforms[FOG_COLOR_U] = getUniformLocation("fog_color");
	m_uniforms[TILING_FACTOR_U] = getUniformLocation("tiling_factor");
	m_uniforms[FOG_DENSITY_U] = getUniformLocation("density");
	m_uniforms[FOG_GRADIENT_U] = getUniformLocation("gradient");
	m_uniforms[VIEW_POSITION_U] = getUniformLocation("viewPos");

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

	m_uniforms[PROJECTION_MATRIX_U] = getUniformLocation("projection");
	m_uniforms[VIEW_MATRIX_U] = getUniformLocation("view");

	m_uniforms[POINT_LIGHTS_NUM_U] = getUniformLocation("pointLightsNum");
	PointLightUniforms plUniforms;
	for(unsigned int i = 0; i < MAX_LIGHTS; i++){
		plUniforms.position = getUniformLocation("pointLights[" + std::to_string(i) + "].position");
		plUniforms.ambient = getUniformLocation("pointLights[" + std::to_string(i) + "].ambient");
		plUniforms.diffuse = getUniformLocation("pointLights[" + std::to_string(i) + "].diffuse");
		plUniforms.specular = getUniformLocation("pointLights[" + std::to_string(i) + "].specular");
		plUniforms.att = getUniformLocation("pointLights[" + std::to_string(i) + "].att");
		m_pointLightsLocations.push_back(plUniforms);
	}
}

void TerrainShader::loadAdditionalData(int n, va_list args){
	loadVec3(m_uniforms[FOG_COLOR_U], m_fog_color);
	loadFloat(m_uniforms[TILING_FACTOR_U], m_tiling_factor);
	loadFloat(m_uniforms[FOG_DENSITY_U], m_density);
	loadFloat(m_uniforms[FOG_GRADIENT_U], m_gradient);

	loadBool(m_uniforms[FLASHLIGHT_U], va_arg(args, bool));
	loadVec3(m_uniforms[VIEW_POSITION_U], *(va_arg(args, glm::vec3*)));
	loadMat4(m_uniforms[VIEW_MATRIX_U], *(va_arg(args, glm::mat4*)));
	loadMat4(m_uniforms[PROJECTION_MATRIX_U], *(va_arg(args, glm::mat4*)));

	std::vector<renderer::Light*>* lights = va_arg(args, std::vector<renderer::Light*>*);
	// directional light
	renderer::DirLight* dir = static_cast<renderer::DirLight*>((*lights)[0]);
	loadVec3(m_uniforms[DIR_LIGHT_DIR_U], -dir->direction);
	loadVec3(m_uniforms[DIR_LIGHT_AMB_U], dir->ambient);
	loadVec3(m_uniforms[DIR_LIGHT_DIF_U], dir->diffuse);
	loadVec3(m_uniforms[DIR_LIGHT_SPEC_U], dir->specular);

	if(!((*lights).size() > 1))
		return;

	//spot light
	renderer::SpotLight* spot = static_cast<renderer::SpotLight*>((*lights)[1]);
	loadVec3(m_uniforms[SPOT_LIGHT_AMB_U], spot->ambient);
	loadVec3(m_uniforms[SPOT_LIGHT_DIF_U], spot->diffuse);
	loadVec3(m_uniforms[SPOT_LIGHT_SPEC_U], spot->specular);
	loadVec3(m_uniforms[SPOT_LIGHT_ATT_U], spot->attenuation);
	loadVec3(m_uniforms[SPOT_LIGHT_POS_U], spot->position);
	loadVec3(m_uniforms[SPOT_LIGHT_DIR_U], spot->direction);
	loadFloat(m_uniforms[SPOT_LIGHT_CUT_OFF_U], spot->cutOff);
	loadFloat(m_uniforms[SPOT_LIGHT_OUTER_CUT_OFF_U], spot->outerCutOff);

	//point lights
	size_t num = ((*lights).size() - 2) > MAX_LIGHTS ? MAX_LIGHTS : (*lights).size() - 2;

	loadInt(m_uniforms[POINT_LIGHTS_NUM_U], (int)num);
	for(unsigned int i = 0; i < num; i++){
		renderer::PointLight* point = static_cast<renderer::PointLight*>((*lights)[i + 2]);
		loadVec3(m_pointLightsLocations[i].position, point->position);
		loadVec3(m_pointLightsLocations[i].ambient, point->ambient);
		loadVec3(m_pointLightsLocations[i].diffuse, point->diffuse);
		loadVec3(m_pointLightsLocations[i].specular, point->specular);
		loadVec3(m_pointLightsLocations[i].att, point->attenuation);
	}
}

void TerrainShader::setFogColor(glm::vec3 & color){
	m_fog_color = color;
}

void TerrainShader::connectTextureUnits(){
	loadInt(m_uniforms[BASE_TEXTURE_U], 0);
	loadInt(m_uniforms[R_TEXTURE_U], 1);
	loadInt(m_uniforms[G_TEXTURE_U], 2);
	loadInt(m_uniforms[B_TEXTURE_U], 3);
	loadInt(m_uniforms[BLEND_TEXTURE_U], 4);

}