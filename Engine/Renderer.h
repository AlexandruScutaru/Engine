#ifndef RENDERER_H
#define RENDERER_H

#include <GLM/glm.hpp>

namespace renderer{

	class TexturedModel;

	class Renderer{
	public:
		static void Init();

		static void SetBackgroundColor(glm::vec4& color);
		static glm::vec4 GetBackgroundColor(){ return m_bgColor; }

		static void BindTexturedModel(TexturedModel* model);
		static void DrawTexturedModel(TexturedModel* model);
		static glm::u8vec4 getColorAt(glm::vec2& coords);
		
		static void updateProjectionMatrix(float fov, int width, int height);
		static glm::mat4& GetProjectionMatrix(){ return m_projection; }

		static void UpdateLightSpaceMatrix(glm::vec3& lightPos);
		static glm::mat4& GetLightSpaceMatrix();
		
		static void EnableBackFaceCulling();
		static void DisableBackFaceCulling();

		static void EnableDepthTest();
		static void DisableDepthTest();

		static void EnableWireframeMode();
		static void DisableWireframeMode();

	private:
		Renderer();
		static glm::mat4 m_projection;
		static glm::vec4 m_bgColor;
		static glm::mat4 m_lightSpace;

	};

}
#endif // !RENDERER_H

