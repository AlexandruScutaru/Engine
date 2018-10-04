#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#include "CollisionBody.h"

#include <rp3d/reactphysics3d.h>

#include <vector>

namespace physics{

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
		void addCollisionShapes(glm::vec3& scaleFactor, std::vector<renderer::CollisionBody>& collisionBodies);
		void enableGravity(bool val);
		void allowSleep(bool val);
		void setBounciness(float val);
		void setFrictionCoefficient(float val);
		void setRollingResistance(float val);
		void setLinearDamping(float val);
		void setAngularDamping(float val);

		glm::vec3 getPostion();
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