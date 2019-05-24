#include "Skybox.h"
#include "ResourceManager.h"

#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace renderer {

	Skybox::Skybox():
		m_enabled(false)
	{
		m_skyboxTextures = std::vector<std::string>(6, "res/textures/black.png");
	}

	Skybox::~Skybox() {
		glDeleteTextures(6, &m_textureID);
	}

	void Skybox::init(float size){
		m_cube = utilities::ResourceManager::getMesh("res/models/cube.obj");
		m_textureID = utilities::ResourceManager::getCubemapTexture(m_skyboxTextures);
		setScale(size);
		m_shader.initShader("res/shaders/skybox");
	}

	void Skybox::render(glm::mat4& view, glm::mat4& projection){
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f));
		model = model * glm::toMat4(glm::quat());
		model = glm::scale(model, m_scale);
		
		m_shader.use();
		
		m_shader.loadProjectionMatrix(projection);
		m_shader.loadViewMatrix(view);
		m_shader.loadModelMatrix(model);

		glBindVertexArray(m_cube->vertexArrayObject);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
		glDrawElements(GL_TRIANGLES, m_cube->indexCount, GL_UNSIGNED_INT, 0);
		
		m_shader.unuse();
	}

	void Skybox::setScale(float scale){
		m_scale = glm::vec3(-scale);
	}

	void Skybox::set(){
		GLuint copy = m_textureID;
		m_textureID = utilities::ResourceManager::getCubemapTexture(m_skyboxTextures);
		glDeleteTextures(6, &copy);
	}

}