#ifndef SKYBOX
#define SKYBOX

#include "SkyboxShader.h"
#include "ModelDataStructs.h"

namespace renderer {
	class Skybox{
	public:
		Skybox();
		~Skybox();
		Skybox(const Skybox& other) = delete;
		Skybox& operator=(const Skybox& other) = delete;

		void init(float size);
		void render(glm::mat4& view, glm::mat4& projection);
		void setScale(float scale);

		enum : unsigned char{
			RIGHT,
			LEFT,
			BOTTOM,
			TOP,
			BACK,
			FRONT
		};

		void set();

		void setSkyboxTexturePath(unsigned char index, const std::string& path) { m_skyboxTextures[index] = path; }
		void setSkyboxTexturePaths(std::vector<std::string>& texturePaths){ m_skyboxTextures = texturePaths; }
		void setEnabled(bool val) { m_enabled = val; }

		std::string getSkyboxTexturePath(unsigned char index) {return m_skyboxTextures[index]; }
		std::vector<std::string> getSkyboxTexturePaths(){ return m_skyboxTextures; }
		bool enabled(){ return m_enabled; }
		bool& getEnabledRef(){ return m_enabled; }

	private:
		bool m_enabled;

		glm::vec3 m_scale;
		MeshData* m_cube;
		GLuint m_textureID;
		SkyboxShader m_shader;

		std::vector<std::string> m_skyboxTextures;

	};

}

#endif //SKYBOX

