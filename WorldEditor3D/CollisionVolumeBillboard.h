#ifndef COLLISION_VOLUME_BILLBOARD_H
#define COLLISION_VOLUME_BILLBOARD_H


#include "Actor.h"
#include <Engine/CollisionBody.h>
#include <string>

enum class VolumeType {
	START = 0,
	END,
	TRIGGER
};

class CollisionVolumeBillboard : public Actor {
public:
	CollisionVolumeBillboard();
	CollisionVolumeBillboard(renderer::TexturedModel* model,
				   const std::string& name,
				   renderer::CollisionShapes shape = renderer::CollisionShapes::SHAPE_CUBE,
				   glm::vec3& pos = glm::vec3(0.0f),
				   glm::quat& rot = glm::quat(),
				   glm::vec3& scale = glm::vec3(1.0f)
	);
	
	virtual ~CollisionVolumeBillboard();

	virtual void setPosition(glm::vec3& pos) override;
	virtual void update(float deltatime) override;

	void setName(const std::string& name){ m_inEditorName = name; }
	void setBody(renderer::CollisionBody& body){ m_colBody = body; }
	void setVolumeType(VolumeType type){ m_type = type; }

	const std::string& getName(){ return m_inEditorName; }
	int getVolumeType(){ return static_cast<int>(m_type); }
	renderer::CollisionBody& getColBodyRef(){ return m_colBody; }
	renderer::CollisionBody* getColBodyPtr(){ return &m_colBody; }
	
	std::string triggerScript;

private:
	std::string m_inEditorName;
	renderer::CollisionBody m_colBody;
	VolumeType m_type;

};

#endif // !LIGHT_BILLBOARD_H

