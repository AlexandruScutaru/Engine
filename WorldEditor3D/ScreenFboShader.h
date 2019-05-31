#ifndef SCREEN_FBO_SHADER_H
#define SCREEN_FBO_SHADER_H 

#include <Engine/IShaderProgram.h>


class ScreenFboShader : public renderer::IShaderProgram{
public:
	ScreenFboShader();
	~ScreenFboShader();

	void initShader(const std::string& shader);
	void getAllUniformLocations();
	void connectTextureUnits();

private:
	enum{
		SCREEN_TEXTURE_U,

		NUM_UNIFORMS
	};

	GLint m_uniforms[NUM_UNIFORMS];

};

#endif // SCREEN_FBO_SHADER_H
