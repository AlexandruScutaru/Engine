#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "PhysicsBody.h"
#include <memory>

#include <rp3d/reactphysics3d.h>
#include <GLM/glm.hpp>


namespace physics{
	class PhysicsEventListener;

	class PhysicsWorld{
	public:
		PhysicsWorld();
		PhysicsWorld(glm::vec3& gravity);
		~PhysicsWorld();

		void setNbIterationsVelocitySolver(unsigned int val);
		void setNbIterationsPositionSolver(unsigned int val);
		void enableSleeping(bool val);
		void update(float timeStep);
		void setEventListener(PhysicsEventListener* listener);
		void destroyBody(std::shared_ptr<physics::PhysicsBody>& body);
		void addTerrainHeightField(float* heights, float min, float max, float size);
		std::shared_ptr<physics::PhysicsBody> createPhysicsBody(glm::vec3& pos, glm::quat& rot);


	private:
		rp3d::DynamicsWorld m_world;

	};
}

#endif // !PHYSICS_WORLD_H
