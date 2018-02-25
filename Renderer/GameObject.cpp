#include "GameObject.h"
#include <climits>


namespace renderer{

	int GameObject::objectCount = 0;

	GameObject::GameObject():
		m_isStatic(false),
		m_pos(glm::vec3(0.0f)),
		m_rot(glm::vec3(0.0f)),
		m_scale(glm::vec3(1.0f)),
		m_model(nullptr)
	{
		objectCount++;
		code = objectCount;
	}

	GameObject::GameObject(TexturedModel* model, bool static_obj):
		m_model(model),
		m_isStatic(static_obj),
		m_pos(glm::vec3(0.0f)),
		m_rot(glm::vec3(0.0f)),
		m_scale(glm::vec3(1.0f))
	{
		objectCount++;
		code = objectCount;
	}

	GameObject::GameObject(TexturedModel* model, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, bool static_obj) :
		m_model(model),
		m_isStatic(static_obj),
		m_pos(pos),
		m_rot(rot),
		m_scale(scale)
	{
		objectCount++;
		code = objectCount;
	}

	GameObject::~GameObject(){}

}