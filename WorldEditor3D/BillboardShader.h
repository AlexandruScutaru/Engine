#ifndef BILLBOARD_SHADER_H
#define BILLBOARD_SHADER_H 

#include <Engine/IShaderProgram.h>


class BillboardShader : public renderer::IShaderProgram{
public:
	BillboardShader();
	~BillboardShader();

	void initShader(const std::string& shader);
	void getAllUniformLocations();

	void loadSelected(bool value);
	void loadModelMatrix(glm::mat4 & model);
	void loadViewMatrix(glm::mat4 & view);
	void loadProjectionMatrix(glm::mat4 & projection);

private:
	void connectTextureUnits();

	enum{
		TEX_DIFF_U,

		SELECTED_U,

		PROJECTION_MATRIX_U,
		VIEW_MATRIX_U,
		MODEL_MATRIX_U,

		NUM_UNIFORMS
	};

	GLint m_uniforms[NUM_UNIFORMS];

};

#endif // GAME_OBJECT_SHADER_H
