#include "PhysicsBody.h"
#include "Logger.h"

#include <GLM/gtx/euler_angles.hpp> 

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>


namespace physics{
	PhysicsBody::PhysicsBody() :
		m_body(nullptr)
	{}

	PhysicsBody::PhysicsBody(rp3d::RigidBody* body) :
		m_body(body)
	{}

	PhysicsBody::~PhysicsBody(){
		//for(auto proxy : m_proxyhapes)
		//	m_body->removeCollisionShape(proxy);
		//for(auto shape : m_boxShapes)
		//	delete shape;
		//for(auto shape : m_sphereShapes)
		//	delete shape;
		//for(auto shape : m_capsuleShapes)
		//	delete shape;
	}


	void PhysicsBody::setBodyType(BodyType type){
		rp3d::BodyType bType;
		switch(type){
		case BodyType::STATIC:
			bType = rp3d::BodyType::STATIC;
			break;
		case BodyType::KINEMATIC:
			bType = rp3d::BodyType::KINEMATIC;
			break;
		case BodyType::DYNAMIC:
			bType = rp3d::BodyType::DYNAMIC;
			break;
		default:
			bType = rp3d::BodyType::STATIC;
			break;
		}
		m_body->setType(bType);
	}

	void PhysicsBody::addCollisionShapes(glm::vec3 & scaleFactor, std::vector<renderer::CollisionBody>& collisionBodies, CollisionCategory category){
		for(auto& body : collisionBodies){
			switch(body.shape){
			case renderer::SHAPE_CUBE:
				addBoxShape(scaleFactor, body);
				break;
			case renderer::SHAPE_SPHERE:
				addSphereShape(scaleFactor, body);
				break;
			case renderer::SHAPE_CAPSULE:
				addCapsuleShape(scaleFactor, body);
				break;
			default:
				LOG_E_INFO("Trying to add unknown shape index {}, shape not added!", body.shape);
				break;
			}
			m_proxyhapes.back()->setCollisionCategoryBits(category);
			if(category == CollisionCategory::PLAYER)
				m_proxyhapes.back()->setCollideWithMaskBits(CollisionCategory::GENERAL | CollisionCategory::TRIGGER);
			else if(category == CollisionCategory::GENERAL)
				m_proxyhapes.back()->setCollideWithMaskBits(CollisionCategory::PLAYER | CollisionCategory::GENERAL);
			//else if(category == CollisionCategory::TRIGGER)
			//	m_proxyhapes.back()->setCollideWithMaskBits(CollisionCategory::PLAYER);
		}
	}

	void PhysicsBody::addHeightFieldTerrain(int rows, int cols, float min, float max, float * data, glm::vec3& scaling){
		m_heightFieldShape = new rp3d::HeightFieldShape(rows, cols, min, max, data, rp3d::HeightFieldShape::HEIGHT_FLOAT_TYPE);
		m_heightFieldShape->setLocalScaling(rp3d::Vector3(scaling.x, scaling.y, scaling.z));
		m_proxyhapes.push_back(m_body->addCollisionShape(m_heightFieldShape, rp3d::Transform::identity(), 1.0));
		m_proxyhapes.back()->setCollideWithMaskBits(physics::CollisionCategory::PLAYER | physics::CollisionCategory::GENERAL);
		m_body->setType(rp3d::BodyType::STATIC);
		setBounciness(0.0f);
	}

	void PhysicsBody::enableGravity(bool val){
		m_body->enableGravity(val);
	}

	void PhysicsBody::allowSleep(bool val){
		m_body->setIsAllowedToSleep(val);
	}

	void PhysicsBody::setBounciness(float val){
		if(val < 0.0f)
			return;
		if(val > 1.0f) val = 1.0f;
		rp3d::Material& material = m_body->getMaterial();
		material.setBounciness(val);
	}

	void PhysicsBody::setFrictionCoefficient(float val){
		if(val < 0.0f)
			return;
		if(val > 1.0f) val = 1.0f;
		rp3d::Material& material = m_body->getMaterial();
		material.setFrictionCoefficient(val);
	}

