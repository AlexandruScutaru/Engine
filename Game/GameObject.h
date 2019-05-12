#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <Engine/RenderableEntity.h>
#include <Engine/TexturedModel.h>
#include <Engine/PhysicsBody.h>

#include "Actor.h"

#include <memory>


class GameObject : public Actor, public renderer::RenderableEntity {
public:
	GameObject();
	GameObject(renderer::TexturedModel* model,
			   bool static_obj
	);
	GameObject(renderer::TexturedModel* model,
			   glm::vec3& pos = glm::vec3(0.0f),
			   glm::quat& rot = glm::quat(),
			   glm::vec3& scale = glm::vec3(1.0f),
			   bool static_obj = false
	);
	GameObject(const GameObject& other);
	virtual ~GameObject();

	enum TYPE {
		STANDARD = 0,
		KEY_PICKUP
	};

	void setIsBillboard(bool isBillboard){ m_model->setBillboard(isBillboard); }
	bool isBillboard(){ return m_model->isBillboard(); }
	void setPhysicsBody(std::shared_ptr<physics::PhysicsBody> body){ m_rigidBody = body; }
	void setType(int type) { m_type = type; }

	std::shared_ptr<physics::PhysicsBody> getPhysicsBody(){ return m_rigidBody; }
	int getType() { return m_type; }

private:
	std::shared_ptr<physics::PhysicsBody> m_rigidBody;
	int m_type;
};
#endif // !GAMEOBJECT_H
