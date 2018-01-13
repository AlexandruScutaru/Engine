#include "MainApp.h"

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

	m_fpsLimiter.setMaxFPS(MAX_FPS);
}

void MainApp::initLevel(){
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
		
		processInput();
		update(deltaTime);
		updateImGuiWindows();
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
			// Exit the app here!
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
	
}

void MainApp::updateImGuiWindows(){
	ImGui_ImplSdlGL3_NewFrame(m_window.getWindow());

	showEditorWindow();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	
	ImGui::End();
}