	void PhysicsBody::setRollingResistance(float val){
		if(val < 0.0f)
			return;
		if(val > 1.0f) val = 1.0f;
		rp3d::Material& material = m_body->getMaterial();
		material.setRollingResistance(val);
	}

	void PhysicsBody::setLinearDamping(float val){
		m_body->setLinearDamping(val);
	}

	void PhysicsBody::setAngularDamping(float val){
		m_body->setAngularDamping(val);
	}

	void PhysicsBody::applyForceToCenterOfMass(glm::vec3 & force){
		m_body->applyForceToCenterOfMass(rp3d::Vector3(force.x, force.y, force.z));
	}

	void PhysicsBody::setLinearVelocity(glm::vec3 & velocity){
		m_body->setLinearVelocity(rp3d::Vector3(velocity.x, velocity.y, velocity.z));
	}

	glm::vec3 PhysicsBody::getLinearVelocity(){
		auto velocity = m_body->getLinearVelocity();
		return glm::vec3(velocity.x, velocity.y, velocity.z);
	}

	glm::vec3 PhysicsBody::getPosition(){
		auto pos = m_body->getTransform().getPosition();
		return glm::vec3(pos.x, pos.y, pos.z);
	}

	glm::quat PhysicsBody::getRotation(){
		rp3d::Transform trs = m_body->getTransform();
		rp3d::Quaternion orientation = trs.getOrientation();
		
		glm::quat rot(orientation.w, orientation.x, orientation.y, orientation.z);

		return rot;
	}

	void PhysicsBody::addBoxShape(glm::vec3& scaleFactor, renderer::CollisionBody& body){
		glm::vec3 pos = body.colRelativePos;
		glm::quat rot = body.colRotQuat;
		glm::vec3 size = body.colScale * scaleFactor;

		rp3d::Vector3 halfDimenions(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);

		rp3d::BoxShape* boxShape = new rp3d::BoxShape(halfDimenions);
		m_boxShapes.push_back(boxShape);

		rp3d::Vector3 position = rp3d::Vector3(pos.x, pos.y, pos.z);
		rp3d::Quaternion orientation(rot.x, rot.y, rot.z, rot.w);
		rp3d::Transform transform(position, orientation);

		m_proxyhapes.push_back(m_body->addCollisionShape(boxShape, transform, body.mass));
	}

	void PhysicsBody::addSphereShape(glm::vec3& scaleFactor, renderer::CollisionBody& body){
		glm::vec3 pos = body.colRelativePos;		
		float radius = body.colScale.x * scaleFactor.x / 2.0f;

		rp3d::SphereShape* sphereShape = new rp3d::SphereShape(radius);
		m_sphereShapes.push_back(sphereShape);

		rp3d::Vector3 position = rp3d::Vector3(pos.x, pos.y, pos.z);
		rp3d::Quaternion orientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(position, orientation);

		m_proxyhapes.push_back(m_body->addCollisionShape(sphereShape, transform, body.mass));
	}

	void PhysicsBody::addCapsuleShape(glm::vec3& scaleFactor, renderer::CollisionBody& body){
		glm::vec3 pos = body.colRelativePos;
		glm::quat rot = body.colRotQuat;
		glm::vec3 size = body.colScale * scaleFactor;
		
		float radius = size.x / 2.0f;
		float height = size.y - 2*radius;
		rp3d::CapsuleShape* capsuleShape = new rp3d::CapsuleShape(radius, height);
		m_capsuleShapes.push_back(capsuleShape);
		
		rp3d::Vector3 position = rp3d::Vector3(pos.x, pos.y, pos.z);
		rp3d::Quaternion orientation(rot.x, rot.y, rot.z, rot.w);
		rp3d::Transform transform(position, orientation);
		
		m_proxyhapes.push_back(m_body->addCollisionShape(capsuleShape, transform, body.mass));
	}

}