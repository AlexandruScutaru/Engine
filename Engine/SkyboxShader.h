#ifndef SKYBOX_SHADER
#define SKYBOX_SHADER

#include "IShaderProgram.h"

namespace renderer {

	class SkyboxShader : public IShaderProgram{
	public:
		SkyboxShader();
		~SkyboxShader();

		void initShader(const std::string& shader);
		void getAllUniformLocations();

		void connectTextureUnits();
		void loadProjectionMatrix(glm::mat4& projection);
		void loadViewMatrix(glm::mat4& view);
		void loadModelMatrix(glm::mat4& model);

	private:
		enum{
			PROJECTION_MATRIX_U,
			VIEW_MATRIX_U,
			MODEL_MATRIX_U,
			CUBE_MAP_U,

			NUM_UNIFORMS
		};

		GLuint m_uniforms[NUM_UNIFORMS];
	
	};

}

#endif // !SKYBOX_SHADER
