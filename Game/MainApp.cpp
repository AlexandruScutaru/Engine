#include "MainApp.h"
#include "Config.h"
#include "Logger.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include "Utilities.h"
#include <algorithm>
#include <cmath>
#include <Engine/CollisionBody.h>

#define NEAR_PLANE 0.1f
#define FAR_PLANE 400.0f


float getDistance(const glm::vec3& e1, const glm::vec3& e2);

bool MainApp::m_resetLevel = false;

MainApp::MainApp() :
	m_appState(AppState::EDIT),
	m_eventListener(physics::PhysicsEventListener(std::bind(&MainApp::beginContact, this, std::placeholders::_1)))
{
	LOG_INFO("Game is starting!");
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

	lua_close(L);
}


void MainApp::run(){
	initLevel();
	loop();
}

void MainApp::beginContact(const rp3d::ContactPointInfo& contact) {
	rp3d::CollisionBody* body1 = contact.shape1->getBody();
	rp3d::CollisionBody* body2 = contact.shape2->getBody();
	rp3d::CollisionBody* other = nullptr;
	rp3d::RigidBody* playerBody = m_player.getPhysicsBody()->m_body;
	
	if(body1 != playerBody && body2 != playerBody)
		return;
	
	if(body1 == playerBody)
		other = body2;
	else
		other = body1;
	
	//check if we collided with a collision volume
	for(auto& colVol : m_collisionVolumes) {
		if(other == colVol->getPhysicsBody()->m_body) {
			if(colVol->m_type == CollisionVolume::VolumeType::END) {
				if(m_player.hasKey){
					std::cout << "finish" << std::endl;
					m_appState = AppState::EXIT;
				} else {
					std::cout << "find the key first" << std::endl;
				}
			} else if(colVol->m_type == CollisionVolume::VolumeType::TRIGGER){
				if(!colVol->triggerScript.empty()){
					luaL_dostring(L, colVol->triggerScript.c_str());
					lua_pcall(L, 0, 0, 0);
				}
			}
		}
	}

	//check if we collided with a special gameobject
	for(size_t i = 0u; i < m_objectsInScene.size(); i++){
		if(other == m_objectsInScene[i]->getPhysicsBody()->m_body) {
			if(m_objectsInScene[i]->getType() == GameObject::TYPE::KEY_PICKUP) {
				std::cout << "picked up key" << std::endl;
				m_objectsInScene.erase(m_objectsInScene.begin() + i--);
				m_player.hasKey = true;
			}
		}
	}
}

void MainApp::initSystems(){
	m_window.create(CONFIG.window_cfg.title, CONFIG.window_cfg.width, CONFIG.window_cfg.height, CONFIG.window_cfg.window_mode);
	renderer::Window::setMouseTrapping(CONFIG.window_cfg.trap_mouse ? SDL_TRUE : SDL_FALSE);
	renderer::Window::setVSync(CONFIG.vsync);
	m_fpsLimiter.setMaxFPS(CONFIG.max_graphics_fps);

	renderer::Renderer::Init();
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH(), NEAR_PLANE, FAR_PLANE);
	utilities::ResourceManager::Init();
	m_audioManager.init();

	L = luaL_newstate();
	luaL_openlibs(L);

	// Register classes
	luabridge::getGlobalNamespace(L)
		.beginClass<glm::vec3>("vec3")
			.addData<float>("x", &glm::vec3::x)
			.addData<float>("y", &glm::vec3::y)
			.addData<float>("z", &glm::vec3::z)
		.endClass();
	
	luabridge::getGlobalNamespace(L)
		.beginClass<GameObject>("GameObject")
			.addFunction("getPos", &GameObject::getPosition)
			.addFunction("setPos", &GameObject::setPosition)
		.endClass();

	luabridge::getGlobalNamespace(L)
		.beginClass<MainApp>("MainApp")
			.addStaticFunction("resetLevel", &MainApp::setResetLevel)
		.endClass();

	m_player.LOOK_SENSITIVITY = CONFIG.look_sensitivity;
}

