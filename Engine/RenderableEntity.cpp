#include "RenderableEntity.h"


namespace renderer{

	RenderableEntity::RenderableEntity():
		m_model(nullptr)
	{}

	RenderableEntity::RenderableEntity(TexturedModel* model) :
		m_model(model)
	{}

	RenderableEntity::RenderableEntity(const RenderableEntity& other){
		m_model = other.m_model;
	}

	RenderableEntity::~RenderableEntity(){}

}