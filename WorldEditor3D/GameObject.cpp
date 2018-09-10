#include "GameObject.h"

using namespace renderer;


GameObject::GameObject(){}

GameObject::GameObject(TexturedModel* model, bool static_obj) :
	RenderableEntity(model),
	m_isStatic(static_obj),
	m_selected(false),
	m_diffName("default.png"),
	m_specName("no_SPEC.png"),
	m_meshName("sphere.obj")
{}

GameObject::GameObject(TexturedModel* model, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, bool static_obj) :
	RenderableEntity(model),
	Actor(pos, rot, scale),
	m_isStatic(static_obj),
	m_selected(false),
	m_diffName("default.png"),
	m_specName("no_SPEC.png"),
	m_meshName("sphere.obj")
{}

GameObject::GameObject(const GameObject& other) :
	Actor(other),
	RenderableEntity(other),
	CollidableEntity(other),
	m_selected(false)
{
	m_name			= other.m_name;
	m_inEditorName	= other.m_inEditorName;
	m_diffName		= other.m_diffName;
	m_specName		= other.m_specName;
	m_meshName		= other.m_meshName;
	m_isStatic  	= other.m_isStatic;
}

GameObject::~GameObject(){}
