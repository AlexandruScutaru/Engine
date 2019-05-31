#include "ScreenFboShader.h"



ScreenFboShader::ScreenFboShader(){}

ScreenFboShader::~ScreenFboShader(){}


void ScreenFboShader::initShader(const std::string& shader){
	init(shader);
	connectTextureUnits();
}

void ScreenFboShader::getAllUniformLocations(){
	m_uniforms[SCREEN_TEXTURE_U] = getUniformLocation("screenTexture");
}

void ScreenFboShader::connectTextureUnits(){
	loadInt(m_uniforms[SCREEN_TEXTURE_U], 0);
}
