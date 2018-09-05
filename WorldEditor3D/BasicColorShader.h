#ifndef BASIC_SHADER_H
#define BASIC_SHADER_H 

#include <Engine/IShaderProgram.h>


class BasicColorShader : public renderer::IShaderProgram{
public:
	BasicColorShader();
	~BasicColorShader();

	void initShader(const std::string& shader);
	void getAllUniformLocations();

	void loadColor(glm::vec3& color);
	void loadModelMatrix(glm::mat4& model);
	void loadViewMatrix(glm::mat4& view);
	void loadProjectionMatrix(glm::mat4& projection);

private:
	enum{
		COLOR_U,

		PROJECTION_MATRIX_U,
		VIEW_MATRIX_U,
		MODEL_MATRIX_U,

		NUM_UNIFORMS
	};

	GLint m_uniforms[NUM_UNIFORMS];

};

#endif // GAME_OBJECT_SHADER_H
