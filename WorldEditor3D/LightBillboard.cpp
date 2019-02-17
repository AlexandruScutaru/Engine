#include "LightBillboard.h"
#include <Engine/Lights.h>

LightBillboard::LightBillboard() :
	Actor(),
	m_light(nullptr)
{}

LightBillboard::LightBillboard(renderer::TexturedModel * model, renderer::Light * light, glm::vec3 & pos, glm::quat & rot, glm::vec3 & scale) :
	Actor(model, pos, rot, scale),
	m_light(light)
{}

LightBillboard::~LightBillboard(){}


void LightBillboard::setPosition(glm::vec3 & pos){
	m_pos = pos;
	if(m_light->casterType == renderer::Caster::DIRECTIONAL){
		static_cast<renderer::DirLight*>(m_light)->direction = pos;
	}else if(m_light->casterType == renderer::Caster::POINT){
		static_cast<renderer::PointLight*>(m_light)->position = pos;
	}
}

void LightBillboard::update(float deltatime){
	if(m_light->casterType == renderer::Caster::DIRECTIONAL){
		static_cast<renderer::DirLight*>(m_light)->direction = m_pos;
	} else if(m_light->casterType == renderer::Caster::POINT){
		static_cast<renderer::PointLight*>(m_light)->position = m_pos;
	}
}

