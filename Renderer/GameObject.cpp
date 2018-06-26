#include "GameObject.h"
#include <climits>


namespace renderer{

	int GameObject::objectCount = 0;

	GameObject::GameObject():
		m_isStatic(false),
		m_selected(false),
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
		m_selected(false),
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
		m_selected(false),
		m_pos(pos),
		m_rot(rot),
		m_scale(scale)
	{
		objectCount++;
		code = objectCount;
	}

	GameObject::GameObject(const GameObject& other):
		m_selected(false)
	{
		m_model = other.m_model;
		m_isStatic = other.m_isStatic;
		m_pos = other.m_pos;
		m_rot = other.m_rot;
		m_scale = other.m_scale;
		m_name = other.m_name;
		m_inEditorName = other.m_inEditorName;

		objectCount++;
		code = objectCount;
	}

	GameObject::~GameObject(){}

}