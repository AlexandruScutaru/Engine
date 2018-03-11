#include "MainApp.h"

#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

//#define SCREEN_WIDTH 1366
//#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576

#define MAX_FPS 120


MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_gui(this)
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
	m_creatingLight = renderer::DirLight(glm::vec3(0.05f, 0.05f, 0.05f),
										 glm::vec3(0.4f, 0.4f, 0.4f),
										 glm::vec3(0.5f, 0.5f, 0.5f),
										 glm::vec3(-2000.0f, 2000.0f, 2000.0f));
	
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

	//glm::vec3 pos = m_camera.getPos();// +m_camera.getFront() * 4.0f;
	//printf("%.5f %.5f %.5f\n", pos.x, pos.y, pos.z);

	///object rotation
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, 1.0f*deltaTime, glm::vec3(0.0, 1.0, 0.0));
	//m_dirLight.direction = glm::vec3(rotationMat * glm::vec4(m_dirLight.direction, 1.0));
	//m_billboards.back()->setPosition(m_dirLight.direction);

	static_cast<renderer::SpotLight*>(m_lights[1])->position = m_camera.getPos();
	static_cast<renderer::SpotLight*>(m_lights[1])->direction = m_camera.getFront();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

	if(m_gui.b_creationTab){
		m_masterRenderer.renderSingleEntity(&m_creatingModel, m_creatingLight, m_camera);
		m_masterRenderer.renderBoundingBox(renderer::ResourceManager::loadModel("cube"), 
										   m_currentCreating.boxScale, m_currentCreating.boxRot, m_camera);
	}
	else if(m_gui.b_placementTab){
		m_masterRenderer.renderScene(m_objectsInScene, m_lights, m_camera);
	}

	ImGui::Render();
	//sdl: swap buffers
	m_window.swapBuffer();
}

void MainApp::saveCreatedObject(char* buf){
	json entry = {
		{"diff", m_currentCreating.diff},
		{"spec", m_currentCreating.spec},
		{"mesh", m_currentCreating.mesh},
		{"billboard", m_currentCreating.isBillboard},
		{"boxScale", {m_currentCreating.boxScale.x, m_currentCreating.boxScale.y, m_currentCreating.boxScale.z}},
		{"boxRot",   {m_currentCreating.boxRot.x, m_currentCreating.boxRot.y, m_currentCreating.boxRot.z}},
		{"boxPos", {m_currentCreating.boxRot.x, m_currentCreating.boxRot.y, m_currentCreating.boxRot.z}},
	};
	std::string path = m_gui.currentPath + std::string(buf);
	std::ofstream out(path);
	out << std::setw(4) << entry << std::endl;
}

void MainApp::renderToSelect(glm::vec2& coords){
	int val = m_masterRenderer.pixelPick(m_objectsInScene, m_camera, m_inputManager.getActualMouseCoords());
	renderer::GameObject* obj = m_gameObjectsMap[val];
	for(size_t i = 0; i < m_objectsInScene.size(); ++i){
		if(m_objectsInScene[i] == obj){
			m_gui.placeGameobjectEntryItem = i;
		}
	}
}

void MainApp::addNewObject(const std::string & file){
	renderer::GameObject* object;
	object = new renderer::GameObject(renderer::ResourceManager::loadModel(file));
	object->setName(file);
	glm::vec3 pos = m_camera.getPos() + m_camera.getFront() * 4.0f;
	object->setPosition(pos);
	m_objectsInScene.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	object = nullptr;
}

void MainApp::removeSelectedObject(int index){
	m_objectsInScene.erase(m_objectsInScene.begin() + index);
}

void MainApp::duplicateSelectedObject(int index){
	renderer::GameObject* object;
	object = new renderer::GameObject(*m_objectsInScene[index]);
	m_objectsInScene.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	object = nullptr;
}
