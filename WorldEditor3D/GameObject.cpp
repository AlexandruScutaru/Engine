#include "GameObject.h"

using namespace renderer;


GameObject::GameObject(){}

GameObject::GameObject(TexturedModel* model, bool static_obj) :
	RenderableEntity(model),
	m_isStatic(static_obj),
	m_selected(false)
{
	m_model->setBillboard(false);
}

GameObject::GameObject(TexturedModel* model, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, bool static_obj) :
	RenderableEntity(model),
	Actor(pos, rot, scale),
	m_isStatic(static_obj),
	m_selected(false)
{
	m_model->setBillboard(false);

}

GameObject::GameObject(const GameObject& other) :
	RenderableEntity(other),
	m_selected(false)
{
	m_isStatic = other.m_isStatic;
	m_model->setBillboard(other.m_model->isBillboard());
}

GameObject::~GameObject(){}
