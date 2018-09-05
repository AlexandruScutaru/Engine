#ifndef BILLBOARD_SHADER_H
#define BILLBOARD_SHADER_H 

#include <Engine/IShaderProgram.h>


class BillboardShader : public renderer::IShaderProgram{
public:
	BillboardShader();
	~BillboardShader();

	void initShader(const std::string& shader);
	void bindAttributes();
	void getAllUniformLocations();


private:
	enum{
		TEX_DIFF_U,

		PROJECTION_MATRIX_U,
		VIEW_MATRIX_U,
		MODEL_MATRIX_U,

		NUM_UNIFORMS
	};

	GLint m_uniforms[NUM_UNIFORMS];

};

#endif // GAME_OBJECT_SHADER_H
