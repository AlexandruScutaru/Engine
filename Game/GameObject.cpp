#include "GameObject.h"

using namespace renderer;


GameObject::GameObject(){}

GameObject::GameObject(TexturedModel* model, bool static_obj) :
	RenderableEntity(model),
	m_type(GameObject::TYPE::STANDARD)
{}

GameObject::GameObject(TexturedModel* model, glm::vec3& pos, glm::quat& rot, glm::vec3& scale, bool static_obj) :
	RenderableEntity(model),
	Actor(pos, rot, scale),
	m_type(GameObject::TYPE::STANDARD)
{}

GameObject::GameObject(const GameObject& other) :
	Actor(other),
	RenderableEntity(other)
{
	m_type = other.m_type;
}

GameObject::~GameObject(){}

void GameObject::setPosition(glm::vec3 & pos){
	m_pos = pos;
	auto body = (*m_rigidBody).m_body;
	if(body->getType() == rp3d::BodyType::STATIC){
		rp3d::Transform trs = body->getTransform();
		trs.setPosition(rp3d::Vector3(pos.x, pos.y, pos.z));
		body->setTransform(trs);
	}
}

glm::vec2 GameObject::getTextureOffset(){
	float x = m_atlasIndex % m_model->getAtlasSize() / (float)m_model->getAtlasSize();
	float y = m_atlasIndex / m_model->getAtlasSize() / (float)m_model->getAtlasSize();
	return glm::vec2(x, y);
}
