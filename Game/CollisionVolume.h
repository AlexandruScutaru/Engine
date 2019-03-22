#ifndef COLLISION_VOLUME_H
#define COLLISION_VOLUME_H

#include <Engine/PhysicsBody.h>

#include "Actor.h"

#include <memory>


class CollisionVolume : public Actor {
public:
	enum class VolumeType {
		START = 0,
		END,
		TRIGGER
	};

	CollisionVolume(glm::vec3& pos = glm::vec3(0.0f));
	CollisionVolume(const CollisionVolume& other);
	virtual ~CollisionVolume();

	void setPhysicsBody(std::shared_ptr<physics::PhysicsBody> body){ m_rigidBody = body; }
	std::shared_ptr<physics::PhysicsBody> getPhysicsBody(){ return m_rigidBody; }
	VolumeType m_type;

private:
	std::shared_ptr<physics::PhysicsBody> m_rigidBody;

};


#endif // !COLLISION_VOLUME_H
