#include "MainApp.h"
#include "Config.h"
#include "Utilities.h"
#include "RenderUtilities.h"
#include "Logger.h"

#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

///TODO:
	//investigate object rename issue
	//lights in creation mode shouldn't appear

float getDistance(const glm::vec3& e1, const glm::vec3& e2);


MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_gui(this)
{
	LOG_INFO("Editor is starting!");
	initSystems();
}

MainApp::~MainApp(){
	ImGui_ImplSdlGL3_Shutdown();

	m_gameObjectsMap.clear();
	m_objects_ToDraw.clear();
	m_lightsBillboardsMap.clear();
	m_selectedObjsVect.clear();

	for(auto lightBillboard : m_lightsBillboards)
		delete lightBillboard;
	m_lightsBillboards.clear();
	for(auto colVolBillboard : m_colVolumeBillboards)
		delete colVolBillboard;
	m_colVolumeBillboards.clear();
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
	m_fpsLimiter.setMaxFPS(CONFIG.max_fps);
	
	//Setup ImGui
	ImGui_ImplSdlGL3_Init(m_window.getWindow());
	ImGui::StyleColorsDark();

	renderer::Renderer::Init();
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());
	utilities::ResourceManager::Init();

	m_screenFBO.init(renderer::Window::getW(), renderer::Window::getH());
	m_dirLightFBO.init(2048, 2048);
}

void MainApp::initLevel(){
	m_gameObjectsShader.initShader("res/shaders/ed_entity");
	m_basicColorShader.initShader("res/shaders/ed_basic");
	m_billboardShader.initShader("res/shaders/ed_billboard");
	m_terrainShader.initShader("res/shaders/terrain");
	m_screenFboShader.initShader("res/shaders/frameBuffer");
	m_lightDepthShader.initShader("res/shaders/lightDepth");

	m_gizmos.init(&m_selectedObjsVect);

	/// lighting, game objects etc
	//set default data for the creatingEntiy object during the process of creating a new gameobject
	m_creationTabGameObject =  GameObject(utilities::ResourceManager::loadModel("default"));
	m_creationTabLight = renderer::DirLight(
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.0f)
	);
	
	addDefaultLighting();

	//init line VAO
	typedef struct vertex{
		float x;
		float y;
		float z;
	} vertex_t;
	vertex_t line[2] = {
		{-100.0f, 0.0f, 0.0f},
		{100.0f, 0.0f, 0.0f}
	};
	GLuint vbo;

	glGenVertexArrays(1, &m_lineVAO);
	glGenBuffers(1, &vbo);

	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * 2, line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	m_skybox.init(399.0f);
	m_terrain.init(&m_terrainShader);
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
		processInput();
		update(deltaTime);

		drawGame();

		m_gui.updateImGuiWindows();
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		ImGui::Render();

		//sdl: swap buffers
		m_window.swapBuffer();
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
		case SDL_WINDOWEVENT:
			switch(e.window.event){
			case SDL_WINDOWEVENT_RESIZED:
				renderer::Window::setW(e.window.data1);
				renderer::Window::setH(e.window.data2);
				m_screenFBO.reset(e.window.data1, e.window.data2);
				break;
			}
		}
	}

	if(m_inputManager.isKeyPressed(SDL_BUTTON_LEFT) 
	   && m_gui.b_placementTab
	   && !ImGui::IsMouseHoveringAnyWindow()
	   && !m_gui.b_showEditRotWindow)
	{
		pixelPick(m_inputManager.getActualMouseCoords());
	}
}

void MainApp::update(float deltaTime){
	if(!ImGui::GetIO().WantCaptureKeyboard)
		m_player.update(m_inputManager, deltaTime);
	
	m_gizmos.updateGizmo(m_player.getCamera(), m_inputManager, deltaTime);

	static_cast<renderer::SpotLight*>(m_lights[1])->position = m_player.getCamera()->getPos();
	static_cast<renderer::SpotLight*>(m_lights[1])->direction = m_player.getCamera()->getFront();
	for(auto lightBillboard : m_lightsBillboards)
		lightBillboard->update(deltaTime);
	for(auto gameobject : m_objectsInScene)
		gameobject->update(deltaTime);
	for(auto colVolume : m_colVolumeBillboards)
		colVolume->update(deltaTime);

	std::sort(m_lights.begin() + 2, m_lights.end(), [this](const auto& lhs, const auto& rhs) -> bool{
		glm::vec3 playerPos = m_player.getPosition();
		glm::vec3 plLPos = static_cast<renderer::PointLight*>(lhs)->position;
		glm::vec3 plRPos = static_cast<renderer::PointLight*>(rhs)->position;

		return getDistance(playerPos, static_cast<renderer::PointLight*>(lhs)->position) <
			getDistance(playerPos, static_cast<renderer::PointLight*>(rhs)->position);

	});

	updateToDrawVector();
}

