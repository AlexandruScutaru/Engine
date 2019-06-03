#include "Renderer.h"
#include "TexturedModel.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

namespace renderer{

	glm::mat4 Renderer::m_projection = glm::mat4();
	glm::mat4 Renderer::m_lightSpace = glm::mat4();
	glm::vec4 Renderer::m_bgColor = glm::vec4();

	void Renderer::Init(){
		//now the object are rendered the farthest to closest
		EnableDepthTest();
		EnableBackFaceCulling();
		m_bgColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
	}

	void Renderer::SetBackgroundColor(glm::vec4& color){
		glClearColor(color.r, color.g, color.b, color.a);
		m_bgColor = color;
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

	void Renderer::UpdateLightSpaceMatrix(glm::vec3 & lightPos){
		glm::mat4 lightProjection = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, 0.1f, 400.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_lightSpace = lightProjection * lightView;
	}

	void Renderer::EnableDepthTest(){
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::DisableDepthTest(){
		glDisable(GL_DEPTH_TEST);
	}

	void Renderer::EnableWireframeMode(){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void Renderer::DisableWireframeMode(){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glm::mat4& Renderer::GetLightSpaceMatrix(){
		return m_lightSpace;
	}

	void Renderer::EnableBackFaceCulling(){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void Renderer::DisableBackFaceCulling(){
		glDisable(GL_CULL_FACE);
	}

}