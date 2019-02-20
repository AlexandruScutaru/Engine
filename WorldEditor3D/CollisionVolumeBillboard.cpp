#include "CollisionVolumeBillboard.h"



CollisionVolumeBillboard::CollisionVolumeBillboard() :
	Actor(),
	m_colBody(renderer::CollisionBody()),
	m_inEditorName(""),
	m_type(VolumeType::TRIGGER)
{}

CollisionVolumeBillboard::CollisionVolumeBillboard(renderer::TexturedModel* model, const std::string& name,
												   renderer::CollisionShapes shape, 
												   glm::vec3& pos, glm::quat& rot, glm::vec3& scale):
	Actor(model, pos, rot, glm::vec3(0.5f)),
	m_type(VolumeType::TRIGGER)
{
	m_colBody = renderer::CollisionBody();
	m_colBody.shape = (int)shape;
	m_colBody.colScale = scale;
	m_inEditorName = name;
}

CollisionVolumeBillboard::~CollisionVolumeBillboard(){}


void CollisionVolumeBillboard::setPosition(glm::vec3& pos){
	m_pos = pos;
	m_colBody.colRelativePos = pos;
}

void CollisionVolumeBillboard::update(float deltatime){
	m_colBody.colRelativePos = m_pos;
	m_colBody.colScale += m_scale - glm::vec3(0.5f), m_scale = glm::vec3(0.5f);
	m_colBody.colRotQuat = m_rot;
}