void MainApp::drawGame(){
	//generate depth map for the light
	//renderer::Renderer::UpdateLightSpaceMatrix(glm::normalize(static_cast<renderer::DirLight*>(m_lights[0])->direction));
	renderer::Renderer::UpdateLightSpaceMatrix(static_cast<renderer::DirLight*>(m_lights[0])->direction);
	RenderUtilities::DrawLightDepthMap(this); //now m_dirLightFBO contiins the depth map

	//set to draw offscreen to the screen frame buffer
	m_screenFBO.bind(); //the bind method also sets the viewport to the size it was inited with
	renderer::Renderer::EnableDepthTest();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, renderer::Window::getW(), renderer::Window::getH());
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());

	if(m_gui.b_creationTab){
		RenderUtilities::DrawGameObjects(this, true);
	} else {
		if(m_skybox.enabled()) 
			m_skybox.render(m_player.getCamera()->getViewMatrix(), renderer::Renderer::GetProjectionMatrix());
		if(m_terrain.enabled()){
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, m_dirLightFBO.getDepthTexture());
			m_terrain.render(6,
							 m_player.isFlashLightOn(),
							 &m_player.getCamera()->getPos(),
							 &m_player.getCamera()->getViewMatrix(),
							 &renderer::Renderer::GetProjectionMatrix(),
							 &renderer::Renderer::GetLightSpaceMatrix(),
							 &m_lights
			);
		}

		RenderUtilities::DrawGameObjects(this, false);
		RenderUtilities::DrawLightBillboards(this);
		RenderUtilities::DrawColVolumesBillboards(this);
		RenderUtilities::DrawLines(this);
		if(Grid::isEnabled()) RenderUtilities::DrawGrid(this);
		RenderUtilities::DrawTransformGizmos(this);
	}

	//set to draw to the screen again but only a quad with the frame buffer's texture
	m_screenFBO.unbind(); // this also sets viewport to actual window size, if it's not what is wanted it can be called explicitly
	renderer::Renderer::DisableDepthTest();
	glClear(GL_COLOR_BUFFER_BIT);
	m_screenFboShader.use();
	//correct pincushion distorsion
	m_screenFboShader.loadHeight(1.0f / renderer::Renderer::GetProjectionMatrix()[1][1]);
	m_screenFboShader.loadAspectRatio(renderer::Window::getW() / (float)renderer::Window::getH());
	m_screenFboShader.loadStrength(0.0f); //0.0 = no correction
	m_screenFboShader.loadCylindricalRatio(1.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screenFBO.getTexture());
	//glBindTexture(GL_TEXTURE_2D, m_dirLightFBO.getDepthTexture());

	glBindVertexArray(utilities::ResourceManager::getNDCquad()->vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, utilities::ResourceManager::getNDCquad()->indexCount);
	m_screenFboShader.unuse();
	renderer::Renderer::EnableDepthTest();
}

void MainApp::addDefaultLighting(){
	/// initial lighting	
	//directional light
	m_lights.push_back(new renderer::DirLight());
	std::string file = "billboard_dirLight";
	LightBillboard* lightBillboard = new LightBillboard(utilities::ResourceManager::loadModel(file), m_lights[0]);
	lightBillboard->setPosition(static_cast<renderer::DirLight*>(m_lights[0])->direction);
	m_lightsBillboards.push_back(lightBillboard);
	m_lightsBillboardsMap[lightBillboard->getCode()] = lightBillboard;
	//spot light
	m_lights.push_back(new renderer::SpotLight(m_player.getCamera()->getFront(), m_player.getCamera()->getPos()));
}

void MainApp::resetData(){
	m_gui = GUI(this);
	m_selectedObjsVect.clear();
	
	m_lightsBillboardsMap.clear();
	m_gameObjectsMap.clear();
	m_objects_ToDraw.clear();
	m_selectedObjsVect.clear();

	for(auto lightBillboard : m_lightsBillboards)
		delete lightBillboard;
	m_lightsBillboards.clear();
	for(auto colVolBillboard : m_colVolumeBillboards)
		delete colVolBillboard;
	m_colVolumeBillboards.clear();
	for(auto obj : m_objectsInScene)
		delete obj;
	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();

	m_creationTabGameObject = GameObject(utilities::ResourceManager::loadModel("default"));
}

