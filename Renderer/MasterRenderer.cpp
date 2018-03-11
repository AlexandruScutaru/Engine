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
		m_selectShader.init("shaders/selection.vs", "shaders/selection.fs");


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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//glClearColor(0.1f, 0.1f, 0.1f, 1.0);
	}

	void MasterRenderer::renderScene(std::vector<GameObject*>& gameObjects, std::vector<Light*>& lights, Camera& camera){
		for(auto gameObject : gameObjects){
			processObject(gameObject);
		}
		setUniforms(lights, camera);
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
		m_entityShader.loadBool("flashlightOn", false);

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

	int MasterRenderer::pixelPick(std::vector<GameObject*> objects, Camera & camera, glm::vec2& coords){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::vec3 colorCode;
		m_view = camera.getViewMatrix();
		m_selectShader.use();
		m_selectShader.loadMat4("projection", m_projection);
		m_selectShader.loadMat4("view", m_view);
		for(auto object : objects){
			glm::mat4 model;
			if(object->isBillboard()){
				model = glm::translate(model, object->getPosition());
				model[0][0] = m_view[0][0];
				model[0][1] = m_view[1][0];
				model[0][2] = m_view[2][0];
				model[1][0] = m_view[0][1];
				model[1][1] = m_view[1][1];
				model[1][2] = m_view[2][1];
				model[2][0] = m_view[0][2];
				model[2][1] = m_view[1][2];
				model[2][2] = m_view[2][2];
				model = glm::scale(model, glm::vec3(0.2f));
			} else{
				model = glm::translate(model, object->getPosition());
				model = glm::rotate(model, object->getRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, object->getRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, object->getRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::scale( model, object->getScale());
			}

			m_selectShader.loadMat4("model", model);
			int code = object->getCode();
			colorCode = glm::vec3(
				((code & 0x000000FF) >> 0) / 255.0f,
				((code & 0x0000FF00) >> 8) / 255.0f, 
				((code & 0x00FF0000) >> 16) / 255.0f
			);
			m_selectShader.loadVec3("colorCode", colorCode);
			glBindVertexArray(object->getTexturedModel()->getMesh()->vertexArrayObject);
			glDrawElements(GL_TRIANGLES, object->getTexturedModel()->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
		}

		
		unsigned char color[4];
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glReadPixels(coords.x, viewport[3] - coords.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);


		int index = color[0] +
					color[1] * 256 +
					color[2] * 256 * 256;

		return index;
	}

	void MasterRenderer::processObject(GameObject* gameObject){
		if(gameObject->isBillboard())
			processBillboard(gameObject);
		else{
			TexturedModel* model = gameObject->getTexturedModel();
			std::vector<GameObject*> batch = m_gameObjectsBatches[model];
			if(!batch.empty()){
				batch.push_back(gameObject);
				m_gameObjectsBatches[model] = batch;
			} else{
				std::vector<GameObject*> newBatch;
				newBatch.push_back(gameObject);
				m_gameObjectsBatches[model] = newBatch;
			}
		}
	}

	void MasterRenderer::processBillboard(GameObject* billboard){
		TexturedModel* model = billboard->getTexturedModel();
		std::vector<GameObject*> batch = m_billboardBatches[model];
		if(!batch.empty()){
			batch.push_back(billboard);
			m_billboardBatches[model] = batch;
		} else{
			std::vector<GameObject*> newBatch;
			newBatch.push_back(billboard);
			m_billboardBatches[model] = newBatch;
		}
	}

	void MasterRenderer::render(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderGameObjects();
		renderBillBoards();
	}

	void MasterRenderer::renderGameObjects(){
		m_entityShader.use();
		m_entityShader.loadBool("flashlightOn", true);

		for(auto const& model : m_gameObjectsBatches){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model.first->getMaterial().getDiffuseMap()->id);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, model.first->getMaterial().getSpecularMap()->id);
			glBindVertexArray(model.first->getMesh()->vertexArrayObject);

			for(auto const& entity : model.second){
				m_entityShader.loadBool("selected", entity->isSelected());
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, entity->getPosition());
				modelMatrix = glm::rotate(modelMatrix, entity->getRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
				modelMatrix = glm::rotate(modelMatrix, entity->getRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMatrix = glm::rotate(modelMatrix, entity->getRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
				modelMatrix = glm::scale(modelMatrix, entity->getScale());
				m_entityShader.loadMat4("model", modelMatrix);
				glDrawElements(GL_TRIANGLES, model.first->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
			}
		}
		m_gameObjectsBatches.clear();
	}

	void MasterRenderer::renderBillBoards(){
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

	void MasterRenderer::setUniforms(std::vector<Light*>& lights, Camera& camera){
		///entity shader
		m_entityShader.use();
		
		// directional light
		DirLight* dir = static_cast<DirLight*>(lights[0]);
		m_entityShader.loadVec3("dirLight.direction", -dir->direction);
		m_entityShader.loadVec3("dirLight.ambient", dir->ambient);
		m_entityShader.loadVec3("dirLight.diffuse", dir->diffuse);
		m_entityShader.loadVec3("dirLight.specular", dir->specular);
		//spot light
		SpotLight* spot = static_cast<SpotLight*>(lights[1]);
		m_entityShader.loadBool("flashlightOn", true);
		m_entityShader.loadVec3("spotLight.ambient", spot->ambient);
		m_entityShader.loadVec3("spotLight.diffuse", spot->diffuse);
		m_entityShader.loadVec3("spotLight.specular", spot->specular);
		m_entityShader.loadVec3("spotLight.att", spot->attenuation);
		m_entityShader.loadVec3("spotLight.position", spot->position);
		m_entityShader.loadVec3("spotLight.direction", spot->direction);
		m_entityShader.loadFloat("spotLight.cutOff", spot->cutOff);
		m_entityShader.loadFloat("spotLight.outerCutOff", spot->outerCutOff);
		//point lights
		int numLights = (lights.size()-2) > 4 ? 4 : lights.size()-2;
		m_entityShader.loadInt("pointLightsNum", numLights);
		for(size_t i = 0; i < numLights; i++){
			PointLight* point = static_cast<PointLight*>(lights[i+2]);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].position", point->position);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].ambient",  point->ambient);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].diffuse",  point->diffuse);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].specular", point->specular);
			m_entityShader.loadVec3("pointLights[" + std::to_string(i) + "].att",      point->attenuation);
		}

		//positional info
		m_entityShader.loadVec3("viewPos", camera.getPos());
		// view/projection transformations
		m_view = camera.getViewMatrix();
		m_entityShader.loadMat4("projection", m_projection);
		m_entityShader.loadMat4("view", m_view);
	}
}