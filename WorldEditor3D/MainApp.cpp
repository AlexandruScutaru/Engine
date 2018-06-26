#include "MainApp.h"

#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

//#define SCREEN_WIDTH 1366
//#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAX_FPS 120


MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_gui(this),
	m_currentlySelectedObject(nullptr)
{
	m_cameraPos = m_camera.getPos();
	m_cameraBck = m_camera.backupCameraProperties();
	initSystems();
}

MainApp::~MainApp(){
	ImGui_ImplSdlGL3_Shutdown();

	renderer::ResourceManager::ClearData();
}


void MainApp::run(){
	initLevel();
	loop();
}

void MainApp::initSystems(){
	m_window.create("Renderer", SCREEN_WIDTH, SCREEN_HEIGHT/*, renderer::WindowFlags::BORDERLESS*/);
	
	//Setup ImGui
	ImGui_ImplSdlGL3_Init(m_window.getWindow());
	ImGui::StyleColorsDark();

	m_masterRenderer.init();
	m_masterRenderer.setProjectionMatrix(m_camera);

	m_fpsLimiter.setMaxFPS(MAX_FPS);

	renderer::ResourceManager::Init();
}

void MainApp::initLevel(){
	/// lighting, game objects etc
	//set default data for the creatingEntiy object during the process of creating a new gameobject;
	
	m_creatingModel = *(renderer::ResourceManager::loadModel("default"));
	m_creatingLight = renderer::DirLight(glm::vec3(0.5f, 0.5f, 0.5f),
										 glm::vec3(0.4f, 0.4f, 0.4f),
										 glm::vec3(0.5f, 0.5f, 0.5f),
										 //glm::vec3(-2000.0f, 2000.0f, 2000.0f));
										 glm::vec3(0.0f, 0.0f, 0.0f));

	
	// initial lighting

	//some objects to draw
	//renderer::TexturedModel model;
	//renderer::Material mat;
	//renderer::GameObject* object;
	
	//character
	
	//mat = renderer::Material(renderer::ResourceManager::getTexture("res/textures/character_DIFF.png"),
	//						 renderer::ResourceManager::getTexture("res/textures/character_SPEC.png"));
	
	//model.setMaterial(mat);
	//model.setMesh(renderer::ResourceManager::getMesh("res/models/character.obj"));
	//object = new renderer::GameObject(renderer::ResourceManager::addTexturedModel(model));
	//object->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
	//object->setScale(glm::vec3(10.0f));
	//m_objects_ToDraw.push_back(object);
	/*
	//plane
	mat.setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/crate_DIFF.png"));
	mat.setSpecularMap(renderer::ResourceManager::getTexture("res/textures/crate_SPEC.png"));
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/quad.obj"));
	object = new renderer::GameObject(renderer::ResourceManager::addTexturedModel(model));
	m_objects_ToDraw.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;

	*/
	//directional light billboard
	//mat.setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/billboard_dirLight.png"));
	//mat.setSpecularMap(nullptr);
	////model.setMaterial(mat);
	//model.setMesh(renderer::ResourceManager::getMesh("res/models/quad.obj"));
	//object = new renderer::GameObject(renderer::ResourceManager::addTexturedModel(model), true, false);
	//m_objects_ToDraw.push_back(object);
	//m_gameObjectsMap[object->getCode()] = object;
	m_lights.push_back(new renderer::DirLight(glm::vec3(0.2f, 0.2f, 0.2f),
											  glm::vec3(0.4f, 0.4f, 0.4f),
											  glm::vec3(0.5f, 0.5f, 0.5f),
											  glm::vec3(0.0f, 0.0f, 0.0f))

	);
	//m_billboardLightsMap[object] = m_lights[0];
	/*
	//point light billboard
	mat.setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/billboard_pointLight.png"));
	mat.setSpecularMap(nullptr);
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/quad.obj"));
	m_billboard_PointLight = new renderer::GameObject(renderer::ResourceManager::addTexturedModel(model), true);

	///lights
	m_dirLight = renderer::DirLight(glm::vec3(0.05f, 0.05f, 0.05f), 
									glm::vec3(0.4f, 0.4f, 0.4f), 
									glm::vec3(0.5f, 0.5f, 0.5f),
									glm::vec3(2.0f, 1.0f, 0.0f));
	m_objects_ToDraw.push_back(m_billboard_DirectionalLight);
	*/
	m_lights.push_back(new renderer::SpotLight(glm::vec3(0.6f, 0.6f, 0.6f),
									  glm::vec3(1.0f, 1.0f, 1.0f),
									  glm::vec3(1.0f, 1.0f, 1.0f),
									  m_camera.getFront(),
									  m_camera.getPos(),
									  glm::vec3(1.0f, 0.09f, 0.032f),
									  glm::cos(glm::radians(15.0f)),
									  glm::cos(glm::radians(22.0f)))
	);

	/*for(int x = -3; x < 3; x++){
		for(int y = -3; y < 3; y++){
			for(int z = -3; z < 3; z++){
				renderer::GameObject* object;
				object = new renderer::GameObject(renderer::ResourceManager::loadModel("default"));
				object->setName("def");
				object->setPosition(glm::vec3(x,y,z));
				object->setScale(glm::vec3(0.25f));
				m_objectsInScene.push_back(object);
				m_gameObjectsMap[object->getCode()] = object;
				object = nullptr;
			}
		}
	}*/
									  
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
	
		processInput();
		update(deltaTime);
		m_gui.updateImGuiWindows();
		drawGame();
		glm::vec3 pos = m_camera.getPos();
		m_fpsLimiter.end();
	}
}

