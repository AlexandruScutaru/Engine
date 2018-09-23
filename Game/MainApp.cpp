#include "MainApp.h"

//#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include "Utilities.h"
#include <algorithm>


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define WINDOW_TITLE "Game"
#define MAX_FPS 120

///TODO:
	//investigate object rename issue

MainApp::MainApp():
	m_appState(AppState::EDIT)
{
	initSystems();
}

MainApp::~MainApp(){
	m_gameObjectsMap.clear();
	m_objects_ToDraw.clear();
	for(auto obj : m_objectsInScene)
		delete obj;
	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();

	utilities::ResourceManager::ClearData();
}


void MainApp::run(){
	initLevel();
	loop();
}

void MainApp::initSystems(){
	m_window.create(WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, renderer::WindowFlags::RESIZABLE);
	
	renderer::Renderer::Init();
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());
	utilities::ResourceManager::Init();

	m_fpsLimiter.setMaxFPS(MAX_FPS);
}

void MainApp::initLevel(){
	m_gameObjectsShader.initShader("res/shaders/entity");
	m_basicColorShader.initShader("res/shaders/basic");
	m_billboardShader.initShader("res/shaders/billboard");

	openMap("res/maps/map");
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
		processInput();
		
		update(deltaTime);

		drawGame();
		
		//sdl: swap buffers
		m_window.swapBuffer();
		m_fpsLimiter.end();
	}
}

void MainApp::processInput(){
	m_inputManager.update();

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
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
		case SDL_WINDOWEVENT:
			switch(e.window.event){
			case SDL_WINDOWEVENT_RESIZED:
				renderer::Window::setW(e.window.data1);
				renderer::Window::setH(e.window.data2);
				break;
			}
		}
	}
}

void MainApp::update(float deltaTime){
	m_player.update(m_inputManager, deltaTime);

	static_cast<renderer::SpotLight*>(m_lights[1])->position = m_player.getCamera()->getPos();
	static_cast<renderer::SpotLight*>(m_lights[1])->direction = m_player.getCamera()->getFront();

	updateToDrawVector();
}

void MainApp::drawGame(){
	glViewport(0, 0, renderer::Window::getW(), renderer::Window::getH());
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	drawGameObjects();
}

void MainApp::resetData(){
	m_gameObjectsMap.clear();
	m_objects_ToDraw.clear();

	for(auto obj : m_objectsInScene)
		delete obj;
	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();
}

void MainApp::openMap(const std::string& file){
	resetData();

	std::ifstream in(file);
	json mapFile;
	in >> mapFile;

	GameObject* object;
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

		object = new GameObject(utilities::ResourceManager::loadModel(obj_name));
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
		m_lights.push_back(new renderer::PointLight(amb, diff, spec, pos, att));
	}
}

void MainApp::saveMap(const std::string& file){
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
	std::ofstream out(file);
	out << std::setw(4) << map << std::endl;
	out.close();
}

void MainApp::updateToDrawVector(){
	m_objects_ToDraw.clear();	
	for(const auto& gameObject : m_objectsInScene)
		m_objects_ToDraw.push_back(gameObject);
}

void MainApp::drawGameObjects(){
	std::vector<renderer::CollisionBody*> colBodies;
	///first draw the normal gameobjects
	//prepare shader
	m_gameObjectsShader.use();
	m_gameObjectsShader.loadFlashlight(m_player.isFlashLightOn());
	
	m_gameObjectsShader.loadLights(m_lights);
	glm::mat4 view = m_player.getCamera()->getViewMatrix();
	m_gameObjectsShader.loadViewPosition(m_player.getCamera()->getPos());
	m_gameObjectsShader.loadViewMatrix(view);
	m_gameObjectsShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	//get textured model batches
	auto batches = Utilities::BatchRenderables<GameObject>(m_objects_ToDraw);
	for(auto const& batch : batches){
		if(batch.first->isBillboard())
			continue;
		renderer::Renderer::BindTexturedModel(batch.first);

		for(auto const& gameObject : batch.second){
			m_gameObjectsShader.loadSelected(gameObject->isSelected());
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
			modelMatrix = glm::rotate(modelMatrix, gameObject->getRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, gameObject->getRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, gameObject->getRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
			modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
			m_gameObjectsShader.loadModelMatrix(modelMatrix);
			
			renderer::Renderer::DrawTexturedModel(batch.first);
		}
	}
	m_gameObjectsShader.unuse();

	///now draw the billboards
	//prepare shader
	m_billboardShader.use();

	m_billboardShader.loadViewMatrix(view);
	m_billboardShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	for(auto const& batch : batches){
		if(batch.first->isBillboard()){
			renderer::Renderer::BindTexturedModel(batch.first);
			for(auto const& gameObject : batch.second){
				m_billboardShader.loadSelected(gameObject->isSelected());
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
				modelMatrix[0][0] = view[0][0];
				modelMatrix[0][1] = view[1][0];
				modelMatrix[0][2] = view[2][0];
				modelMatrix[1][0] = view[0][1];
				modelMatrix[1][1] = view[1][1];
				modelMatrix[1][2] = view[2][1];
				modelMatrix[2][0] = view[0][2];
				modelMatrix[2][1] = view[1][2];
				modelMatrix[2][2] = view[2][2];
				modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
				m_billboardShader.loadModelMatrix(modelMatrix);

				renderer::Renderer::DrawTexturedModel(batch.first);
			}
		}
	}
	m_billboardShader.unuse();
}