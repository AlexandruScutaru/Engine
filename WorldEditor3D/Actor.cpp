#include "Actor.h"

#include <Engine/TexturedModel.h>

int Actor::objectCount = 0;

Actor::Actor() :
	m_model(nullptr),
	m_pos(glm::vec3(0.0f)),
	m_rot(glm::quat()),
	m_scale(glm::vec3(1.0f)),
	m_selected(false),
	m_atlasIndex(0)
{
	objectCount++;
	m_code = objectCount;
}

Actor::Actor(renderer::TexturedModel* model) :
	m_model(model),
	m_pos(glm::vec3(0.0f)),
	m_rot(glm::quat()),
	m_scale(glm::vec3(1.0f)),
	m_selected(false),
	m_atlasIndex(0)
{
	objectCount++;
	m_code = objectCount;
}

Actor::Actor(renderer::TexturedModel* model, glm::vec3 & pos, glm::quat & rot, glm::vec3 & scale) :
	m_model(model),
	m_pos(pos),
	m_rot(rot),
	m_scale(scale),
	m_selected(false),
	m_atlasIndex(0)
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
	m_atlasIndex = other.m_atlasIndex;

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

void Actor::setDoubleSided(bool doubleSided){
	m_model->setDoubleSided(doubleSided);
}

bool Actor::isDoubleSided(){
	return m_model->isDoubleSided();
}

bool& Actor::isDoubleSidedRef(){
	return m_model->isDoubleSidedRef();
}

bool Actor::isBillboard(){
	return m_model->isBillboard();
}

bool& Actor::isBillboardRef(){
	return m_model->isBillboardRef();
}

glm::vec2 Actor::getTextureOffset(){
	float x = m_atlasIndex % m_model->getAtlasSize() / (float)m_model->getAtlasSize();
	float y = m_atlasIndex / m_model->getAtlasSize() / (float)m_model->getAtlasSize();
	return glm::vec2(x, y);
}
