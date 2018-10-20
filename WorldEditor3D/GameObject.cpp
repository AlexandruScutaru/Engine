#include "GameObject.h"

using namespace renderer;


GameObject::GameObject(){}

GameObject::GameObject(TexturedModel* model, bool static_obj) :
	RenderableEntity(model),
	m_selected(false),
	m_diffName("default.png"),
	m_specName("no_SPEC.png"),
	m_meshName("sphere.obj"),
	m_bodyType(physics::BodyType::STATIC),
	m_gravityEnabled(true),
	m_allowedToSleep(true),
	m_bounciness(-1.0f),
	m_frictionCoefficient(-1.0f),
	m_rollingResistance(-1.0f),
	m_linearDamping(0.0f),
	m_angularDamping(0.0f)
{}

GameObject::GameObject(TexturedModel* model, glm::vec3& pos, glm::quat& rot, glm::vec3& scale, bool static_obj) :
	RenderableEntity(model),
	Actor(pos, rot, scale),
	m_selected(false),
	m_diffName("default.png"),
	m_specName("no_SPEC.png"),
	m_meshName("sphere.obj"),
	m_bodyType(physics::BodyType::STATIC),
	m_gravityEnabled(true),
	m_allowedToSleep(true),
	m_bounciness(-1.0f),
	m_frictionCoefficient(-1.0f),
	m_rollingResistance(-1.0f),
	m_linearDamping(0.0f),
	m_angularDamping(0.0f)
{}

GameObject::GameObject(const GameObject& other) :
	Actor(other),
	RenderableEntity(other),
	CollidableEntity(other),
	m_selected(false)
{
	m_name					= other.m_name;
	m_inEditorName			= other.m_inEditorName;
	m_diffName				= other.m_diffName;
	m_specName				= other.m_specName;
	m_meshName				= other.m_meshName;
	m_bodyType  			= other.m_bodyType;
	m_gravityEnabled		= other.m_gravityEnabled;
	m_allowedToSleep		= other.m_allowedToSleep;
	m_bounciness			= other.m_bounciness;
	m_frictionCoefficient	= other.m_frictionCoefficient;
	m_rollingResistance		= other.m_rollingResistance;
	m_linearDamping			= other.m_linearDamping;
	m_angularDamping		= other.m_angularDamping;
}

GameObject::~GameObject(){}
