#include "Utilities.h"
#include "CollisionVolume.h"
#include "GameObject.h"

#include <Engine/PhysicsWorld.h>
#include <Engine/ResourceManager.h>
#include <Engine/PhysicsWorld.h>

#include <fstream>
#include <algorithm>
#include <JSON/json.hpp>
using json = nlohmann::json;


GameObject* Utilities::OpenGameObject(const std::string& file, glm::vec3& pos, glm::quat& rot, glm::vec3& scale, physics::PhysicsWorld* world){
	std::ifstream in("res/gameobjects/" + file);
	json obj;
	in >> obj;
	in.close();

	std::vector<renderer::CollisionBody> colBodies;
	for(auto it = obj["collision"].begin(); it != obj["collision"].end(); it++){
		json col = it.value();
		renderer::CollisionBody body;
		body.shape = col["shape"].get<int>();
		body.mass = col["mass"].get<float>();
		std::vector<float> p = col["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = col["rot"].get<std::vector<float>>();
		glm::quat rot(r[3], r[0], r[1], r[2]);
		std::vector<float> s = col["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);
		body.colRelativePos = pos;
		body.colRotQuat = rot;
		body.colScale = scale;
		colBodies.push_back(body);
	}

	GameObject* object;
	object = new GameObject(utilities::ResourceManager::loadModel(file));
	auto physicsBody = world->createPhysicsBody(pos, rot);
	
	physicsBody->enableGravity(obj["gravity"].get<bool>());
	physicsBody->allowSleep(obj["sleep"].get<bool>());
	//physicsBody->setBounciness(obj["bounciness"].get<float>());
	physicsBody->setBounciness(0.0f);
	physicsBody->setFrictionCoefficient(obj["friction"].get<float>());
	physicsBody->setRollingResistance(obj["rollingResist"].get<float>());
	physicsBody->setLinearDamping(obj["linearDamp"].get<float>());
	physicsBody->setAngularDamping(obj["angularDamp"].get<float>());
	physicsBody->addCollisionShapes(scale, colBodies, physics::CollisionCategory::GENERAL);
	physicsBody->setBodyType(physics::BodyType::STATIC);
	object->setPhysicsBody(physicsBody);
	object->setPosition(pos);
	object->setRotation(rot);
	object->setScale(scale);

	return object;
}