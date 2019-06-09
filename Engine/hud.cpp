#include "hud.h"
#include "ResourceManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace renderer{

	HUD::HUD(){}

	HUD::~HUD(){}


	void HUD::initHUD(){
		init("res/shaders/hud");
		connectTextureUnits();
		m_digits.resize(10);

		for(int i = 0; i < 10; i++){
			m_digits[i] = renderer::HudElement{utilities::ResourceManager::getTexture("res/textures/" + std::to_string(i) + "_digit.png")};
		}
	}

	void HUD::render(std::vector<HudElement>& elements){
		use();
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(utilities::ResourceManager::getNDCquad()->vertexArrayObject);
		
		for(auto& element : elements){
			if(!element.enabled) continue;
			glBindTexture(GL_TEXTURE_2D,  element.texture->id);
			glm::mat4 model;
			model = glm::translate(model, element.pos);
			model = model * glm::toMat4(glm::quat());
			model = glm::scale(model, element.scale);
			loadModelMatrix(model);
			glDrawArrays(GL_TRIANGLES, 0, utilities::ResourceManager::getNDCquad()->indexCount);
		}
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		
		unuse();
	}

	void HUD::render(int num, glm::vec3 & pos, glm::vec3 & scale){
		use();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(utilities::ResourceManager::getNDCquad()->vertexArrayObject);

		glBindTexture(GL_TEXTURE_2D, m_digits[num].texture->id);
		glm::mat4 model;
		model = glm::translate(model, pos);
		model = model * glm::toMat4(glm::quat());
		model = glm::scale(model, scale);
		loadModelMatrix(model);
		glDrawArrays(GL_TRIANGLES, 0, utilities::ResourceManager::getNDCquad()->indexCount);
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		unuse();
		
		
		/*

		int n = 123589;
		int copy = n;
		int power = 1;
		while(copy!= 0){
		power*=10;
		copy /= 10;
		}
		std::cout << power<<std::endl;;
		while(n != 0){
		power /= 10;
		std::cout << n / power << " ";
		n %= power;

		}

		*/
	}

	void HUD::loadModelMatrix(glm::mat4 & model){
		loadMat4(m_uniforms[MODEL_MATRIX_U], model);
	}

	void HUD::connectTextureUnits(){
		loadInt(m_uniforms[TEXTURE_U], 0);
	}

	void HUD::getAllUniformLocations(){
		m_uniforms[MODEL_MATRIX_U] = getUniformLocation("model");
		m_uniforms[TEXTURE_U] = getUniformLocation("hudTexture");
	}

}