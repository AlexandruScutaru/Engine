#include "MainApp.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define MAX_FPS 120


MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_collapseEditor(false)
{
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

	m_fpsLimiter.setMaxFPS(MAX_FPS);
}

void MainApp::initLevel(){
	//game objects
	renderer::TexturedModel model;
	renderer::Material mat = renderer::Material(renderer::ResourceManager::getTexture("res/textures/character_DIFF.png"),
												renderer::ResourceManager::getTexture("res/textures/character_SPEC.png"));
	
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/character.obj"));
	m_entities.push_back(new renderer::Entity(renderer::ResourceManager::addTexturedModel(model)));
	m_entities.back()->setPosition(glm::vec3(0.0f));

	mat.setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/crate_DIFF.png"));
	mat.setSpecularMap(renderer::ResourceManager::getTexture("res/textures/crate_SPEC.png"));
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/quad.obj"));
	m_entities.push_back(new renderer::Entity(renderer::ResourceManager::addTexturedModel(model)));
	m_entities.back()->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));

	mat.setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/bulb_DIFF.png"));
	mat.setSpecularMap(nullptr);
	model.setMaterial(mat);
	model.setMesh(renderer::ResourceManager::getMesh("res/models/light_source.obj"));
	m_billboards.push_back(new renderer::BillBoard(renderer::ResourceManager::addTexturedModel(model)));
	m_billboards.back()->setPosition(m_dirLight.direction);

	//lights
	m_dirLight = renderer::DirLight(glm::vec3(2.0f, 1.0f, 0.0f), 
									glm::vec3(0.05f, 0.05f, 0.05f), 
									glm::vec3(0.4f, 0.4f, 0.4f), 
									glm::vec3(0.5f, 0.5f, 0.5f));
	m_spotLight = renderer::SpotLight(glm::vec3(0.0f, 0.0f, 0.0f),
									  glm::vec3(1.0f, 1.0f, 1.0f),
									  glm::vec3(1.0f, 1.0f, 1.0f),
									  glm::vec3(1.0f, 0.09f, 0.032f),
									  m_camera.getPos(),
									  m_camera.getFront(),
									  12.5f,
									  15.0f);	
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
		
		processInput();
		updateImGuiWindows();
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
			m_inputManager.setMouseCoords((float)e.motion.xrel, (float)e.motion.yrel);
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
}

void MainApp::update(float deltaTime){
	if(!ImGui::GetIO().WantCaptureKeyboard)
		m_camera.update(m_inputManager, deltaTime);

	///object rotation
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, 1.0f*deltaTime, glm::vec3(0.0, 1.0, 0.0));
	m_dirLight.direction = glm::vec3(rotationMat * glm::vec4(m_dirLight.direction, 1.0));
	m_billboards.back()->setPosition(m_dirLight.direction);

	//m_spotLight.position = m_camera.getPos();
	//m_spotLight.direction = m_camera.getFront();

}

void MainApp::updateImGuiWindows(){
	ImGui_ImplSdlGL3_NewFrame(m_window.getWindow());

	showEditorWindow();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

	m_masterRenderer.renderScene(m_entities, m_billboards, m_dirLight, m_pointLights, m_spotLight, m_camera);
	
	ImGui::Render();
	//sdl: swap buffers
	m_window.swapBuffer();
}

/// ImGui
void MainApp::showEditorWindow(){
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH-300.0f, 0.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, SCREEN_HEIGHT), ImGuiCond_Once);

	ImGui::Begin("editor", NULL);
	
	ImGui::Text("test");
	static char a[32];
	if(ImGui::InputText("testInput", a, 32)){

	}
	
	ImGui::End();
}