void MainApp::initLevel(){
	m_gameObjectsShader.initShader("res/shaders/entity");
	m_basicColorShader.initShader("res/shaders/basic");
	m_billboardShader.initShader("res/shaders/billboard");
	m_terrainShader.initShader("res/shaders/terrain");
	m_skybox.init(399.0f);
	m_terrain.init(&m_terrainShader);

	Utilities::OpenMap("res/maps/" + CONFIG.map, m_objectsInScene, m_collisionVolumes, 
					   m_lights, &m_dynamicWorld, &m_player, m_skybox, m_terrain,
					   m_terrainShader);
	m_dynamicWorld.setEventListener(&m_eventListener);

	audio::Music music = m_audioManager.loadMusic("res/sounds/atmosphere.mp3");
	music.play(-1);
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

	m_player.getPhysicsBody()->m_body->setLinearVelocity(rp3d::Vector3(0.0f, 0.0f, 0.0f));
	m_player.getPhysicsBody()->m_body->setAngularVelocity(rp3d::Vector3(0.0f, 0.0f, 0.0f));


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
			obj->setPosition(obj->getPhysicsBody()->getPosition());
			obj->setRotation(obj->getPhysicsBody()->getRotation());
		}
		m_player.setPosition(m_player.getPhysicsBody()->getPosition());

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
		if(m_resetLevel) resetLevel();
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


	for(auto& obj : m_objectsInScene){
		if(!obj->updateScript.empty()){
			luaL_dostring(L, obj->updateScript.c_str());
			lua_pcall(L, 0, 0, 0);

			// lookup script function in global table
			luabridge::LuaRef updateFunc = luabridge::getGlobal(L, "update");

			if(updateFunc.isFunction()){
				try{
					updateFunc(&obj, SDL_GetTicks() / 1000.0f);
				} catch(luabridge::LuaException& ex){
					std::cout << ex.what() << std::endl;
				}
			}
		}
	}

	std::sort(m_lights.begin() + 2, m_lights.end(), [this](const auto& lhs, const auto& rhs) -> bool{
		glm::vec3 playerPos = m_player.getPosition();
		glm::vec3 plLPos = static_cast<renderer::PointLight*>(lhs)->position;
		glm::vec3 plRPos = static_cast<renderer::PointLight*>(rhs)->position;

		return getDistance(playerPos, static_cast<renderer::PointLight*>(lhs)->position) <
			   getDistance(playerPos, static_cast<renderer::PointLight*>(rhs)->position);

	});
}

void MainApp::drawGame(float interpolation){
	//interpolate the position from physics engine with the previous one
	for(auto& obj : m_objectsInScene){
		obj->setPosition(obj->getPhysicsBody()->getPosition() * interpolation + obj->getPosition() * (1.0f - interpolation));
		obj->setRotation(obj->getPhysicsBody()->getRotation() * interpolation + obj->getRotation() * (1.0f - interpolation));
	}
	
	m_player.setPosition(m_player.getPhysicsBody()->getPosition() * interpolation + m_player.getPosition() * (1.0f - interpolation));
	//auto rotEuler = glm::eulerAngles(m_player.getPhysicsBody()->getRotation());
	//rotEuler = glm::vec3(glm::degrees(rotEuler.x), glm::degrees(rotEuler.y), glm::degrees(rotEuler.z));
	//m_player.setRotation(rotEuler * interpolation + m_player.getRotation() * (1.0f - interpolation));

	updateToDrawVector();

	glViewport(0, 0, renderer::Window::getW(), renderer::Window::getH());
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH(), NEAR_PLANE, FAR_PLANE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(m_skybox.enabled())
		m_skybox.render(m_player.getCamera()->getViewMatrix(), renderer::Renderer::GetProjectionMatrix());
	if(m_terrain.enabled())
		m_terrain.render(5,
						 m_player.isFlashLightOn(),
						 &m_player.getCamera()->getPos(),
						 &m_player.getCamera()->getViewMatrix(),
						 &renderer::Renderer::GetProjectionMatrix(),
						 &m_lights
		);
	
	drawGameObjects();
}

void MainApp::resetData(){
	m_objects_ToDraw.clear();

	for(auto obj : m_objectsInScene){
		//m_dynamicWorld.destroyBody(obj->getPhysicsBody());
		delete obj;
	}

	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();
	for(auto colVol : m_collisionVolumes)
		delete colVol;
	m_collisionVolumes.clear();
}

void MainApp::resetLevel(){
	resetData();
	m_player.reset();
	initLevel();
	m_resetLevel = false;
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
		if(batch.first->isDoubleSided())
			renderer::Renderer::DisableBackFaceCulling();
		m_gameObjectsShader.loadAtlasSize(batch.first->getAtlasSize());
		renderer::Renderer::BindTexturedModel(batch.first);
		for(auto const& gameObject : batch.second){
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, gameObject->getPosition());
			modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());
			modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
			m_gameObjectsShader.loadModelMatrix(modelMatrix);
			m_gameObjectsShader.loadAtlasOffset(gameObject->getTextureOffset());
			renderer::Renderer::DrawTexturedModel(batch.first);
		}
		renderer::Renderer::EnableBackFaceCulling();
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

float getDistance(const glm::vec3 & e1, const glm::vec3 & e2){
	return std::pow(e1.x - e2.x, 2) + std::pow(e1.y - e2.y, 2) + std::pow(e1.z - e2.z, 2);
}
