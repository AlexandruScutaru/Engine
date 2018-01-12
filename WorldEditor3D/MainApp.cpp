#include "MainApp.h"

#define SCREEN_WIDTH 800	
#define SCREEN_HEIGHT 600


MainApp::MainApp():
	m_appState(AppState::EDIT)
{
	initSystems();
}

MainApp::~MainApp(){}


void MainApp::run(){
	initLevel();
	loop();
}

void MainApp::initSystems(){
	m_window.create("Renderer", SCREEN_WIDTH, SCREEN_HEIGHT/*, renderer::WindowFlags::BORDERLESS*/);
}

void MainApp::initLevel(){
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		processInput();
		update(0.0f);
		drawGame();
	}
}

void MainApp::processInput(){
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
		case SDL_QUIT:
			// Exit the app here!
			m_appState = AppState::EXIT;
			break;
		}
	}
}

void MainApp::update(float deltaTime){

}

void MainApp::drawGame(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//sdl: swap buffers
	m_window.swapBuffer();
}
