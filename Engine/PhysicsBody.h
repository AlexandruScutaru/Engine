#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#include "CollisionBody.h"

#include <rp3d/reactphysics3d.h>

#include <vector>

namespace physics{

	enum CollisionCategory {
		GENERAL = 0x001,
		TRIGGER = 0x002,
		PLAYER  = 0x004
	};

	enum class BodyType { 
		STATIC, 
		KINEMATIC, 
		DYNAMIC
	};

	class PhysicsBody{
	public:
		PhysicsBody();
		PhysicsBody(rp3d::RigidBody* body);

		PhysicsBody(const PhysicsBody&) = delete;
		PhysicsBody& operator=(const PhysicsBody&) = delete;
		PhysicsBody& operator=(PhysicsBody&&) = delete;

		~PhysicsBody();

		void setBodyType(BodyType type = BodyType::DYNAMIC);
		void addCollisionShapes(glm::vec3& scaleFactor, std::vector<renderer::CollisionBody>& collisionBodies, CollisionCategory category = CollisionCategory::GENERAL);
		void enableGravity(bool val);
		void allowSleep(bool val);
		void setBounciness(float val);
		void setFrictionCoefficient(float val);
		void setRollingResistance(float val);
		void setLinearDamping(float val);
		void setAngularDamping(float val);

		void applyForceToCenterOfMass(glm::vec3& force);
		void setLinearVelocity(glm::vec3& velocity);
		glm::vec3 getLinearVelocity();

		glm::vec3 getPosition();
		glm::quat getRotation();

	public:
		void addBoxShape(glm::vec3& scaleFactor, renderer::CollisionBody& body);
		void addSphereShape(glm::vec3& scaleFactor, renderer::CollisionBody& body);
		void addCapsuleShape(glm::vec3& scaleFactor, renderer::CollisionBody& body);

		std::vector<rp3d::BoxShape*> m_boxShapes;
		std::vector<rp3d::SphereShape*> m_sphereShapes;
		std::vector<rp3d::CapsuleShape*> m_capsuleShapes;
		std::vector<rp3d::ProxyShape*> m_proxyhapes;

		rp3d::RigidBody* m_body;
	};

}
#endif //PHYSICS_BODY_H