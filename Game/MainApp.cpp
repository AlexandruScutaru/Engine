#include "MainApp.h"
#include "Config.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include "Utilities.h"
#include <algorithm>
#include <cmath>
#include <Engine/CollisionBody.h>

MainApp::MainApp():
	m_appState(AppState::EDIT)
{
	initSystems();
}

MainApp::~MainApp(){
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
	m_window.create(CONFIG.window_cfg.title, CONFIG.window_cfg.width, CONFIG.window_cfg.height, CONFIG.window_cfg.window_mode);
	renderer::Window::setMouseTrapping(CONFIG.window_cfg.trap_mouse ? SDL_TRUE : SDL_FALSE);
	renderer::Window::setVSync(CONFIG.vsync);
	m_fpsLimiter.setMaxFPS(CONFIG.max_graphics_fps);

	renderer::Renderer::Init();
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());
	utilities::ResourceManager::Init();
}

void MainApp::initLevel(){
	m_gameObjectsShader.initShader("res/shaders/entity");
	m_basicColorShader.initShader("res/shaders/basic");
	m_billboardShader.initShader("res/shaders/billboard");

	Utilities::OpenMap("res/maps/a", m_objectsInScene, m_lights, &m_dynamicWorld);
}

void MainApp::loop(){
	const float PHYSICS_STEP = 1.0f / (float)CONFIG.max_physics_fps;
	float accumulator = 0.0f;
	float interpolation = 0.0f;
	
	float crateSpeed = 0.5f;
	
	Uint32 physicsSteps = 0;
	Uint32 fps = 0;
	utilities::Timer fpsTimer;
	fpsTimer.start();

	float currentTime = SDL_GetTicks() / 1000.0f;
	while(m_appState == AppState::EDIT){
		m_fpsLimiter.begin();

		float newTime = SDL_GetTicks() / 1000.0f;
		float frameTime = newTime - currentTime;
		if(frameTime > 0.25f)
			frameTime = 0.25f;
		currentTime = newTime;
	
		//set object position to positon in physiscs world before update
		for(auto& obj : m_objectsInScene){
			obj->setPosition(obj->getPhysicsBody()->getPostion());
			obj->setRotation(obj->getPhysicsBody()->getRotation());
		}
		accumulator += frameTime;
		while(accumulator >= PHYSICS_STEP){
			processInput();
			update(PHYSICS_STEP);
			accumulator -= PHYSICS_STEP;
			physicsSteps++;
		}
	
		interpolation = accumulator / PHYSICS_STEP;

		drawGame(interpolation);
		m_window.swapBuffer();
	
		fps++;
		if(fpsTimer.getTicks() >= 1000){
			//std::cout << "Fps: " << fps << " | Physics Steps: " << physicsSteps << std::endl;
			fps = 0;
			physicsSteps = 0;
			fpsTimer.start();
		}
	
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
			if(e.key.keysym.sym == SDLK_ESCAPE)
				m_appState = AppState::EXIT;
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
	m_dynamicWorld.update(deltaTime);
	m_player.update(m_inputManager, deltaTime);

	static_cast<renderer::SpotLight*>(m_lights[1])->position = m_player.getCamera()->getPos();
	static_cast<renderer::SpotLight*>(m_lights[1])->direction = m_player.getCamera()->getFront();
}

void MainApp::drawGame(float interpolation){
	//interpolate the position from physics engine with the previous one
	for(auto& obj : m_objectsInScene){
		obj->setPosition(obj->getPhysicsBody()->getPostion() * interpolation + obj->getPosition() * (1.0f - interpolation));
		obj->setRotation(obj->getPhysicsBody()->getRotation() * interpolation + obj->getRotation() * (1.0f - interpolation));
	}
	updateToDrawVector();

	glViewport(0, 0, renderer::Window::getW(), renderer::Window::getH());
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	drawGameObjects();
}

void MainApp::resetData(){
	m_objects_ToDraw.clear();

	for(auto obj : m_objectsInScene)
		delete obj;
	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();
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
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
			modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());
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