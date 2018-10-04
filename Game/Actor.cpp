#include "Actor.h"


int Actor::objectCount = 0;

Actor::Actor() :
	m_pos(glm::vec3(0.0f)),
	m_rot(glm::quat()),
	m_scale(glm::vec3(1.0f)) 
{
	objectCount++;
	m_code = objectCount;
}

Actor::Actor(glm::vec3 & pos, glm::quat & rot, glm::vec3 & scale) :
	m_pos(pos),
	m_rot(rot),
	m_scale(scale)
{
	objectCount++;
	m_code = objectCount;
}

Actor::Actor(const Actor & other){
	m_pos = other.m_pos;
	m_rot = other.m_rot;
	m_scale = other.m_scale;

	objectCount++;
	m_code = objectCount;
}

Actor::~Actor(){}
