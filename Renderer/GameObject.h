#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "ResourceManager.h"


namespace renderer{

	class GameObject{
	public:
		GameObject();
		GameObject(unsigned int model);
		virtual ~GameObject();

		void setTexturedModel(unsigned int id){ m_model = id; }
		void setPosition(glm::vec3& pos){ m_pos = pos; }
		void setRotation(glm::vec3& rot){ m_rot = rot; }
		void setScale(glm::vec3& scale){ m_scale = scale; }

		TexturedModel* getTexturedModel(){ return ResourceManager::getTexturedModelAt(m_model); }
		const glm::vec3 getPosition(){ return m_pos; }
		const glm::vec3 getRotation(){ return m_rot; }
		const glm::vec3 getScale(){ return m_scale; }

		virtual void update(float deltaTime) = 0;
		virtual void prepareToRender() = 0;
		virtual void render() = 0;

	protected:
		unsigned int m_model;
		glm::vec3 m_pos;
		glm::vec3 m_rot;
		glm::vec3 m_scale;

	};

}

#endif // !GAME_OBJECT_H