void MainApp::processInput(){
	m_inputManager.update();

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		ImGui_ImplSdlGL3_ProcessEvent(&e);
		switch(e.type) {
		case SDL_QUIT:
			m_appState = AppState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			m_inputManager.setRelMouseCoords((float)e.motion.xrel, (float)e.motion.yrel);
			m_inputManager.setActualMouseCoords((float)e.motion.x, (float)e.motion.y);
			break;
		case SDL_MOUSEWHEEL:
			m_inputManager.setMouseWheel(e.wheel.y);
			break;
		case SDL_KEYDOWN:
			m_inputManager.pressKey(e.key.keysym.sym);
			break;
		case SDL_KEYUP:
			m_inputManager.releaseKey(e.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_inputManager.pressKey(e.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			m_inputManager.releaseKey(e.button.button);
			break;
		}
	}

	if(m_inputManager.isKeyPressed(SDL_BUTTON_LEFT) &&
	   m_gui.b_placementTab &&
	   !ImGui::IsMouseHoveringAnyWindow()){
		renderToSelect(m_inputManager.getActualMouseCoords());
	}
}

void MainApp::update(float deltaTime){
	if(!ImGui::GetIO().WantCaptureKeyboard)
		m_camera.update(m_inputManager, deltaTime);

	///object rotation
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, 1.0f*deltaTime, glm::vec3(0.0, 1.0, 0.0));
	
	static_cast<renderer::SpotLight*>(m_lights[1])->position = m_camera.getPos();
	static_cast<renderer::SpotLight*>(m_lights[1])->direction = m_camera.getFront();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

	if(m_gui.b_creationTab){
		m_masterRenderer.renderSingleEntity(&m_creatingModel, m_creatingLight, m_camera);
		m_masterRenderer.renderCollisionBodies(m_currentCreating.colBodies, m_camera);
	}
	else if(m_gui.b_placementTab){
		m_masterRenderer.renderScene(m_objectsInScene, m_lights, m_camera);
	}

	ImGui::Render();
	//sdl: swap buffers
	m_window.swapBuffer();
}

void MainApp::resetData(){
	m_gui = GUI(this);
	m_currentlySelectedObject = nullptr;

	m_gameObjectsMap.clear();
	m_objects_ToDraw.clear();
	for(auto obj : m_objectsInScene)
		delete obj;
	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();

	renderer::ResourceManager::ClearData();

	m_creatingModel = *(renderer::ResourceManager::loadModel("default"));
	m_currentCreating = CreatedObject();
}

