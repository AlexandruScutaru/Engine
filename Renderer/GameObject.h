#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "ResourceManager.h"


namespace renderer{

	class GameObject{
	public:
		GameObject();
		GameObject(unsigned int model,
				   bool billboard
		);
		GameObject(unsigned int model, 
				   glm::vec3& pos = glm::vec3(0.0f), 
				   glm::vec3& rot = glm::vec3(0.0f), 
				   glm::vec3& scale = glm::vec3(1.0f), 
				   bool billboard = false
		);
		
		virtual ~GameObject();

		void setTexturedModel(unsigned int id){ m_model = id; }
		void setPosition(glm::vec3& pos){ m_pos = pos; }
		void setRotation(glm::vec3& rot){ m_rot = rot; }
		void setScale(glm::vec3& scale){ m_scale = scale; }

		TexturedModel* getTexturedModel(){ return ResourceManager::getTexturedModelAt(m_model); }
		const glm::vec3 getPosition(){ return m_pos; }
		const glm::vec3 getRotation(){ return m_rot; }
		const glm::vec3 getScale(){ return m_scale; }
		unsigned int getCode(){ return code; }
		bool isBillboard(){ return m_isBillboard; }

		virtual void update(float deltaTime){}

		static unsigned int objectCount;

	protected:
		bool m_isBillboard;
		unsigned int m_model;
		unsigned int code;
		
		glm::vec3 m_pos;
		glm::vec3 m_rot;
		glm::vec3 m_scale;

	};

}

#endif // !GAME_OBJECT_H
