#ifndef HUD_H
#define HUD_H

#include "IShaderProgram.h"

#include <GLM/glm.hpp>


namespace renderer{
	struct TextureData;

	struct HudElement{
		TextureData* texture;
		glm::vec3 pos;
		glm::vec3 scale;
		bool enabled;
	};

	class HUD : public IShaderProgram{
	public:
		HUD();
		~HUD();

		void initHUD();
		void render(std::vector<HudElement>& elements);
		void render(int num, glm::vec3& pos, glm::vec3& scale);

	private:
		void loadModelMatrix(glm::mat4& model);
		void connectTextureUnits();
		void getAllUniformLocations();
		
		enum{
			MODEL_MATRIX_U,	
			TEXTURE_U,
			NUM_UNIFORMS
		};

		GLuint m_uniforms[NUM_UNIFORMS];
		std::vector<HudElement> m_digits;
	};

}

#endif // !HUD_H
