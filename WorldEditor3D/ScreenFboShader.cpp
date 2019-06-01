#include "ScreenFboShader.h"



ScreenFboShader::ScreenFboShader(){}

ScreenFboShader::~ScreenFboShader(){}


void ScreenFboShader::initShader(const std::string& shader){
	init(shader);
	connectTextureUnits();
}

void ScreenFboShader::getAllUniformLocations(){
	m_uniforms[SCREEN_TEXTURE_U] = getUniformLocation("screenTexture");
	m_uniforms[STRENGTH_U] = getUniformLocation("strength");
	m_uniforms[HEIGHT_U] = getUniformLocation("height");
	m_uniforms[ASPECT_U] = getUniformLocation("aspectRatio");
	m_uniforms[CYLINDRIAL_RATIO_U] = getUniformLocation("cylindricalRatio");
}

void ScreenFboShader::connectTextureUnits(){
	loadInt(m_uniforms[SCREEN_TEXTURE_U], 0);
}

void ScreenFboShader::loadStrength(float strength){
	loadFloat(m_uniforms[STRENGTH_U], strength);
}

void ScreenFboShader::loadHeight(float height){
	loadFloat(m_uniforms[HEIGHT_U], height);
}

void ScreenFboShader::loadAspectRatio(float aspect){
	loadFloat(m_uniforms[ASPECT_U], aspect);
}

void ScreenFboShader::loadCylindricalRatio(float cylRatio){
	loadFloat(m_uniforms[CYLINDRIAL_RATIO_U], cylRatio);
}
