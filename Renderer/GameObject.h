#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "ResourceManager.h"


namespace renderer{

	class GameObject{
	public:
		GameObject();
		GameObject(TexturedModel* model,
				   bool static_obj
		);
		GameObject(TexturedModel* model,
				   glm::vec3& pos = glm::vec3(0.0f),
				   glm::vec3& rot = glm::vec3(0.0f),
				   glm::vec3& scale = glm::vec3(1.0f),
				   bool static_obj = false
		);
		GameObject(const GameObject& other);
		
		virtual ~GameObject();

		void setTexturedModel(TexturedModel* model){ m_model = model; }
		void setPosition(glm::vec3& pos){ m_pos = pos; }
		void setRotation(glm::vec3& rot){ m_rot = rot; }
		void setScale(glm::vec3& scale){ m_scale = scale; }
		void setName(const std::string& name){ m_name = name; }
		void setSelected(bool selected){ m_selected = selected; }

		TexturedModel* getTexturedModel(){ return m_model; }
		glm::vec3& getPosition(){ return m_pos; }
		glm::vec3& getRotation(){ return m_rot; }
		glm::vec3& getScale(){ return m_scale; }
		int getCode(){ return code; }
		bool isBillboard(){ return m_model->isBillboard(); }
		bool isStatic(){ return m_isStatic; }
		bool isSelected(){ return m_selected; }
		const std::string& getName(){ return m_name; }

		virtual void update(float deltaTime){}

		static int objectCount;

	protected:
		bool m_isStatic;
		bool m_selected;
		int code;
		TexturedModel* m_model;
		std::string m_name;
		
		glm::vec3 m_pos;
		glm::vec3 m_rot;
		glm::vec3 m_scale;

	};

}

#endif // !GAME_OBJECT_H
