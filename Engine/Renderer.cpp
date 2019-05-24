#include "Renderer.h"
#include "TexturedModel.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>


namespace renderer{

	glm::mat4 Renderer::m_projection = glm::mat4();
	
	void Renderer::Init(){
		//now the object are rendered the farthest to closest
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void Renderer::SetBackgroundColor(glm::vec4 & color){
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void renderer::Renderer::BindTexturedModel(TexturedModel* model){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model->getMaterial().getDiffuseMap()->id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, model->getMaterial().getSpecularMap()->id);
		glBindVertexArray(model->getMesh()->vertexArrayObject);
	}

	void Renderer::DrawTexturedModel(TexturedModel* model){
		glDrawElements(GL_TRIANGLES, model->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
	}

	glm::u8vec4 Renderer::getColorAt(glm::vec2 & coords){
		unsigned char color[4];
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glReadPixels((GLuint)coords.x, (GLuint)(viewport[3] - coords.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);

		int index = color[0] +
			color[1] * 256 +
			color[2] * 256 * 256;

		return glm::u8vec4(color[0], color[1], color[2], color[3]);
	}

	void Renderer::updateProjectionMatrix(float fov, int width, int height){
		m_projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 400.0f);
	}

	void Renderer::enableBackFaceCulling(){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void Renderer::disableBackFaceCulling(){
		glDisable(GL_CULL_FACE);
	}

}