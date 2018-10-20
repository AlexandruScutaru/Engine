#include "Utilities.h"
#include "GameObject.h"

#include <Engine/Lights.h>
#include <Engine/ResourceManager.h>
#include <Engine/PhysicsWorld.h>
#include <fstream>

#include <JSON/json.hpp>
using json = nlohmann::json;


void Utilities::OpenMap(const std::string & file, std::vector<GameObject*>& objects, std::vector<renderer::Light*>& lights, physics::PhysicsWorld* world){
	std::ifstream in(file);
	json mapFile;
	in >> mapFile;
	in.close();

	//gameobjects
	GameObject* object;
	for(auto it = mapFile["gameobjects"].begin(); it != mapFile["gameobjects"].end(); ++it){
		json obj = it.value();
		std::string obj_name = obj["name"].get<std::string>();
		physics::BodyType type = static_cast<physics::BodyType>(obj["bodyType"].get<int>());
		std::vector<float> p = obj["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = obj["rot"].get<std::vector<float>>();
		glm::quat rot(r[3], r[0], r[1], r[2]);
		std::vector<float> s = obj["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);

		object = OpenGameObject(obj_name, pos, rot, scale, world); //info about collision
		object->setPosition(pos);
		object->setRotation(rot);
		object->setScale(scale);
		object->getPhysicsBody()->setBodyType(type);
		objects.push_back(object);
	}

	//lights
	std::vector<float> v;
	///dirlight
	v = mapFile["lights"]["dirLight"]["amb"].get<std::vector<float>>();
	glm::vec3 amb = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["dirLight"]["diff"].get<std::vector<float>>();
	glm::vec3 diff = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["dirLight"]["spec"].get<std::vector<float>>();
	glm::vec3 spec = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["dirLight"]["dir"].get<std::vector<float>>();
	glm::vec3 dir = glm::vec3(v[0], v[1], v[2]);
	lights.push_back(new renderer::DirLight(amb, diff, spec, dir));

	///spotlight
	v = mapFile["lights"]["spotLight"]["amb"].get<std::vector<float>>();
	amb = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["spotLight"]["diff"].get<std::vector<float>>();
	diff = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["spotLight"]["spec"].get<std::vector<float>>();
	spec = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["spotLight"]["dir"].get<std::vector<float>>();
	dir = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["spotLight"]["pos"].get<std::vector<float>>();
	glm::vec3 pos = glm::vec3(v[0], v[1], v[2]);
	v.clear(), v = mapFile["lights"]["spotLight"]["att"].get<std::vector<float>>();
	glm::vec3 att = glm::vec3(v[0], v[1], v[2]);
	float cutOff = mapFile["lights"]["spotLight"]["cutOff"].get<float>();
	float outCutOff = mapFile["lights"]["spotLight"]["outCutOff"].get<float>();
	lights.push_back(new renderer::SpotLight(amb, diff, spec, dir, pos, att, cutOff, outCutOff));

	///pointlights
	for(auto it = mapFile["lights"]["pointLights"].begin(); it != mapFile["lights"]["pointLights"].end(); ++it){
		json pointLight = it.value();
		v = pointLight["amb"].get<std::vector<float>>();
		amb = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = pointLight["diff"].get<std::vector<float>>();
		diff = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = pointLight["spec"].get<std::vector<float>>();
		spec = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = pointLight["pos"].get<std::vector<float>>();
		pos = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = pointLight["att"].get<std::vector<float>>();
		att = glm::vec3(v[0], v[1], v[2]);
		lights.push_back(new renderer::PointLight(amb, diff, spec, pos, att));
	}
}

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
	object->setIsBillboard(obj["billboard"].get<bool>());
	
	auto physicsBody = world->createPhysicsBody(pos, rot);
	
	physicsBody->enableGravity(obj["gravity"].get<bool>());
	physicsBody->allowSleep(obj["sleep"].get<bool>());
	physicsBody->setBounciness(obj["bounciness"].get<float>());
	physicsBody->setFrictionCoefficient(obj["friction"].get<float>());
	physicsBody->setRollingResistance(obj["rollingResist"].get<float>());
	physicsBody->setLinearDamping(obj["linearDamp"].get<float>());
	physicsBody->setAngularDamping(obj["angularDamp"].get<float>());
	physicsBody->addCollisionShapes(scale, colBodies);
	
	object->setPhysicsBody(physicsBody);

	return object;
}

Utilities::Utilities(){}
