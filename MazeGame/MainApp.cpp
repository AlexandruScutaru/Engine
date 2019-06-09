#include "MainApp.h"
#include "Config.h"
#include "Logger.h"
#include "Utilities.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <Engine/CollisionBody.h>
#include <thread>

float getDistance(const glm::vec3& e1, const glm::vec3& e2);
glm::vec3 genPos(int maxW, int maxH, float height);

bool MainApp::m_resetLevel = false;

#define MAX_COINS 9
#define NEAR_PLANE 0.1f
#define FAR_PLANE 40.0f 


MainApp::MainApp() :
	m_appState(AppState::RUNNING),
	m_eventListener(physics::PhysicsEventListener(std::bind(&MainApp::beginContact, this, std::placeholders::_1)))
{
	LOG_INFO("Game is starting!");
	initSystems();
}

MainApp::~MainApp(){
	for(auto obj : m_objectsInScene)
		delete obj;
	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();

	utilities::ResourceManager::ClearData();

	delete m_maze;
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
		colWithEnd = false;
		if(other == colVol->getPhysicsBody()->m_body) {
			if(colVol->m_type == CollisionVolume::VolumeType::END) {
				colWithEnd = true;
			}
		}
	}

	//check if we collided with a special gameobject
	for(size_t i = 0u; i < m_objectsInScene.size(); i++){
		if(other == m_objectsInScene[i]->getPhysicsBody()->m_body) {
			if(m_objectsInScene[i]->getType() == GameObject::TYPE::KEY_PICKUP) {
				delete m_objectsInScene[i];
				m_objectsInScene.erase(m_objectsInScene.begin() + i--);
				m_player.hasKey = true;
				m_hudElements[HUD_ELements::KEY].enabled = true;
				delete m_lights.back();
				m_lights.pop_back();
				audio::SoundEffect sound = m_audioManager.loadSountEffect("res/sounds/key_pickup.mp3");
				sound.play();
			}else if(m_objectsInScene[i]->getType() == GameObject::TYPE::COIN_PICKUP) {
				delete m_objectsInScene[i];
				m_objectsInScene.erase(m_objectsInScene.begin() + i--);
				m_player.coins++;
				audio::SoundEffect sound = m_audioManager.loadSountEffect("res/sounds/coin_pickup.mp3");
				sound.play();
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
	m_hud.initHUD();

	m_player.LOOK_SENSITIVITY = CONFIG.look_sensitivity;

	L = luaL_newstate();
	luaL_openlibs(L);

	// Register classes
	luabridge::getGlobalNamespace(L)
		.beginClass<GameObject>("GameObject")
			.addFunction("setRotY", &GameObject::setRotY)
			.addFunction("getRotY", &GameObject::getRotY)
		.endClass();

	srand(time(NULL));
}

void MainApp::initLevel(){
	m_hudElements.resize(HUD_ELements::NUM_ELEMENTS);
	m_hudElements[HUD_ELements::COIN] = renderer::HudElement{
		utilities::ResourceManager::getTexture("res/textures/coins_icon.png"), 
		glm::vec3(-0.916f, 0.9f, 0.0f), glm::vec3(0.08f), true};
	m_hudElements[HUD_ELements::KEY] = renderer::HudElement{
		utilities::ResourceManager::getTexture("res/textures/key_icon.png"), 
		glm::vec3(0.916f, 0.9f, 0.0f), glm::vec3(0.07f), false};

	m_gameObjectsShader.initShader("res/shaders/entity");
	m_dynamicWorld.setEventListener(&m_eventListener);
	m_skybox.init(99.0f);
	//generate maze
	m_maze = new Maze(CONFIG.mazeWidth, CONFIG.mazeHeight);
	std::cout << *m_maze;
	auto data = m_maze->getMaze();
	//set start, end and key position in maze
	glm::vec3 start = genPos(data[0].size(), data.size(), 1.6f);
	glm::vec3 end = genPos(data[0].size(), data.size(), 0.0f);
	glm::vec3 key = genPos(data[0].size(), data.size(), 1.28f);

	//add gameobject based on maze
	for(size_t i = 0; i < data.size(); i++){
		for(size_t j = 0; j < data[i].size(); j++){
			m_objectsInScene.push_back(Utilities::OpenGameObject("mod_concrete_floor", glm::vec3(2.24f*i, 0.0f, 2.24f*j), glm::quat(), glm::vec3(0.875f, 1.0f, 0.875f), &m_dynamicWorld));
			m_objectsInScene.push_back(Utilities::OpenGameObject("mod_brick_pillar_thin", glm::vec3(2.24f*i - 1.12f + 0.16f, 1.28f/2.0f, 2.24f*j - 1.12f + 0.16f), glm::quat(), glm::vec3(1.0f, 1.0f, 1.0f), &m_dynamicWorld));
			switch(data[i][j]){
				case Maze::Cells::F:
					std::cout << "F ";
					m_objectsInScene.push_back(Utilities::OpenGameObject("mod_brick_wall", glm::vec3(2.24f*i - 1.12f + 0.16f, 1.28f / 2.0f, 2.24f*j + 0.16f), glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f)), glm::vec3(0.75f, 1.0f, 1.0f), &m_dynamicWorld));
					m_objectsInScene.push_back(Utilities::OpenGameObject("mod_brick_wall", glm::vec3(2.24f*i + 0.16f, 1.28f / 2.0f, 2.24f*j - 1.12f + 0.16f), glm::quat(), glm::vec3(0.75f, 1.0f, 1.0f), &m_dynamicWorld));
					break;
				case Maze::Cells::S:
					std::cout << "S ";
					m_objectsInScene.push_back(Utilities::OpenGameObject("mod_brick_wall", glm::vec3(2.24f*i + 0.16f, 1.28f / 2.0f, 2.24f*j - 1.12f + 0.16f), glm::quat(), glm::vec3(0.75f, 1.0f, 1.0f), &m_dynamicWorld));
					break;
				case Maze::Cells::T:
					std::cout << "T ";
					m_objectsInScene.push_back(Utilities::OpenGameObject("mod_brick_wall", glm::vec3(2.24f*i - 1.12f + 0.16f, 1.28f / 2.0f, 2.24f*j + 0.16f), glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f)), glm::vec3(0.75f, 1.0f, 1.0f), &m_dynamicWorld));
					break;
				default:
					std::cout << "E ";
					break;
			}
		}
		std::cout << std::endl;
	}
	m_objectsInScene.push_back(Utilities::OpenGameObject("key", key, glm::quat(), glm::vec3(1.0f), &m_dynamicWorld));
	m_objectsInScene.back()->setType(GameObject::TYPE::KEY_PICKUP);
	m_objectsInScene.back()->updateScript = "speed = 150"
											"\nupdate = function(obj, dt)"
											"\n\tlocal rot_y = obj:getRotY() + speed * dt"
											"\n\tif rot_y >= 360.0 then"
											"\n\t\trot_y = 0.0"
											"\n\tend"
											"\n\tobj:setRotY(rot_y)"
											"\nend";

	for(int i = 0; i < MAX_COINS; i++){
		glm::vec3 coin = genPos(data[0].size(), data.size(), 1.28f);
		m_objectsInScene.push_back(Utilities::OpenGameObject("coin", coin, glm::quat(), glm::vec3(1.0f), &m_dynamicWorld));
		m_objectsInScene.back()->setType(GameObject::TYPE::COIN_PICKUP);
		m_objectsInScene.back()->updateScript = "speed = 150"
												"\nupdate = function(obj, dt)"
												"\n\tlocal rot_y = obj:getRotY() + speed * dt"
												"\n\tif rot_y >= 360.0 then"
												"\n\t\trot_y = 0.0"
												"\n\tend"
												"\n\tobj:setRotY(rot_y)"
												"\nend";
	}
	
	m_objectsInScene.push_back(Utilities::OpenGameObject("crate", end, glm::quat(), glm::vec3(1.0f, 0.5f, 1.0f), &m_dynamicWorld));
	auto colVol = new CollisionVolume(end);
	colVol->m_type = static_cast<CollisionVolume::VolumeType>(CollisionVolume::VolumeType::END);
	renderer::CollisionBody body(glm::vec3(0.0f), glm::quat(), glm::vec3(0.0f), glm::vec3(1.0f, 0.5f, 1.0f), static_cast<int>(renderer::CollisionShapes::SHAPE_CUBE), 1.0f);
	auto physicsBody = m_dynamicWorld.createPhysicsBody(end, glm::quat());
	physicsBody->addCollisionShapes(glm::vec3(1.0f), std::vector<renderer::CollisionBody>{body}, physics::CollisionCategory::TRIGGER);
	physicsBody->setBodyType(physics::BodyType::STATIC);
	physicsBody->setBounciness(0.0f);
	colVol->setPhysicsBody(physicsBody);
	m_collisionVolumes.push_back(colVol);

	std::cout << m_objectsInScene.size() << std::endl;
	
	//add player capsule
	body = renderer::CollisionBody(glm::vec3(0.0f), glm::quat(), glm::vec3(0.0f), glm::vec3(0.56f, 1.15f, 0.56f), static_cast<int>(renderer::CollisionShapes::SHAPE_CAPSULE), 60.0f);
	physicsBody = m_dynamicWorld.createPhysicsBody(start, glm::quat());

	physicsBody->enableGravity(true);
	physicsBody->allowSleep(false);
	physicsBody->setBounciness(0.0f);
	physicsBody->setFrictionCoefficient(1.0f);
	physicsBody->setRollingResistance(1.0f);
	physicsBody->setLinearDamping(0.6f);
	//physicsBody->setAngularDamping(0.6f);
	physicsBody->setAngularDamping(1.0f);
	physicsBody->addCollisionShapes(glm::vec3(1.0f), std::vector<renderer::CollisionBody>{body}, physics::CollisionCategory::PLAYER);
	m_player.setPhysicsBody(physicsBody);
	m_player.setPosition(start);

	//set lighting
	m_lights.push_back(new renderer::DirLight(
		glm::vec3(0.1f),
		glm::vec3(0.2f),
		glm::vec3(0.1f),
		glm::vec3(100.0f, 100.0f, -100.0f)
	));
	m_lights.push_back(new renderer::SpotLight(m_player.getCamera()->getFront(), m_player.getCamera()->getPos()));
	m_lights.push_back(new renderer::PointLight(
		glm::vec3(0.0f, 0.12f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(end.x, end.y + 1.28f, end.z),
		glm::vec3(2.9f, 3.5f, 2.5f)
	));
	m_lights.push_back(new renderer::PointLight(
		glm::vec3(0.12f, 0.12f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(key),
		glm::vec3(2.9f, 3.5f, 2.5f)
	));

	//set night skybox
	m_skybox.setSkyboxTexturePath(renderer::Skybox::RIGHT,	"res/textures/skyboxes/night_right.png");
	m_skybox.setSkyboxTexturePath(renderer::Skybox::LEFT,	"res/textures/skyboxes/night_left.png");
	m_skybox.setSkyboxTexturePath(renderer::Skybox::BOTTOM, "res/textures/skyboxes/night_bottom.png");
	m_skybox.setSkyboxTexturePath(renderer::Skybox::TOP,	"res/textures/skyboxes/night_top.png");
	m_skybox.setSkyboxTexturePath(renderer::Skybox::BACK,	"res/textures/skyboxes/night_back.png");
	m_skybox.setSkyboxTexturePath(renderer::Skybox::FRONT,	"res/textures/skyboxes/night_front.png");
	m_skybox.setEnabled(true);
	m_skybox.set();

	//add background music
	audio::Music music = m_audioManager.loadMusic("res/sounds/atmosphere.mp3");
	music.play(-1);
}

void MainApp::loop(){
	const float PHYSICS_STEP = 1.0f / (float)CONFIG.max_physics_fps;
	float accumulator = 0.0f;
	float interpolation = 0.0f;

	Uint32 physicsSteps = 0;
	Uint32 fps = 0;
	utilities::Timer fpsTimer;
	fpsTimer.start();

	m_player.getPhysicsBody()->m_body->setLinearVelocity(rp3d::Vector3(0.0f, 0.0f, 0.0f));
	m_player.getPhysicsBody()->m_body->setAngularVelocity(rp3d::Vector3(0.0f, 0.0f, 0.0f));

	float currentTime = SDL_GetTicks() / 1000.0f;
	while(m_appState == AppState::RUNNING){
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

	if(colWithEnd && m_inputManager.isKeyPressed(SDLK_RETURN)){
		if(m_player.hasKey){
			std::cout << "finish" << std::endl;
			std::cout << "you found " << m_player.coins << " coins!\n";
			m_appState = AppState::EXIT;
		} else {
			std::cout << "find the key first" << std::endl;
		}
	}

	for(auto& obj : m_objectsInScene){
		if(!obj->updateScript.empty()){
			if((luaL_dostring(L, obj->updateScript.c_str())) != LUA_OK)
				std::cout << "doString error: " << lua_tostring(L, -1) << std::endl;
			if((lua_pcall(L, 0, 0, 0)) != LUA_OK){}
				//std::cout << "pcall error: " << lua_tostring(L, -1) << std::endl;

			// lookup script function in global table
			luabridge::LuaRef updateFunc = luabridge::getGlobal(L, "update");

			if(updateFunc.isFunction()){
				try{
					updateFunc(&obj, deltaTime);
				} catch(luabridge::LuaException& ex){
					std::cout << ex.what() << std::endl;
				}
			}
		}
	}
}

void MainApp::drawGame(float interpolation){
	//interpolate the position from physics engine with the previous one
	for(auto& obj : m_objectsInScene){
		obj->setPosition(obj->getPhysicsBody()->getPosition() * interpolation + obj->getPosition() * (1.0f - interpolation));
		obj->setRotation(obj->getPhysicsBody()->getRotation() * interpolation + obj->getRotation() * (1.0f - interpolation));
	}

	m_player.setPosition(m_player.getPhysicsBody()->getPosition() * interpolation + m_player.getPosition() * (1.0f - interpolation));
	
	glViewport(0, 0, renderer::Window::getW(), renderer::Window::getH());
	
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH(), NEAR_PLANE, FAR_PLANE);
	glm::mat4 vp = renderer::Renderer::GetProjectionMatrix() * m_player.getCamera()->getViewMatrix();
	
	renderer::Renderer::updateFrustumPlanes(vp);
	updateToDrawVector();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(m_skybox.enabled())
		m_skybox.render(m_player.getCamera()->getViewMatrix(), renderer::Renderer::GetProjectionMatrix());
	drawGameObjects(vp);

	m_hud.render(m_hudElements);
	m_hud.render(m_player.coins, glm::vec3(-0.75f, 0.9f, 0.0f), glm::vec3(0.07f));
}

void MainApp::resetData(){
	for(auto obj : m_objectsInScene){
		m_dynamicWorld.destroyBody(obj->getPhysicsBody());
		delete obj;
	}

	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();
	for(auto colVol : m_collisionVolumes)
		delete colVol;
	m_collisionVolumes.clear();

	delete m_maze;
}

void MainApp::resetLevel(){
	resetData();
	m_player.reset();
	initLevel();
	m_resetLevel = false;
}


void MainApp::updateToDrawVector(){
	auto cullingJob = [this](size_t start, size_t end){
		for(size_t i = start; i < end; i++){
			std::lock_guard<std::mutex>(this->m_objsMutex);
			if(renderer::Renderer::intersected(this->m_objectsInScene[i]->getPosition(), m_objectsInScene[i]->boundSphere.radius))
				this->m_objectsToDraw.push_back(this->m_objectsInScene[i]);
		}
	};

	m_objectsToDraw.clear();
	size_t third = m_objectsInScene.size() / 3;
	std::thread thd(cullingJob, third, 2 * third);
	std::thread thd2(cullingJob, 2 * third, m_objectsInScene.size());
	cullingJob(0, third);

	thd.join();
	thd2.join();
}

void MainApp::drawGameObjects(glm::mat4& vp){
	std::vector<renderer::CollisionBody*> colBodies;
	///first draw the normal gameobjects
	//prepare shader
	m_gameObjectsShader.use();
	m_gameObjectsShader.loadFlashlight(m_player.isFlashLightOn());

	m_gameObjectsShader.loadLights(m_lights);
	m_gameObjectsShader.loadViewPosition(m_player.getCamera()->getPos());

	glm::mat4 mvp;
	auto batches = Utilities::BatchRenderables<GameObject>(m_objectsToDraw);
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
			mvp = vp * modelMatrix;
			
			m_gameObjectsShader.loadModelMatrix(modelMatrix);
			m_gameObjectsShader.loadMvpMatrix(mvp);
			renderer::Renderer::DrawTexturedModel(batch.first);
	
		}
		renderer::Renderer::EnableBackFaceCulling();
	}
	m_gameObjectsShader.unuse();
}

float getDistance(const glm::vec3 & e1, const glm::vec3 & e2){
	return std::pow(e1.x - e2.x, 2) + std::pow(e1.y - e2.y, 2) + std::pow(e1.z - e2.z, 2);
}

glm::vec3 genPos(int maxW, int maxH, float height){
	return glm::vec3((float)(rand() % (maxW-1)) * 2.24f + 0.16f, height, (float)(rand() % (maxH-1)) * 2.24f + 0.16f);
}
