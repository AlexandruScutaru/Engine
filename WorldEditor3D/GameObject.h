#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H


#include <Engine/TexturedModel.h>
#include <Engine/PhysicsBody.h>

#include "Actor.h"


class GameObject : public Actor {
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

	void setDiffName(const std::string& diff){ m_diffName = diff; }
	void setSpecName(const std::string& spec){ m_specName = spec; }
	void setMeshName(const std::string& mesh){ m_meshName = mesh; }
	
	void setBodyType(physics::BodyType type){ m_bodyType = type; }
	void setName(const std::string& name){ m_name = name; }
	void setInEditorName(const std::string& inEditorName){ m_inEditorName = inEditorName; }

	physics::BodyType getBodyType(){ return m_bodyType; }
	const std::string& getName(){ return m_name; }
	const std::string& getInEditorName(){ return m_inEditorName; }
	const std::string& getDiffName(){ return m_diffName; }
	const std::string& getSpecName(){ return m_specName; }
	const std::string& getMeshName(){ return m_meshName; }

	void addColBody(renderer::CollisionBody& body){ m_colBodies.push_back(body); };
	void removeColBody(size_t index);
	std::vector<renderer::CollisionBody>& getColBodies(){ return m_colBodies; }
	void clearColBodies(){ m_colBodies.clear(); }

	physics::BodyType m_bodyType;
	bool m_gravityEnabled;
	bool m_allowedToSleep;
	float m_bounciness;
	float m_frictionCoefficient;
	float m_rollingResistance;
	float m_linearDamping;
	float m_angularDamping;

	int m_type;
	std::string m_updateScript;

private:
	std::string m_name;
	std::string m_inEditorName;
	std::string m_diffName;
	std::string m_specName;
	std::string m_meshName;

	std::vector<renderer::CollisionBody> m_colBodies;

};
#endif // !GAMEOBJECT_H