void MainApp::pixelPick(glm::vec2& coords){
	if(!m_gui.m_sceneTabs)
		return;

	RenderUtilities::PrePixelPickDraw(this);
	glm::u8vec4 color = renderer::Renderer::getColorAt(coords);
	int val = color[0] +
			  color[1] * 256 +
			  color[2] * 256 * 256;

	//check if clicked on gizmo
	if(m_gizmos.wasClicked(val)){
		m_gizmos.setActivated(val);
	} else{
		m_gizmos.setActivated(0);
		if(val){
			//something was clicked, add it to the list
			Actor* selected = nullptr;
			if(m_gui.m_sceneTabs & 1ui8 << Scene_Tabs::GAMEOBJECTS){
				auto it = m_gameObjectsMap.find(val);
				if(it != m_gameObjectsMap.end()) selected = static_cast<Actor*>(it->second);
			} else if(m_gui.m_sceneTabs & 1ui8 << Scene_Tabs::LIGHTS){
				auto it = m_lightsBillboardsMap.find(val);
				if(it != m_lightsBillboardsMap.end()) selected = static_cast<Actor*>(it->second);
			} else if(m_gui.m_sceneTabs & 1ui8 << Scene_Tabs::COL_VOLUMES){
				auto it = m_colVolumeBillboardsMap.find(val);
				if(it != m_colVolumeBillboardsMap.end()) selected = static_cast<Actor*>(it->second);
			}
			if(selected){
				if(m_inputManager.isKeyDown(SDLK_LCTRL)){
					auto it = std::find(m_selectedObjsVect.begin(), m_selectedObjsVect.end(), selected);
					if(it == m_selectedObjsVect.end()){
						selected->setSelected(true);
						m_selectedObjsVect.push_back(selected);
					}
				} else if(m_inputManager.isKeyDown(SDLK_LSHIFT)){
					auto it = std::find(m_selectedObjsVect.begin(), m_selectedObjsVect.end(), selected);
					if(it != m_selectedObjsVect.end()){
						selected->setSelected(false);
						m_selectedObjsVect.erase(it);
					}
				} else{
					for(auto& obj : m_selectedObjsVect)
						obj->setSelected(false);
					m_selectedObjsVect.clear();
					m_selectedObjsVect.push_back(selected);
					selected->setSelected(true);
				}
			} else{
				for(auto& obj : m_selectedObjsVect)
					obj->setSelected(false);
				m_selectedObjsVect.clear();
			}
		} else{
			//clicked on empty space, reset selected list
			for(auto& obj : m_selectedObjsVect)
				obj->setSelected(false);
			m_selectedObjsVect.clear();
		}
	}
}

void MainApp::addNewObject(const std::string& file){
	GameObject* object;
	object = new GameObject(utilities::ResourceManager::loadModel(file));
	object->setName(file);
	object->setInEditorName(file);

	glm::vec3 pos = m_player.getCamera()->getPos() + m_player.getCamera()->getFront() * 4.0f;
	pos.y = Grid::getHeight();
	object->setPosition(pos);
	m_objectsInScene.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	
	deselectAll();
	object->setSelected(true);
	m_selectedObjsVect.push_back(object);

	object = nullptr;
}

