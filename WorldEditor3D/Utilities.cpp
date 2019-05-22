#include "Utilities.h"
#include "MainApp.h"

#include <fstream>

#include <Engine/ResourceManager.h>
#include <JSON/json.hpp>
using json = nlohmann::json;


void Utilities::openMap(MainApp* app, const std::string& file){
	std::string path = app->m_gui.currentPath + file;

	app->resetData();

	std::ifstream in(path);
	json mapFile;
	in >> mapFile;

	GameObject* object;
	//gameobjects
	for(auto it = mapFile["gameobjects"].begin(); it != mapFile["gameobjects"].end(); ++it){
		json obj = it.value();
		std::string obj_name = obj["name"].get<std::string>();
		std::string obj_inEditorName = obj["inEditorName"].get<std::string>();
		physics::BodyType type = static_cast<physics::BodyType>(obj["bodyType"].get<int>());
		std::vector<float> p = obj["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = obj["rot"].get<std::vector<float>>();
		std::vector<float> s = obj["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);
		
		object = new GameObject(utilities::ResourceManager::loadModel(obj_name));
		object->setName(obj_name);
		object->setInEditorName(obj_inEditorName);
		object->setPosition(pos);
		object->setRotation(glm::quat(r[3], r[0], r[1], r[2]));
		object->setScale(scale);
		object->m_bodyType = type;
		object->m_type = obj["genericType"].get<int>();
		object->m_updateScript = obj["updateScript"].get<std::string>();

		app->m_objectsInScene.push_back(object);
		app->m_gameObjectsMap[object->getCode()] = object;
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
	app->m_lights.push_back(new renderer::DirLight(amb, diff, spec, dir));

	std::string billboard_file = "billboard_dirLight";
	LightBillboard* lightBillboard = new LightBillboard(utilities::ResourceManager::loadModel(billboard_file), app->m_lights[0]);
	lightBillboard->setPosition(static_cast<renderer::DirLight*>(app->m_lights[0])->direction);
	app->m_lightsBillboards.push_back(lightBillboard);
	app->m_lightsBillboardsMap[lightBillboard->getCode()] = lightBillboard;

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
	app->m_lights.push_back(new renderer::SpotLight(amb, diff, spec, dir, pos, att, cutOff, outCutOff));

	///pointlights
	billboard_file = "billboard_pointLight";
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
		app->m_lights.push_back(new renderer::PointLight(amb, diff, spec, pos, att));

		LightBillboard* lightBillboard = new LightBillboard(utilities::ResourceManager::loadModel(billboard_file), app->m_lights.back());
		lightBillboard->setPosition(static_cast<renderer::DirLight*>(app->m_lights.back())->direction);
		app->m_lightsBillboards.push_back(lightBillboard);
		app->m_lightsBillboardsMap[lightBillboard->getCode()] = lightBillboard;
	}

	//collision volumes
	CollisionVolumeBillboard* colVol;

	billboard_file = "billboard_colVolume";
	for(auto it = mapFile["collisionVolumes"].begin(); it != mapFile["collisionVolumes"].end(); ++it){
		json obj = it.value();
		std::string name = obj["inEditorName"].get<std::string>();
		int type = obj["type"].get<int>();
		int shape = obj["shape"].get<int>();
		std::vector<float> p = obj["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = obj["rot"].get<std::vector<float>>();
		glm::quat rot = glm::quat(r[3], r[0], r[1], r[2]);
		std::vector<float> s = obj["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);

		colVol = new CollisionVolumeBillboard(utilities::ResourceManager::loadModel(billboard_file), name);
		colVol->setPosition(pos);
		colVol->setVolumeType(static_cast<VolumeType>(type));
		colVol->triggerScript = obj["triggerScript"].get<std::string>();

		renderer::CollisionBody body;
		body.colRotQuat = rot;
		body.shape = shape;
		body.colScale = scale;
		body.colRelativePos = pos;
		colVol->setBody(body);

		app->m_colVolumeBillboards.push_back(colVol);
		app->m_colVolumeBillboardsMap[colVol->getCode()] = colVol;
	}

	auto& skybox = mapFile["skybox"];
	app->m_skybox.setEnabled(skybox["enabled"].get<bool>());
	app->m_skybox.setSkyboxTexturePath(renderer::Skybox::RIGHT,	 skybox["textures"]["right"].get<std::string>());
	app->m_skybox.setSkyboxTexturePath(renderer::Skybox::LEFT,	 skybox["textures"]["left"].get<std::string>());
	app->m_skybox.setSkyboxTexturePath(renderer::Skybox::BOTTOM, skybox["textures"]["bottom"].get<std::string>());
	app->m_skybox.setSkyboxTexturePath(renderer::Skybox::TOP,	 skybox["textures"]["top"].get<std::string>());
	app->m_skybox.setSkyboxTexturePath(renderer::Skybox::BACK,	 skybox["textures"]["back"].get<std::string>());
	app->m_skybox.setSkyboxTexturePath(renderer::Skybox::FRONT,	 skybox["textures"]["front"].get<std::string>());
	app->m_skybox.set();

}

void Utilities::saveMap(MainApp* app, const std::string& file){
	if(!checkIfMapCanBeSaved(app))
		return;

	std::string path = app->m_gui.currentPath + file;
	json map;
	map["gameobjects"] = {};
	map["lights"] = {};
	map["lights"]["pointLights"] = {};
	map["collisionVolumes"] = {};

	for(auto obj : app->m_objectsInScene){
		json entry = {
			{"name", obj->getName()},
			{"inEditorName", obj->getInEditorName()},
			{"pos", {obj->getPosition().x, obj->getPosition().y, obj->getPosition().z}},
			{"rot", {obj->getRotation().x, obj->getRotation().y, obj->getRotation().z, obj->getRotation().w}},
			{"scale", {obj->getScale().x, obj->getScale().y, obj->getScale().z}},
			{"bodyType", (int)obj->m_bodyType},
			{"genericType", obj->m_type},
			{"updateScript", obj->m_updateScript}
		};
		map["gameobjects"].push_back(entry);
	}

	renderer::DirLight* dl = static_cast<renderer::DirLight*>(app->m_lights[0]);
	map["lights"]["dirLight"] = {
		{"amb", {dl->ambient.x, dl->ambient.y, dl->ambient.z}},
		{"diff", {dl->diffuse.x, dl->diffuse.y, dl->diffuse.z}},
		{"spec", {dl->specular.x, dl->specular.y, dl->specular.z}},
		{"dir", {dl->direction.x, dl->direction.y, dl->direction.z}}
	};
	renderer::SpotLight* sl = static_cast<renderer::SpotLight*>(app->m_lights[1]);
	map["lights"]["spotLight"] = {
		{"amb", {sl->ambient.x, sl->ambient.y, sl->ambient.z}},
		{"diff", {sl->diffuse.x, sl->diffuse.y, sl->diffuse.z}},
		{"spec", {sl->specular.x, sl->specular.y, sl->specular.z}},
		{"dir", {sl->direction.x, sl->direction.y, sl->direction.z}},
		{"pos", {sl->position.x, sl->position.y, sl->position.z}},
		{"att", {sl->attenuation.x, sl->attenuation.y, sl->attenuation.z}},
		{"cutOff", sl->cutOff},
		{"outCutOff", sl->outerCutOff}
	};
	for(size_t index = 2; index < app->m_lights.size(); index++){
		renderer::PointLight* pl = static_cast<renderer::PointLight*>(app->m_lights[index]);
		json entry = {
			{"amb", {pl->ambient.x, pl->ambient.y, pl->ambient.z}},
			{"diff", {pl->diffuse.x, pl->diffuse.y, pl->diffuse.z}},
			{"spec", {pl->specular.x, pl->specular.y, pl->specular.z}},
			{"pos", {pl->position.x, pl->position.y, pl->position.z}},
			{"att", {pl->attenuation.x, pl->attenuation.y, pl->attenuation.z}},
		};
		map["lights"]["pointLights"].push_back(entry);
	}

	for(auto obj : app->m_colVolumeBillboards){
		auto& colBody = obj->getColBodyRef();
		json entry = {
			{"inEditorName", obj->getName()},
			{"type", obj->getVolumeType()},
			{"rot", {colBody.colRotQuat.x, colBody.colRotQuat.y, colBody.colRotQuat.z, colBody.colRotQuat.w}},
			{"scale", {colBody.colScale.x, colBody.colScale.y, colBody.colScale.z}},
			{"pos", {obj->getPosition().x, obj->getPosition().y, obj->getPosition().z}},
			{"shape", colBody.shape},
			{ "triggerScript", obj->triggerScript }
		};
		map["collisionVolumes"].push_back(entry);
	}

	map["skybox"] = {
		{"enabled", app->m_skybox.enabled()}, 
		{"textures", 
			{
				{"top", app->m_skybox.getSkyboxTexturePath(renderer::Skybox::TOP)},
				{"bottom", app->m_skybox.getSkyboxTexturePath(renderer::Skybox::BOTTOM)},
				{"left", app->m_skybox.getSkyboxTexturePath(renderer::Skybox::LEFT)},
				{"right", app->m_skybox.getSkyboxTexturePath(renderer::Skybox::RIGHT)},
				{"back", app->m_skybox.getSkyboxTexturePath(renderer::Skybox::BACK)},
				{"front", app->m_skybox.getSkyboxTexturePath(renderer::Skybox::FRONT)}
			}
		}
	};

	std::ofstream out(path);
	out << std::setw(4) << map << std::endl;
	out.close();
}

void Utilities::saveCreatedObject(MainApp* app, const std::string& buf){
	json entry = {
		{"diff", app->m_creationTabGameObject.getDiffName()},
		{"spec", app->m_creationTabGameObject.getSpecName()},
		{"mesh", app->m_creationTabGameObject.getMeshName()},
		{"billboard", app->m_creationTabGameObject.isBillboard()},
		{"gravity", app->m_creationTabGameObject.m_gravityEnabled},
		{"sleep", app->m_creationTabGameObject.m_allowedToSleep},
		{"bounciness", app->m_creationTabGameObject.m_bounciness},
		{"friction", app->m_creationTabGameObject.m_frictionCoefficient},
		{"rollingResist", app->m_creationTabGameObject.m_rollingResistance},
		{"linearDamp", app->m_creationTabGameObject.m_linearDamping},
		{"angularDamp", app->m_creationTabGameObject.m_angularDamping}
	};
	for(renderer::CollisionBody body : app->m_creationTabGameObject.getColBodies()){
		json col = {
			{"shape", body.shape},
			{"pos", {body.colRelativePos.x, body.colRelativePos.y, body.colRelativePos.z}},
			{"rot", {body.colRotQuat.x, body.colRotQuat.y, body.colRotQuat.z, body.colRotQuat.w}},
			{"scale", {body.colScale.x, body.colScale.y, body.colScale.z}},
			{"mass", body.mass}
		};
		entry["collision"].push_back(col);
	}

	std::string path = app->m_gui.currentPath + buf;
	std::ofstream out(path);
	out << std::setw(4) << entry << std::endl;
	out.close();
}

void Utilities::openCreatedObject(MainApp* app, const std::string& object){
	std::string path = app->m_gui.currentPath + object;
	std::ifstream in(path);
	json obj;
	in >> obj;
	in.close();

	app->m_creationTabGameObject.clearColBodies();
	app->m_gui.collisionBodies.clear();
	app->m_gui.collisionBodyEntryItem = -1;
	app->m_creationTabGameObject.setDiffName(obj["diff"].get<std::string>());
	app->m_creationTabGameObject.setSpecName(obj["spec"].get<std::string>());
	app->m_creationTabGameObject.setMeshName(obj["mesh"].get<std::string>());
	app->m_creationTabGameObject.setIsBillboard(obj["billboard"].get<bool>());
	app->m_creationTabGameObject.m_gravityEnabled = obj["gravity"].get<bool>();
	app->m_creationTabGameObject.m_allowedToSleep = obj["sleep"].get<bool>();
	app->m_creationTabGameObject.m_bounciness = obj["bounciness"].get<float>();
	app->m_creationTabGameObject.m_frictionCoefficient = obj["friction"].get<float>();
	app->m_creationTabGameObject.m_rollingResistance = obj["rollingResist"].get<float>();
	app->m_creationTabGameObject.m_linearDamping = obj["linearDamp"].get<float>();
	app->m_creationTabGameObject.m_angularDamping = obj["angularDamp"].get<float>();

	for(auto it = obj["collision"].begin(); it != obj["collision"].end(); it++){
		json col = it.value();
		renderer::CollisionBody body;
		body.shape = col["shape"].get<int>();
		body.mass = col["mass"].get<float>();
		std::vector<float> p = col["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = col["rot"].get<std::vector<float>>();
		std::vector<float> s = col["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);
		body.colRelativePos = pos;
		body.colRotQuat = glm::quat(r[3], r[0], r[1], r[2]);
		glm::vec3 rot = glm::eulerAngles(body.colRotQuat);
		body.colRotEuler = glm::vec3(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
		body.colScale = scale;
		app->m_creationTabGameObject.addColBody(body);
		app->m_gui.collisionBodies.push_back(body.shape);
	}

	app->m_creationTabGameObject.getTexturedModel()->setMesh(utilities::ResourceManager::getMesh("res/models/" + app->m_creationTabGameObject.getMeshName()));
	app->m_creationTabGameObject.getTexturedModel()->getMaterial().setDiffuseMap(utilities::ResourceManager::getTexture("res/textures/" + app->m_creationTabGameObject.getDiffName()));
	app->m_creationTabGameObject.getTexturedModel()->getMaterial().setSpecularMap(utilities::ResourceManager::getTexture("res/textures/" + app->m_creationTabGameObject.getSpecName()));
}

Utilities::Utilities(){}

bool Utilities::checkIfMapCanBeSaved(MainApp* app){
	unsigned int startPoint = 0;
	unsigned int endPoint   = 0;

	for(const auto& colVol : app->m_colVolumeBillboards){
		if(colVol->getVolumeType() == static_cast<int>(VolumeType::START))
			startPoint++;
		else if(colVol->getVolumeType() == static_cast<int>(VolumeType::END))
			endPoint++;
	}

	if(startPoint == 1u && endPoint == 1u) {
		return true;
	} else {
		app->m_gui.popupTitle = "Cannot save map!";
		app->m_gui.popupInfo = "Make sure you have 1 Starting point and 1 Ending point!";
		app->m_gui.b_showPopup = true;
	}
	return false;
}
