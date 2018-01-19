#include "MainApp.h"

#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

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
	///game objects
	//set default data for the creatingEntiy;
	m_creatingModel = *(renderer::ResourceManager::getTexturedModelAt(0));
	m_creatingLight = renderer::DirLight(glm::vec3(0.05f, 0.05f, 0.05f),
										 glm::vec3(0.4f, 0.4f, 0.4f),
										 glm::vec3(0.5f, 0.5f, 0.5f),
										 glm::vec3(-2000.0f, 2000.0f, 2000.0f));

	//some objects to draw
	renderer::TexturedModel model;
	renderer::Material mat;
	renderer::GameObject* object;
	
	//character
	mat = renderer::Material(renderer::ResourceManager::getTexture("res/textures/character_DIFF.png"),
							 renderer::ResourceManager::getTexture("res/textures/character_SPEC.png"));
	
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/character.obj"));
	object = new renderer::GameObject(renderer::ResourceManager::addTexturedModel(model));
	//object->setScale(glm::vec3(10.0f));
	m_objects_ToDraw.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;

	//plane
	mat.setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/crate_DIFF.png"));
	mat.setSpecularMap(renderer::ResourceManager::getTexture("res/textures/crate_SPEC.png"));
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/quad.obj"));
	object = new renderer::GameObject(renderer::ResourceManager::addTexturedModel(model));
	m_objects_ToDraw.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;


	//directional light billboard
	mat.setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/billboard_dirLight.png"));
	mat.setSpecularMap(nullptr);
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/quad.obj"));
	object = new renderer::GameObject(renderer::ResourceManager::addTexturedModel(model), true);
	m_objects_ToDraw.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	m_lights.push_back(new renderer::DirLight(glm::vec3(0.05f, 0.05f, 0.05f),
											  glm::vec3(0.4f, 0.4f, 0.4f),
											  glm::vec3(0.5f, 0.5f, 0.5f),
											  glm::vec3(2000.0f, 1100.0f, 500.0f))
	);
	m_billboardLightsMap[object] = m_lights[0];
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
	m_lights.push_back(new renderer::SpotLight(glm::vec3(0.0f, 0.0f, 0.0f),
									  glm::vec3(1.0f, 1.0f, 1.0f),
									  glm::vec3(1.0f, 1.0f, 1.0f),
									  m_camera.getFront(),
									  m_camera.getPos(),
									  glm::vec3(1.0f, 0.09f, 0.032f),
									  glm::cos(glm::radians(13.0f)),
									  glm::cos(glm::radians(20.0f)))
	);
									  
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
		
		processInput();
		m_gui.updateImGuiWindows();
		update(deltaTime);
		drawGame();

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
	   m_gui.b_placementHeader &&
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
	//m_dirLight.direction = glm::vec3(rotationMat * glm::vec4(m_dirLight.direction, 1.0));
	//m_billboards.back()->setPosition(m_dirLight.direction);

	static_cast<renderer::SpotLight*>(m_lights[1])->position = m_camera.getPos();
	static_cast<renderer::SpotLight*>(m_lights[1])->direction = m_camera.getFront();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

	if(m_gui.b_creationHeader){
		m_masterRenderer.renderSingleEntity(&m_creatingModel, m_creatingLight, m_camera);
		m_masterRenderer.renderBoundingBox(renderer::ResourceManager::getTexturedModelAt(1), m_currentCreating.boxScale, m_currentCreating.boxRot, m_camera);
	}
	else if(m_gui.b_placementHeader){
		m_masterRenderer.renderScene(m_objects_ToDraw, m_lights, m_camera);
	}

	ImGui::Render();
	//sdl: swap buffers
	m_window.swapBuffer();
}

void MainApp::saveCreatedObject(char* buf){
	json entry = {
		{"name", m_currentCreating.name},
		{"diff", m_currentCreating.diff},
		{"spec", m_currentCreating.spec},
		{"mesh", m_currentCreating.mesh},
		{"boxScale", {m_currentCreating.boxScale.x, m_currentCreating.boxScale.y, m_currentCreating.boxScale.z}},
		{"boxRot",   {m_currentCreating.boxRot.x, m_currentCreating.boxRot.y, m_currentCreating.boxRot.z}}
	};
	std::string path = "res/gameobjects/" + std::string(buf);
	std::ofstream out(path);
	out << std::setw(4) << entry << std::endl;
}

void MainApp::renderToSelect(glm::vec2& coords){
	int val = m_masterRenderer.pixelPick(m_objects_ToDraw, m_camera, m_inputManager.getActualMouseCoords());
	std::cout << val << std::endl;
}
