#include "Utilities.h"
#include "GameObject.h"
#include "CollisionVolume.h"
#include "TerrainShader.h"

#include <Engine/Lights.h>
#include <Engine/ResourceManager.h>
#include <Engine/PhysicsWorld.h>
#include <Engine/Skybox.h>
#include <Engine/Terrain.h>

#include <fstream>
#include <algorithm>
#include <JSON/json.hpp>
using json = nlohmann::json;


void Utilities::OpenMap(const std::string & file, std::vector<GameObject*>& objects, 
						std::vector<CollisionVolume*>& colVols, std::vector<renderer::Light*>& lights, 
						physics::PhysicsWorld* world, Player* player, renderer::Skybox& skybox, 
						renderer::Terrain& terrain, TerrainShader& terrainShader)
{
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
		object->setType(obj["genericType"].get<int>());
		object->setAtlasIndex(obj["atlasIndex"].get<int>());
		object->updateScript = obj["updateScript"].get<std::string>();
		objects.push_back(object);
	}

	//collision bodies
	CollisionVolume* colVol;
	for(auto it = mapFile["collisionVolumes"].begin(); it != mapFile["collisionVolumes"].end(); ++it) {
		json obj = it.value();
		int type = obj["type"].get<int>();
		int shape = obj["shape"].get<int>();
		std::vector<float> p = obj["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = obj["rot"].get<std::vector<float>>();
		glm::quat rot(r[3], r[0], r[1], r[2]);
		std::vector<float> s = obj["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);
		std::string script = obj["triggerScript"].get<std::string>();

		if(type == static_cast<int>(CollisionVolume::VolumeType::START)) { 
			//player start - contains data about player collision capsule too
			auto rotEuler = glm::eulerAngles(rot);
			rotEuler = glm::vec3(glm::degrees(rotEuler.x), glm::degrees(rotEuler.y), glm::degrees(rotEuler.z));
			player->setRotation(glm::vec3(0.0f, rotEuler.y, 0.0f));
			
			renderer::CollisionBody body(glm::vec3(0.0f), rot, rotEuler, scale, static_cast<int>(renderer::CollisionShapes::SHAPE_CAPSULE), 60.0f);
			auto physicsBody = world->createPhysicsBody(pos, rot);

			physicsBody->enableGravity(true);
			physicsBody->allowSleep(false);
			physicsBody->setBounciness(0.0f);
			physicsBody->setFrictionCoefficient(1.0f);
			physicsBody->setRollingResistance(1.0f);
			physicsBody->setLinearDamping(0.6f);
			//physicsBody->setAngularDamping(0.6f);
			physicsBody->setAngularDamping(1.0f);
			physicsBody->addCollisionShapes(glm::vec3(1.0f), std::vector<renderer::CollisionBody>{body}, physics::CollisionCategory::PLAYER);
			player->setPhysicsBody(physicsBody);
		} else {
			colVol = new CollisionVolume(pos);
			colVol->m_type = static_cast<CollisionVolume::VolumeType>(type);
			colVol->triggerScript = script;
			auto rotEuler = glm::eulerAngles(rot);
			rotEuler = glm::vec3(glm::degrees(rotEuler.x), glm::degrees(rotEuler.y), glm::degrees(rotEuler.z));

			renderer::CollisionBody body(glm::vec3(0.0f), rot, rotEuler, scale, shape, 1.0f);
			auto physicsBody = world->createPhysicsBody(pos, rot);

			physicsBody->addCollisionShapes(glm::vec3(1.0f), std::vector<renderer::CollisionBody>{body}, physics::CollisionCategory::TRIGGER);
			physicsBody->setBodyType(physics::BodyType::STATIC);
			physicsBody->setBounciness(0.0f);
			colVol->setPhysicsBody(physicsBody);

			colVols.push_back(colVol);
		}
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

	auto& skBox = mapFile["skybox"];
	skybox.setEnabled(skBox["enabled"].get<bool>());
	skybox.setSkyboxTexturePath(renderer::Skybox::RIGHT, skBox["textures"]["right"].get<std::string>());
	skybox.setSkyboxTexturePath(renderer::Skybox::LEFT, skBox["textures"]["left"].get<std::string>());
	skybox.setSkyboxTexturePath(renderer::Skybox::BOTTOM, skBox["textures"]["bottom"].get<std::string>());
	skybox.setSkyboxTexturePath(renderer::Skybox::TOP, skBox["textures"]["top"].get<std::string>());
	skybox.setSkyboxTexturePath(renderer::Skybox::BACK, skBox["textures"]["back"].get<std::string>());
	skybox.setSkyboxTexturePath(renderer::Skybox::FRONT, skBox["textures"]["front"].get<std::string>());
	skybox.set();

	
	auto& terrainData = mapFile["terrain"];
	if(terrainData["enabled"].get<bool>()){
		terrain.setEnabled(true);
		//textures
		terrain.setTerrainTexturePath(renderer::Terrain::HEIGHT, terrainData["textures"]["height"].get<std::string>());
		terrain.setTerrainTexturePath(renderer::Terrain::BLEND, terrainData["textures"]["blend"].get<std::string>());
		terrain.setTerrainTexturePath(renderer::Terrain::BASE, terrainData["textures"]["base"].get<std::string>());
		terrain.setTerrainTexturePath(renderer::Terrain::RED, terrainData["textures"]["red"].get<std::string>());
		terrain.setTerrainTexturePath(renderer::Terrain::GREEN, terrainData["textures"]["green"].get<std::string>());
		terrain.setTerrainTexturePath(renderer::Terrain::BLUE, terrainData["textures"]["blue"].get<std::string>());
		//sizing
		terrain.setHeightMultiplier(terrainData["sizing"]["multiplier"].get<float>());
		terrain.setSize(terrainData["sizing"]["length"].get<float>());
		terrainShader.getTilingFactorRef() = terrainData["sizing"]["tiling"].get<float>();
		//fog
		std::vector<float> color = terrainData["fog"]["fog_color"].get<std::vector<float>>();
		terrainShader.setFogColor(glm::vec3(color[0], color[1], color[2]));
		terrainShader.getFogDensityRef() = terrainData["fog"]["density"].get<float>();
		terrainShader.getFogGradientRef() = terrainData["fog"]["gradient"].get<float>();
		terrain.set();
	} else{
		//terrain not defined, continue without it
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
	object->setIsDoubleSided(obj["doubleSided"].get<bool>());
	object->getTexturedModel()->setAtlasSize(obj["atlasSize"].get<int>());
	auto physicsBody = world->createPhysicsBody(pos, rot);
	
	physicsBody->enableGravity(obj["gravity"].get<bool>());
	physicsBody->allowSleep(obj["sleep"].get<bool>());
	physicsBody->setBounciness(obj["bounciness"].get<float>());
	physicsBody->setFrictionCoefficient(obj["friction"].get<float>());
	physicsBody->setRollingResistance(obj["rollingResist"].get<float>());
	physicsBody->setLinearDamping(obj["linearDamp"].get<float>());
	physicsBody->setAngularDamping(obj["angularDamp"].get<float>());
	physicsBody->addCollisionShapes(scale, colBodies, physics::CollisionCategory::GENERAL);
	
	object->setPhysicsBody(physicsBody);

	return object;
}

Utilities::Utilities(){}
