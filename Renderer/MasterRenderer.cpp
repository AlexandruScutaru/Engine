#include "MasterRenderer.h"
#include "Window.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>


namespace renderer{
	
	MasterRenderer::MasterRenderer(){
	}

	MasterRenderer::~MasterRenderer(){}

	void MasterRenderer::init(){
		// build and compile the shader programs
		m_entityShader.init("shaders/entity.vs", "shaders/entity.fs");
		m_billBoardShader.init("shaders/billboard.vs", "shaders/billboard.fs");
		m_basicShader.init("shaders/basic.vs", "shaders/basic.fs");

		// shader configuration
		m_entityShader.use();
		m_entityShader.loadInt("material.diffuse", 0);
		m_entityShader.loadInt("material.specular", 1);
		m_entityShader.loadFloat("material.shininess", 32.0f);
	
		m_billBoardShader.use();
		m_billBoardShader.loadInt("diffuse", 0);

		//now the object are rendered the farthest to closest
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0);
	}

	void MasterRenderer::renderScene(std::vector<Entity*>& entities, std::vector<BillBoard*>& billboards, DirLight& dirLight, std::vector<PointLight>& pointLights, SpotLight& spotLight, Camera& camera){
		for(auto entity : entities)
			processObject(entity);
		for(auto billboard : billboards)
			processObject(billboard);
		
		setUniforms(dirLight, pointLights, spotLight, camera);
		render();
	}

	void MasterRenderer::setProjectionMatrix(Camera& camera){
		m_projection = glm::perspective(glm::radians(camera.getFOV()), (float)Window::getW() / (float)Window::getH(), 0.1f, 100.0f);
	}

	void MasterRenderer::renderSingleEntity(TexturedModel* object, DirLight& sun, Camera& camera){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(!object)
			return;
		m_entityShader.use();
		// directional light
		m_entityShader.loadVec3("dirLight.direction", -sun.direction);
		m_entityShader.loadVec3("dirLight.ambient", sun.ambient);
		m_entityShader.loadVec3("dirLight.diffuse", sun.diffuse);
		m_entityShader.loadVec3("dirLight.specular", sun.specular);
		//point lights
		m_entityShader.loadInt("pointLightsNum", 0);
		//spot light
		m_entityShader.loadBool("flashlightOn", true);

		//positional info
		m_entityShader.loadVec3("viewPos", camera.getPos());
		// view/projection transformations
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 model;
		m_entityShader.loadMat4("projection", m_projection);
		m_entityShader.loadMat4("view", view);
		m_entityShader.loadMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object->getMaterial().getDiffuseMap()->id);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, object->getMaterial().getSpecularMap()->id);
		glBindVertexArray(object->getMesh()->vertexArrayObject);
		
		glDrawElements(GL_TRIANGLES, object->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
	}

	void MasterRenderer::renderBoundingBox(TexturedModel* object, glm::vec3 & scale, glm::vec3 & rot, Camera & camera){
		if(!object)
			return;
		// view/projection transformations
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f));
		
		model = glm::rotate(model, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));

		model = glm::scale(model, scale);

		m_basicShader.use();
		m_basicShader.loadMat4("projection", m_projection);
		m_basicShader.loadMat4("view", view);
		m_basicShader.loadMat4("model", model);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(object->getMesh()->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, object->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void MasterRenderer::processObject(Entity* entity){
		TexturedModel* model = entity->getTexturedModel();
		std::vector<Entity*> batch = m_entityBatches[model];
		if(!batch.empty()){
			batch.push_back(entity);
			m_entityBatches[model] = batch;
		} else{
			std::vector<Entity*> newBatch;
			newBatch.push_back(entity);
			m_entityBatches[model] = newBatch;
		}
	}

	void MasterRenderer::processObject(BillBoard* billboard){
		TexturedModel* model = billboard->getTexturedModel();
		std::vector<BillBoard*> batch = m_billboardBatches[model];
		if(!batch.empty()){
			batch.push_back(billboard);
			m_billboardBatches[model] = batch;
		} else{
			std::vector<BillBoard*> newBatch;
			newBatch.push_back(billboard);
			m_billboardBatches[model] = newBatch;
		}
	}

	void MasterRenderer::render(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderEntities();
		renderBillBoard();
	}

	void MasterRenderer::renderEntities(){
		glm::mat4 modelMatrix;
		m_entityShader.use();
		for(auto const& model : m_entityBatches){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model.first->getMaterial().getDiffuseMap()->id);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, model.first->getMaterial().getSpecularMap()->id);
			glBindVertexArray(model.first->getMesh()->vertexArrayObject);

			for(auto const& entity : model.second){
				modelMatrix = glm::translate(modelMatrix, entity->getPosition());
				m_entityShader.loadMat4("model", modelMatrix);
				glDrawElements(GL_TRIANGLES, model.first->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
			}
		}
		m_entityBatches.clear();
	}

	void MasterRenderer::renderBillBoard(){
		m_billBoardShader.use();
		m_billBoardShader.loadMat4("projection", m_projection);
		m_billBoardShader.loadMat4("view", m_view);

		glm::mat4 modelMatrix;

		for(auto const& model : m_billboardBatches){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model.first->getMaterial().getDiffuseMap()->id);
			glBindVertexArray(model.first->getMesh()->vertexArrayObject);

			for(auto const& billboard : model.second){
				modelMatrix = glm::translate(modelMatrix, billboard->getPosition());
				modelMatrix[0][0] = m_view[0][0];
				modelMatrix[0][1] = m_view[1][0];
				modelMatrix[0][2] = m_view[2][0];
				modelMatrix[1][0] = m_view[0][1];
				modelMatrix[1][1] = m_view[1][1];
				modelMatrix[1][2] = m_view[2][1];
				modelMatrix[2][0] = m_view[0][2];
				modelMatrix[2][1] = m_view[1][2];
				modelMatrix[2][2] = m_view[2][2];
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
				m_billBoardShader.loadMat4("model", modelMatrix);
				glDrawElements(GL_TRIANGLES, model.first->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
			}
		}
		m_billboardBatches.clear();
	}

	void MasterRenderer::setUniforms(DirLight& dirLight, std::vector<PointLight>& pointLights, SpotLight& spotLight, Camera& camera){
		///entity shader
		m_entityShader.use();
		// directional light
		m_entityShader.loadVec3("dirLight.direction", -dirLight.direction);
		m_entityShader.loadVec3("dirLight.ambient", dirLight.ambient);
		m_entityShader.loadVec3("dirLight.diffuse", dirLight.diffuse);
		m_entityShader.loadVec3("dirLight.specular", dirLight.specular);
		//point lights
		int numLights = pointLights.size() > 4 ? 4 : pointLights.size();
		m_entityShader.loadInt("pointLightsNum", numLights);
		for(size_t i = 0; i < pointLights.size(); i++){
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i].position);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].ambient",  pointLights[i].ambient);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].diffuse",  pointLights[i].diffuse);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i].specular);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].att",      pointLights[i].attenuation);
		}
		//spot light
		m_entityShader.loadBool("flashlightOn", true);
		m_entityShader.loadVec3("spotLight.ambient", spotLight.ambient);
		m_entityShader.loadVec3("spotLight.diffuse", spotLight.diffuse);
		m_entityShader.loadVec3("spotLight.specular", spotLight.specular);
		m_entityShader.loadVec3("spotLight.att",	  spotLight.attenuation);
		m_entityShader.loadVec3("spotLight.position", spotLight.position);
		m_entityShader.loadVec3("spotLight.direction", spotLight.direction);
		m_entityShader.loadFloat("spotLight.cutOff", spotLight.cutOff);
		m_entityShader.loadFloat("spotLight.outerCutOff", spotLight.outerCutOff);

		//positional info
		m_entityShader.loadVec3("viewPos", camera.getPos());
		// view/projection transformations
		m_view = camera.getViewMatrix();
		m_entityShader.loadMat4("projection", m_projection);
		m_entityShader.loadMat4("view", m_view);
	}
}