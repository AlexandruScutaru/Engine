#include "Actor.h"

#include <Engine/TexturedModel.h>

int Actor::objectCount = 0;

Actor::Actor() :
	m_model(nullptr),
	m_pos(glm::vec3(0.0f)),
	m_rot(glm::quat()),
	m_scale(glm::vec3(1.0f)),
	m_selected(false)
{
	objectCount++;
	m_code = objectCount;
}

Actor::Actor(renderer::TexturedModel* model) :
	m_model(model),
	m_pos(glm::vec3(0.0f)),
	m_rot(glm::quat()),
	m_scale(glm::vec3(1.0f)),
	m_selected(false)
{
	objectCount++;
	m_code = objectCount;
}

Actor::Actor(renderer::TexturedModel* model, glm::vec3 & pos, glm::quat & rot, glm::vec3 & scale) :
	m_model(model),
	m_pos(pos),
	m_rot(rot),
	m_scale(scale),
	m_selected(false)
{
	objectCount++;
	m_code = objectCount;
}

Actor::Actor(const Actor& other):
	m_selected(false)
{
	m_pos = other.m_pos;
	m_rot = other.m_rot;
	m_scale = other.m_scale;
	m_model = other.m_model;

	objectCount++;
	m_code = objectCount;
}

Actor::~Actor(){}

void Actor::setIsBillboard(bool isBillboard){
	m_model->setBillboard(isBillboard);
}

void Actor::setTexturedModel(renderer::TexturedModel * model){
	m_model = model;
}

bool Actor::isBillboard(){
	return m_model->isBillboard();
}

bool & Actor::isBillboardRef(){
	return m_model->isBillboardRef();
}
