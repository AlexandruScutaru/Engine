#include "GameObject.h"
#include <iostream>
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

void GameObject::setRotY(float rot_y){
	m_rot_y = rot_y;
	auto rot = glm::quat(glm::vec3(0.0f, glm::radians(rot_y), 0.0f));
	m_rot = rot;
	auto body = (*m_rigidBody).m_body;
	if(body->getType() == rp3d::BodyType::STATIC){
		rp3d::Transform trs = body->getTransform();
		trs.setOrientation(rp3d::Quaternion(rot.x, rot.y , rot.z, rot.w));
		body->setTransform(trs);
	}
}

float GameObject::getRotY(){
	return m_rot_y;
}