void MainApp::openMap(const std::string& file){
	std::string path = m_gui.currentPath + file;
	resetData();

	std::ifstream in(path);
	json mapFile;
	in >> mapFile;

	renderer::GameObject* object;
	//gameobjects
	for(auto it = mapFile["gameobjects"].begin(); it != mapFile["gameobjects"].end(); ++it){
		json obj = it.value();
		std::string obj_name = obj["name"].get<std::string>();
		std::string obj_inEditorName = obj["inEditorName"].get<std::string>();


		std::vector<float> p = obj["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = obj["rot"].get<std::vector<float>>();
		glm::vec3 rot = glm::vec3(r[0], r[1], r[2]);
		std::vector<float> s = obj["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);

		bool isStatic = obj["static"].get<bool>();

		object = new renderer::GameObject(renderer::ResourceManager::loadModel(obj_name));
		object->setName(obj_name);
		object->setInEditorName(obj_inEditorName);
		object->setPosition(pos);
		object->setRotation(rot);
		object->setScale(scale);
		object->setIsStatic(isStatic);

		m_objectsInScene.push_back(object);
		m_gameObjectsMap[object->getCode()] = object;
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
	m_lights.push_back(new renderer::DirLight(amb, diff, spec, dir));
	
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
	m_lights.push_back(new renderer::SpotLight(amb, diff, spec, dir, pos, att, cutOff, outCutOff));
	
	///pointlights
	for(auto it = mapFile["lights"]["pointLights"].begin(); it != mapFile["lights"]["pointLights"].end(); ++it){
		v = mapFile["lights"]["spotLight"]["amb"].get<std::vector<float>>();
		amb = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = mapFile["lights"]["spotLight"]["diff"].get<std::vector<float>>();
		diff = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = mapFile["lights"]["spotLight"]["spec"].get<std::vector<float>>();
		spec = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = mapFile["lights"]["spotLight"]["pos"].get<std::vector<float>>();
		pos = glm::vec3(v[0], v[1], v[2]);
		v.clear(), v = mapFile["lights"]["spotLight"]["att"].get<std::vector<float>>();
		att = glm::vec3(v[0], v[1], v[2]);
		m_lights.push_back(new renderer::PointLight(amb, diff, spec, pos, att));
	}
}

void MainApp::saveMap(const std::string& file){
	std::string path = m_gui.currentPath + file;
	json map;
	map["gameobjects"] = {};
	map["lights"] = {};
	map["lights"]["pointLights"] = {};
	
	for(auto obj : m_objectsInScene){
		json entry = {
			{"name", obj->getName()},
			{"inEditorName", obj->getInEditorName()},
			{"pos", {obj->getPosition().x, obj->getPosition().y, obj->getPosition().z}},
			{"rot", {obj->getRotation().x, obj->getRotation().y, obj->getRotation().z}},
			{"scale", {obj->getScale().x, obj->getScale().y, obj->getScale().z}},
			{"static", obj->isStatic()}
		};
		map["gameobjects"].push_back(entry);
	}
	
	renderer::DirLight* dl = static_cast<renderer::DirLight*>(m_lights[0]);
	map["lights"]["dirLight"] = {
		{"amb", {dl->ambient.x, dl->ambient.y, dl->ambient.z}},
		{"diff", {dl->diffuse.x, dl->diffuse.y, dl->diffuse.z}},
		{"spec", {dl->specular.x, dl->specular.y, dl->specular.z}},
		{"dir", {dl->direction.x, dl->direction.y, dl->direction.z}}
	};
	renderer::SpotLight* sl = static_cast<renderer::SpotLight*>(m_lights[1]);
	//l->
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
	for(size_t index = 2; index < m_lights.size(); index++){
		renderer::PointLight* pl = static_cast<renderer::PointLight*>(m_lights[index]);
		json entry = {
			{"amb",		{pl->ambient.x,		pl->ambient.y,		pl->ambient.z}},
			{"diff",	{pl->diffuse.x,		pl->diffuse.y,		pl->diffuse.z}},
			{"spec",	{pl->specular.x,	pl->specular.y,		pl->specular.z}},
			{"pos",		{pl->position.x,	pl->position.y,		pl->position.z}},
			{"att",		{pl->attenuation.x, pl->attenuation.y,	pl->attenuation.z}},
		};
		map["lights"]["pointLights"].push_back(entry);
	}
	std::ofstream out(path);
	out << std::setw(4) << map << std::endl;
	out.close();
}

void MainApp::saveCreatedObject(char* buf){
	json entry = {
		{"diff", m_currentCreating.diff},
		{"spec", m_currentCreating.spec},
		{"mesh", m_currentCreating.mesh},
		{"billboard", m_currentCreating.isBillboard}
	};
	for(renderer::CollisionBody body : m_currentCreating.colBodies){
		json col = {
			{"shape", body.shape},
			{"pos", {body.colRelativePos.x, body.colRelativePos.y, body.colRelativePos.z}},
			{"rot", {body.colRot.x, body.colRot.y, body.colRot.z}},
			{"scale", {body.colScale.x, body.colScale.y, body.colScale.z}}
		};
		entry["collision"].push_back(col);
	}

	std::string path = m_gui.currentPath + std::string(buf);
	std::ofstream out(path);
	out << std::setw(4) << entry << std::endl;
	out.close();
}

void MainApp::openCreatedObject(const std::string& object){
	std::string path = m_gui.currentPath + object;
	std::ifstream in(path);
	json obj;
	in >> obj;
	m_currentCreating = CreatedObject();
	m_currentCreating.diff = obj["diff"].get<std::string>();
	m_currentCreating.spec = obj["spec"].get<std::string>();
	m_currentCreating.mesh = obj["mesh"].get<std::string>();
	m_currentCreating.isBillboard = obj["billboard"].get<bool>();
	for(auto it = obj["collision"].begin(); it != obj["collision"].end(); it++){
		json col = it.value();
		renderer::CollisionBody body;
		body.shape = col["shape"].get<int>();
		std::vector<float> p = col["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = col["rot"].get<std::vector<float>>();
		glm::vec3 rot = glm::vec3(r[0], r[1], r[2]);
		std::vector<float> s = col["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);
		body.colRelativePos = pos;
		body.colRot = rot;
		body.colScale = scale;
		m_currentCreating.colBodies.push_back(body);
		m_gui.collisionBodies.push_back(body.shape);
	}
	in.close();

	m_creatingModel.setMesh(renderer::ResourceManager::getMesh("res/models/" + m_currentCreating.mesh));
	m_creatingModel.getMaterial().setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/" + m_currentCreating.diff));
	m_creatingModel.getMaterial().setSpecularMap(renderer::ResourceManager::getTexture("res/textures/" + m_currentCreating.spec));
}

void MainApp::renderToSelect(glm::vec2& coords){
	int val = m_masterRenderer.pixelPick(m_objectsInScene, m_camera, m_inputManager.getActualMouseCoords());
	if(m_currentlySelectedObject)
		m_currentlySelectedObject->setSelected(false);
	if(val){
		renderer::GameObject* obj = m_gameObjectsMap[val];
		m_currentlySelectedObject = obj;
		m_currentlySelectedObject->setSelected(true);
		for(size_t i = 0; i < m_objectsInScene.size(); ++i){
			if(m_objectsInScene[i] == obj){
				m_gui.placeGameobjectEntryItem = i;
			}
		}
	} else{
		m_currentlySelectedObject = nullptr;
		m_gui.placeGameobjectEntryItem = -1;
	}
}

void MainApp::addNewObject(const std::string & file){
	renderer::GameObject* object;
	object = new renderer::GameObject(renderer::ResourceManager::loadModel(file));
	object->setName(file);
	object->setInEditorName(file);

	glm::vec3 pos = m_camera.getPos() + m_camera.getFront() * 4.0f;
	object->setPosition(pos);
	m_objectsInScene.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	
	if(m_currentlySelectedObject)
		m_currentlySelectedObject->setSelected(false);
	m_currentlySelectedObject = object;
	m_currentlySelectedObject->setSelected(true);
	object = nullptr;
}

void MainApp::removeSelectedObject(int index){
	m_objectsInScene.erase(m_objectsInScene.begin() + index);
	m_currentlySelectedObject = nullptr;
}

void MainApp::duplicateSelectedObject(int index){
	renderer::GameObject* object;
	object = new renderer::GameObject(*m_objectsInScene[index]);
	m_objectsInScene.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	if(m_currentlySelectedObject)
		m_currentlySelectedObject->setSelected(false);
	m_currentlySelectedObject = object;
	m_currentlySelectedObject->setSelected(true);
	object = nullptr;
}
