#ifndef RENDERER_H
#define RENDERER_H

#include <GLM/glm.hpp>

namespace renderer{

	class TexturedModel;

	class Renderer{
	public:
		static void Init();
		static void SetBackgroundColor(glm::vec4& color);
		static void BindTexturedModel(TexturedModel* model);
		static void DrawTexturedModel(TexturedModel* model);
		static glm::u8vec4 getColorAt(glm::vec2& coords);
		static void updateProjectionMatrix(float fov, int width, int height);
		static glm::mat4& GetProjectionMatrix(){ return m_projection; }

	private:
		Renderer();
		static glm::mat4 m_projection;

	};

}
#endif // !RENDERER_H

