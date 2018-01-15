#include "GameObject.h"
#include <climits>


namespace renderer{

	int GameObject::objectCount = 0;

	GameObject::GameObject():
		m_isBillboard(false),
		m_pos(glm::vec3(0.0f)),
		m_rot(glm::vec3(0.0f)),
		m_scale(glm::vec3(1.0f)),
		m_model(UINT_MAX)
	{
		objectCount++;
		code = objectCount;
	}

	GameObject::GameObject(unsigned int model, bool billboard):
		m_model(model),
		m_isBillboard(billboard),
		m_pos(glm::vec3(0.0f)),
		m_rot(glm::vec3(0.0f)),
		m_scale(glm::vec3(1.0f))
	{
		objectCount++;
		code = objectCount;
	}

	GameObject::GameObject(unsigned int model, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, bool billboard) :
		m_model(model),
		m_isBillboard(billboard),
		m_pos(pos),
		m_rot(rot),
		m_scale(scale)
	{
		objectCount++;
		code = objectCount;
	}

	GameObject::~GameObject(){}

}