void MainApp::addPointLight(){
	glm::vec3 pos = m_player.getCamera()->getPos() + m_player.getCamera()->getFront() * 4.0f;
	m_lights.push_back(new renderer::PointLight(
		glm::vec3(0.3f, 0.24f, 0.14f),
		glm::vec3(0.7f, 0.42f, 0.26f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		pos,
		glm::vec3(1.0f, 0.09f, 0.032f))
	);
	std::string file = "billboard_pointLight";
	LightBillboard* lightBillboard = new LightBillboard(utilities::ResourceManager::loadModel(file), m_lights.back());
	lightBillboard->setPosition(pos);
	m_lightsBillboards.push_back(lightBillboard);
	m_lightsBillboardsMap[lightBillboard->getCode()] = lightBillboard;
}

void MainApp::addNewColVolume(){
	std::string file = "billboard_colVolume";
	CollisionVolumeBillboard* colVolumeBillboard = new CollisionVolumeBillboard(utilities::ResourceManager::loadModel(file),
																				"collision_volume",
																				renderer::CollisionShapes::SHAPE_CUBE);
	glm::vec3 pos = m_player.getCamera()->getPos() + m_player.getCamera()->getFront() * 4.0f;
	colVolumeBillboard->setPosition(pos);
	m_colVolumeBillboards.push_back(colVolumeBillboard);
	m_colVolumeBillboardsMap[colVolumeBillboard->getCode()] = colVolumeBillboard;

	deselectAll();
	colVolumeBillboard->setSelected(true);
	m_selectedObjsVect.push_back(colVolumeBillboard);

	colVolumeBillboard = nullptr;
}

void MainApp::duplicateSelectedPointLights(){
	std::vector<Actor*> backUp = m_selectedObjsVect;
	deselectAll();

    LightBillboard* object;
    for(auto& obj : backUp){
		m_lights.push_back(new renderer::PointLight(*static_cast<renderer::PointLight*>(static_cast<LightBillboard*>(obj)->getLight())));
    	object = new LightBillboard(*static_cast<LightBillboard*>(obj), m_lights.back());
    	m_lightsBillboards.push_back(object);
    	m_lightsBillboardsMap[object->getCode()] = object;
    	obj->setSelected(true);
    	m_selectedObjsVect.push_back(obj);
    }
}

void MainApp::duplicateSelectedColVolumes(){
	std::vector<Actor*> backUp = m_selectedObjsVect;
	deselectAll();

	CollisionVolumeBillboard* object;
	for(auto& obj : backUp){
		object = new CollisionVolumeBillboard(*static_cast<CollisionVolumeBillboard*>(obj));
		m_colVolumeBillboards.push_back(object);
		m_colVolumeBillboardsMap[object->getCode()] = object;
		obj->setSelected(true);
		m_selectedObjsVect.push_back(obj);
	}
}

void MainApp::removeSelectedPointLights(){
	for(auto& obj : m_selectedObjsVect){
		auto it = std::find(m_lightsBillboards.begin(), m_lightsBillboards.end(), static_cast<LightBillboard*>(obj));
		if(it != m_lightsBillboards.end()){
			m_lightsBillboardsMap.erase(obj->getCode());
			//erase-remove-idiom
			m_lights.erase(std::remove(m_lights.begin(), m_lights.end(), (*it)->getLight()), m_lights.end());
			m_lightsBillboards.erase(it);
		}
	}
	m_selectedObjsVect.clear();
}

void MainApp::removeSelectedColVolumes(){
	for(auto& obj : m_selectedObjsVect){
		auto it = std::find(m_colVolumeBillboards.begin(), m_colVolumeBillboards.end(), obj);
		if(it != m_colVolumeBillboards.end()){
			m_colVolumeBillboardsMap.erase(obj->getCode());
			m_colVolumeBillboards.erase(it);
		}
	}
	m_selectedObjsVect.clear();
}

void MainApp::removeSelectedGameObjects(){
	for(auto& obj : m_selectedObjsVect){
		auto it = std::find(m_objectsInScene.begin(), m_objectsInScene.end(), obj);
		if(it != m_objectsInScene.end()){
			m_gameObjectsMap.erase(obj->getCode());
			m_objectsInScene.erase(it);
		}
	}
	m_selectedObjsVect.clear();
}

void MainApp::duplicateSelectedGameObjects(){
	std::vector<Actor*> backUp = m_selectedObjsVect;
	deselectAll();

	GameObject* object;
	for(auto& obj : backUp){
		object = new GameObject(*static_cast<GameObject*>(obj));
		m_objectsInScene.push_back(object);
		m_gameObjectsMap[object->getCode()] = object;
		obj->setSelected(true);
		m_selectedObjsVect.push_back(obj);
	}
}

void MainApp::deselectAll(){
	for(auto obj : m_selectedObjsVect)
		if(obj) obj->setSelected(false);
	m_selectedObjsVect.clear();
}

void MainApp::updateToDrawVector(){
	m_objects_ToDraw.clear();
	if(m_gui.b_creationTab){
		m_objects_ToDraw.push_back(&m_creationTabGameObject);
	}
	else{
		for(const auto& gameObject : m_objectsInScene)
			m_objects_ToDraw.push_back(gameObject);
	}
}


float getDistance(const glm::vec3 & e1, const glm::vec3 & e2){
	return std::pow(e1.x - e2.x, 2) + std::pow(e1.y - e2.y, 2) + std::pow(e1.z - e2.z, 2);
}