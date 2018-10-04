#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "PhysicsBody.h"
#include <memory>

#include <rp3d/reactphysics3d.h>
#include <GLM/glm.hpp>


namespace physics{

	class PhysicsWorld{
	public:
		PhysicsWorld();
		PhysicsWorld(glm::vec3& gravity);
		~PhysicsWorld();

		void setNbIterationsVelocitySolver(unsigned int val);
		void setNbIterationsPositionSolver(unsigned int val);
		void enableSleeping(bool val);
		void update(float timeStep);

		std::shared_ptr<physics::PhysicsBody> createPhysicsBody(glm::vec3& pos, glm::vec3& rot);

	private:
		rp3d::DynamicsWorld m_world;

	};
}

#endif // !PHYSICS_WORLD_H
