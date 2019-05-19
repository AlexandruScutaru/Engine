#include "PhysicsWorld.h"
#include "PhysicsEventListener.h"

#include <iostream>


namespace physics{

	PhysicsWorld::PhysicsWorld() :
		m_world(rp3d::Vector3(0.0f, -9.81f, 0.0f))
	{}

	PhysicsWorld::PhysicsWorld(glm::vec3& gravity) :
		m_world(rp3d::Vector3(gravity.x, gravity.y, gravity.z))
	{}

	PhysicsWorld::~PhysicsWorld(){}


	void PhysicsWorld::setNbIterationsVelocitySolver(unsigned int val){
		m_world.setNbIterationsVelocitySolver(val);
	}

	void PhysicsWorld::setNbIterationsPositionSolver(unsigned int val){
		m_world.setNbIterationsPositionSolver(val);
	}

	void PhysicsWorld::enableSleeping(bool val){
		m_world.enableSleeping(val);
	}

	std::shared_ptr<physics::PhysicsBody> PhysicsWorld::createPhysicsBody(glm::vec3& pos, glm::quat& rot){
		rp3d::RigidBody* rigidBody = nullptr;
		
		rp3d::Vector3 position(pos.x, pos.y, pos.z);
		rp3d::Quaternion orientation(rot.x, rot.y, rot.z, rot.w);
		rp3d::Transform transform(position, orientation);

		rigidBody = m_world.createRigidBody(transform);
		std::shared_ptr<physics::PhysicsBody> physicsBody = std::make_shared<physics::PhysicsBody>(rigidBody);
		return physicsBody;
	}

	void PhysicsWorld::update(float timeStep){
		m_world.update(timeStep);
	}

	void PhysicsWorld::setEventListener(PhysicsEventListener* listener) {
		m_world.setEventListener(listener);
	}

	void PhysicsWorld::destroyBody(std::shared_ptr<physics::PhysicsBody>& body){
		m_world.destroyRigidBody(body->m_body);
	}

}