#ifndef GAME_OBJECT_SHADER_H
#define GAME_OBJECT_SHADER_H 

#include <Engine/IShaderProgram.h>
#include <vector>

namespace renderer{
	struct Light;
}

class GameObjectShader : public renderer::IShaderProgram{
public:
	GameObjectShader();
	~GameObjectShader();

	void initShader(const std::string& shader);
	void getAllUniformLocations();

	void connectTextureUnits();
	void loadShininess(float value);
	void loadFlashlight(bool value);
	void loadSelected(bool value);
	void loadAtlasSize(int size);
	void loadAtlasOffset(glm::vec2& offset);
	void loadModelMatrix(glm::mat4& model);
	void loadViewMatrix(glm::mat4& view);
	void loadProjectionMatrix(glm::mat4& projection);
	void loadMvpMatrix(glm::mat4& mvp);
	void loadLights(std::vector<renderer::Light*>& lights);
	void loadViewPosition(glm::vec3& viewPos);
	
private:
	enum{
		MAT_TEX_DIFF_U,
		MAT_TEX_SPEC_U,
		MAT_SHININESS_U,
		
		FLASHLIGHT_U,
		SELECTED_U,
		ATLAS_SIZE_U,
		ATLAS_OFFSET_U,

		DIR_LIGHT_DIR_U,
		DIR_LIGHT_AMB_U,
		DIR_LIGHT_DIF_U,
		DIR_LIGHT_SPEC_U,

		SPOT_LIGHT_DIR_U,
		SPOT_LIGHT_AMB_U,
		SPOT_LIGHT_DIF_U,
		SPOT_LIGHT_SPEC_U,
		SPOT_LIGHT_ATT_U,
		SPOT_LIGHT_POS_U,
		SPOT_LIGHT_CUT_OFF_U,
		SPOT_LIGHT_OUTER_CUT_OFF_U,
		
		POINT_LIGHTS_NUM_U,
		
		VIEW_POSITION_U,
		
		PROJECTION_MATRIX_U,
		VIEW_MATRIX_U,
		MODEL_MATRIX_U,
		MVP_MATRIX_U,

		NUM_UNIFORMS
	};

	struct PointLightUniforms {
		GLint position;
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint att;
	};

	GLint m_uniforms[NUM_UNIFORMS];
	std::vector<PointLightUniforms> m_pointLightsLocations;
	static const unsigned int MAX_LIGHTS;

};

#endif // GAME_OBJECT_SHADER_H
