#ifndef TERRAIN_SHADER
#define TERRAIN_SHADER


#include <Engine/IShaderProgram.h>


class TerrainShader : public renderer::IShaderProgram {

public:
	TerrainShader();
	~TerrainShader();

	void initShader(const std::string& shader);
	void connectTextureUnits();
	void getAllUniformLocations();
	virtual void loadAdditionalData(int n, va_list args);

	void setFogColor(glm::vec3& color);

	float& getTilingFactorRef(){ return m_tiling_factor; }
	float& getFogDensityRef(){ return m_density; }
	float& getFogGradientRef(){ return m_gradient; }

	glm::vec3 getFogColor(){ return m_fog_color; }

private:
	enum{
		BASE_TEXTURE_U,
		R_TEXTURE_U,
		G_TEXTURE_U,
		B_TEXTURE_U,
		BLEND_TEXTURE_U,
		SHADOW_MAP_U,

		TILING_FACTOR_U,
		FOG_DENSITY_U,
		FOG_GRADIENT_U,
		FOG_COLOR_U,
		FLASHLIGHT_U,

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
		LIGHT_SPACE_U,

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
	float m_tiling_factor;
	float m_density;
	float m_gradient;
	glm::vec3 m_fog_color;

};

#endif // !TERRAIN_SHADER
