#include "PhysicsEventListener.h"
#include <iostream>


namespace physics{

	PhysicsEventListener::PhysicsEventListener(std::function<void(const rp3d::ContactPointInfo&)> callback) {
		m_beginContactCallback = callback;
	}

	PhysicsEventListener::~PhysicsEventListener() {}


	void PhysicsEventListener::beginContact(const rp3d::ContactPointInfo& contact) {
		m_beginContactCallback(contact);
	}

	void PhysicsEventListener::newContact(const rp3d::ContactPointInfo& contact){
		//std::cout << "contact";
	}

}