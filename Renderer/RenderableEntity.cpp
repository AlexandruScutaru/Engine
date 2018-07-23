#include "RenderableEntity.h"
#include <climits>


namespace renderer{

	int RenderableEntity::objectCount = 0;

	RenderableEntity::RenderableEntity():
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

	RenderableEntity::RenderableEntity(TexturedModel* model, bool static_obj):
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

	RenderableEntity::RenderableEntity(TexturedModel* model, glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, bool static_obj) :
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

	RenderableEntity::RenderableEntity(const RenderableEntity& other):
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

	RenderableEntity::~RenderableEntity(){}

}