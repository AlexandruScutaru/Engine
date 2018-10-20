#include "MainApp.h"
#include "Config.h"
#include "Utilities.h"

#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

///TODO:
	//investigate object rename issue
	//lights in creation mode

MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_gui(this),
	m_currentlySelectedLight(nullptr)
{
	initSystems();
}

MainApp::~MainApp(){
	ImGui_ImplSdlGL3_Shutdown();

	m_billboardLightsMap.clear();
	m_gameObjectsMap.clear();
	m_objects_ToDraw.clear();
	for(auto obj : m_billboardsForLights)
		delete obj;
	m_billboardsForLights.clear();
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
}

void MainApp::initLevel(){
	m_gameObjectsShader.initShader("res/shaders/entity");
	m_basicColorShader.initShader("res/shaders/basic");
	m_billboardShader.initShader("res/shaders/billboard");

	m_gizmos.init((std::vector<Actor*>*)(&m_selectedObjsVect));

	/// lighting, game objects etc
	//set default data for the creatingEntiy object during the process of creating a new gameobject
	m_creationTabGameObject =  GameObject(utilities::ResourceManager::loadModel("default"));
	m_creationTabLight = renderer::DirLight();
	
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
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
		processInput();
		update(deltaTime);

		drawGame();
		m_gui.updateImGuiWindows();
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

	if(m_selectedObjsVect.size() > 0 && m_currentlySelectedLight){
		if(m_currentlySelectedLight->casterType == renderer::Caster::DIRECTIONAL){
			static_cast<renderer::DirLight*>(m_currentlySelectedLight)->direction = m_selectedObjsVect[0]->getPosition();
		}else if(m_currentlySelectedLight->casterType == renderer::Caster::POINT){
			(static_cast<renderer::PointLight*>(m_currentlySelectedLight))->position = m_selectedObjsVect[0]->getPosition();
		}
	}

	updateToDrawVector();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	bool bDrawColBodies = false;
	if(m_gui.b_creationTab)
		bDrawColBodies = true;
	drawGameObjects(bDrawColBodies);
	drawLines();
	if(Grid::isEnabled()) drawGrid();
	drawTransformGizmos();
}

void MainApp::addDefaultLighting(){
	/// initial lighting	
	//directional light
	m_lights.push_back(new renderer::DirLight());
	std::string file = "billboard_dirLight";
	GameObject* object;
	object = new GameObject(utilities::ResourceManager::loadModel(file));
	glm::vec3 pos = static_cast<renderer::DirLight*>(m_lights[0])->direction;
	object->setPosition(pos);
	m_billboardsForLights.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	m_billboardLightsMap[object] = m_lights[0];
	//spot light
	m_lights.push_back(new renderer::SpotLight(m_player.getCamera()->getFront(), m_player.getCamera()->getPos()));
}

void MainApp::resetData(){
	m_gui = GUI(this);
	m_selectedObjsVect.clear();
	m_currentlySelectedLight = nullptr;
	
	m_billboardLightsMap.clear();
	m_gameObjectsMap.clear();
	m_objects_ToDraw.clear();

	for(auto obj : m_billboardsForLights)
		delete obj;
	m_billboardsForLights.clear();
	for(auto obj : m_objectsInScene)
		delete obj;
	m_objectsInScene.clear();
	for(auto light : m_lights)
		delete light;
	m_lights.clear();

	m_creationTabGameObject = GameObject(utilities::ResourceManager::loadModel("default"));
}

void MainApp::openMap(const std::string& file){
	std::string path = m_gui.currentPath + file;

	resetData();

	std::ifstream in(path);
	json mapFile;
	in >> mapFile;

	GameObject* object;
	//gameobjects
	for(auto it = mapFile["gameobjects"].begin(); it != mapFile["gameobjects"].end(); ++it){
		json obj = it.value();
		std::string obj_name = obj["name"].get<std::string>();
		std::string obj_inEditorName = obj["inEditorName"].get<std::string>();
		physics::BodyType type = static_cast<physics::BodyType>(obj["bodyType"].get<int>());
		std::vector<float> p = obj["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = obj["rot"].get<std::vector<float>>();
		std::vector<float> s = obj["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);

		object = new GameObject(utilities::ResourceManager::loadModel(obj_name));
		object->setName(obj_name);
		object->setInEditorName(obj_inEditorName);
		object->setPosition(pos);
		object->setRotation(glm::quat(r[3], r[0], r[1], r[2]));
		object->setScale(scale);
		object->m_bodyType = type;

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
	
	std::string billboard_file = "billboard_dirLight";
	object = new GameObject(utilities::ResourceManager::loadModel(billboard_file));
	glm::vec3 light_pos = static_cast<renderer::DirLight*>(m_lights[0])->direction;
	object->setPosition(light_pos);
	m_billboardsForLights.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	m_billboardLightsMap[object] = m_lights[0];
	
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

		billboard_file = "billboard_pointLight";
		object = new GameObject(utilities::ResourceManager::loadModel(billboard_file));
		light_pos = static_cast<renderer::PointLight*>(m_lights.back())->position;
		object->setPosition(light_pos);
		m_billboardsForLights.push_back(object);
		m_gameObjectsMap[object->getCode()] = object;
		m_billboardLightsMap[object] = m_lights.back();
	}
}

void MainApp::saveMap(const std::string& file){
	std::string path = m_gui.currentPath + file;
	json map;
	map["gameobjects"] = {};
	map["lights"] = {};
	map["lights"]["pointLights"] = {};
	
	for(auto obj : m_objectsInScene){
		json entry = {
			{"name", obj->getName()},
			{"inEditorName", obj->getInEditorName()},
			{"pos", {obj->getPosition().x, obj->getPosition().y, obj->getPosition().z}},
			{"rot", {obj->getRotation().x, obj->getRotation().y, obj->getRotation().z, obj->getRotation().w}},
			{"scale", {obj->getScale().x, obj->getScale().y, obj->getScale().z}},
			{"bodyType", (int)obj->m_bodyType},
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
	std::ofstream out(path);
	out << std::setw(4) << map << std::endl;
	out.close();
}

void MainApp::saveCreatedObject(char* buf){
	json entry = {
		{"diff", m_creationTabGameObject.getDiffName()},
		{"spec", m_creationTabGameObject.getSpecName()},
		{"mesh", m_creationTabGameObject.getMeshName()},
		{"billboard", m_creationTabGameObject.isBillboard()},
		{"gravity", m_creationTabGameObject.m_gravityEnabled},
		{"sleep", m_creationTabGameObject.m_allowedToSleep},
		{"bounciness", m_creationTabGameObject.m_bounciness},
		{"friction", m_creationTabGameObject.m_frictionCoefficient},
		{"rollingResist", m_creationTabGameObject.m_rollingResistance},
		{"linearDamp", m_creationTabGameObject.m_linearDamping},
		{"angularDamp", m_creationTabGameObject.m_angularDamping}
	};
	for(renderer::CollisionBody body : m_creationTabGameObject.getColBodies()){
		json col = {
			{"shape", body.shape},
			{"pos", {body.colRelativePos.x, body.colRelativePos.y, body.colRelativePos.z}},
			{"rot", {body.colRotQuat.x, body.colRotQuat.y, body.colRotQuat.z, body.colRotQuat.w}},
			{"scale", {body.colScale.x, body.colScale.y, body.colScale.z}},
			{"mass", body.mass}
		};
		entry["collision"].push_back(col);
	}

	std::string path = m_gui.currentPath + std::string(buf);
	std::ofstream out(path);
	out << std::setw(4) << entry << std::endl;
	out.close();
}

void MainApp::openCreatedObject(const std::string& object){
	std::string path = m_gui.currentPath + object;
	std::ifstream in(path);
	json obj;
	in >> obj;
	in.close();

	m_creationTabGameObject.clearColBodies();
	m_gui.collisionBodies.clear();
	m_gui.collisionBodyEntryItem = -1;
	m_creationTabGameObject.setDiffName(obj["diff"].get<std::string>());
	m_creationTabGameObject.setSpecName(obj["spec"].get<std::string>());
	m_creationTabGameObject.setMeshName(obj["mesh"].get<std::string>());
	m_creationTabGameObject.setIsBillboard(obj["billboard"].get<bool>());
	m_creationTabGameObject.m_gravityEnabled = obj["gravity"].get<bool>();
	m_creationTabGameObject.m_allowedToSleep = obj["sleep"].get<bool>();
	m_creationTabGameObject.m_bounciness = obj["bounciness"].get<float>();
	m_creationTabGameObject.m_frictionCoefficient = obj["friction"].get<float>();
	m_creationTabGameObject.m_rollingResistance = obj["rollingResist"].get<float>();
	m_creationTabGameObject.m_linearDamping = obj["linearDamp"].get<float>();
	m_creationTabGameObject.m_angularDamping = obj["angularDamp"].get<float>();

	for(auto it = obj["collision"].begin(); it != obj["collision"].end(); it++){
		json col = it.value();
		renderer::CollisionBody body;
		body.shape = col["shape"].get<int>();
		body.mass = col["mass"].get<float>();
		std::vector<float> p = col["pos"].get<std::vector<float>>();
		glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
		std::vector<float> r = col["rot"].get<std::vector<float>>();
		std::vector<float> s = col["scale"].get<std::vector<float>>();
		glm::vec3 scale = glm::vec3(s[0], s[1], s[2]);
		body.colRelativePos = pos;
		body.colRotQuat = glm::quat(r[3], r[0], r[1], r[2]);
		glm::vec3 rot = glm::eulerAngles(body.colRotQuat);
		body.colRotEuler = glm::vec3(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
		body.colScale = scale;
		m_creationTabGameObject.addColBody(body);
		m_gui.collisionBodies.push_back(body.shape);
	}

	m_creationTabGameObject.getTexturedModel()->setMesh(utilities::ResourceManager::getMesh("res/models/" + m_creationTabGameObject.getMeshName()));
	m_creationTabGameObject.getTexturedModel()->getMaterial().setDiffuseMap( utilities::ResourceManager::getTexture("res/textures/" + m_creationTabGameObject.getDiffName()));
	m_creationTabGameObject.getTexturedModel()->getMaterial().setSpecularMap(utilities::ResourceManager::getTexture("res/textures/" + m_creationTabGameObject.getSpecName()));
}

void MainApp::pixelPick(glm::vec2& coords){
	prePixelPickDraw();
	glm::u8vec4 color = renderer::Renderer::getColorAt(coords);
	int val = color[0] +
			  color[1] * 256 +
			  color[2] * 256 * 256;

	//check if clicked on gizmo
	if(m_gizmos.wasClicked(val)){
		m_gizmos.setActivated(val);
	} else {
		m_gizmos.setActivated(0);
		if(val){
			GameObject* obj = m_gameObjectsMap[val];
			auto it = m_billboardLightsMap.find(obj);
			if(it != m_billboardLightsMap.end()){ 
				//clicked on a billboard for light
				m_currentlySelectedLight = it->second;
				for(auto& obj : m_selectedObjsVect)
					if(obj) obj->setSelected(false);
				m_selectedObjsVect.clear();
				m_selectedObjsVect.push_back(obj);
				m_selectedObjsVect[0]->setSelected(true);

				for(size_t i = 0; i < m_lights.size(); ++i){
					if(m_lights[i] == m_currentlySelectedLight){
						m_gui.placedLightEntryItem = i;
					}
				}
				m_gui.placedGameobjectEntryItem = -1;
			} else{ 
				//clicked on a gameobject
				if(m_inputManager.isKeyDown(SDLK_LCTRL) && m_gui.placedLightEntryItem == -1){
					auto it = std::find(m_selectedObjsVect.begin(), m_selectedObjsVect.end(), obj);
					if(it == m_selectedObjsVect.end())
						m_selectedObjsVect.push_back(obj);
					obj->setSelected(true);
					m_gui.placedGameobjectEntryItem = -1;
				} else if(m_inputManager.isKeyDown(SDLK_LSHIFT) && m_gui.placedLightEntryItem == -1){
					auto it = std::find(m_selectedObjsVect.begin(), m_selectedObjsVect.end(), obj);
					if(it != m_selectedObjsVect.end()){
						(*it)->setSelected(false);
						m_selectedObjsVect.erase(it);
					}
					m_gui.placedGameobjectEntryItem = -1;
				} else{
					for(auto& obj : m_selectedObjsVect)
						if(obj) obj->setSelected(false);
					m_selectedObjsVect.clear();
					m_selectedObjsVect.push_back(obj);
					obj->setSelected(true);
					auto it = std::find(m_objectsInScene.begin(), m_objectsInScene.end(), obj);
					for(size_t i = 0; i < m_objectsInScene.size(); ++i){
						if(m_objectsInScene[i] == obj){
							m_gui.placedGameobjectEntryItem = i;
						}
					}
				}
				m_gui.placedLightEntryItem = -1;
				m_currentlySelectedLight = nullptr;
			}
		} else{
			for(auto& obj : m_selectedObjsVect)
				if(obj) obj->setSelected(false);
			m_selectedObjsVect.clear();
			m_currentlySelectedLight = nullptr;
			m_gui.placedGameobjectEntryItem = -1;
			m_gui.placedLightEntryItem = -1;
			if(!m_inputManager.isKeyPressed(SDLK_LCTRL) || !m_inputManager.isKeyPressed(SDLK_LSHIFT)){
				for(auto& obj : m_selectedObjsVect)
					if(obj) obj->setSelected(false);
				m_selectedObjsVect.clear();
			}
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
	
	for(auto& obj : m_selectedObjsVect)
		if(obj) obj->setSelected(false);
	m_selectedObjsVect.clear();

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
	GameObject* object = new GameObject(utilities::ResourceManager::loadModel(file));
	object->setPosition(pos);
	m_billboardsForLights.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	m_billboardLightsMap[object] = m_lights.back();
}

void MainApp::duplicatePointLight(int index){
	renderer::PointLight *pl = new renderer::PointLight(*static_cast<renderer::PointLight*>(m_lights[index]));
	m_lights.push_back(pl);
	std::string file = "billboard_pointLight";
	GameObject* object = new GameObject(utilities::ResourceManager::loadModel(file));
	object->setPosition(pl->position);
	m_billboardsForLights.push_back(object);
	m_gameObjectsMap[object->getCode()] = object;
	m_billboardLightsMap[object] = m_lights.back();
}

void MainApp::removePointLight(int index){
	if(m_selectedObjsVect.size() != 1)
		return;
	m_gameObjectsMap.erase(m_selectedObjsVect[0]->getCode());
	m_billboardLightsMap.erase(m_selectedObjsVect[0]);

	auto it = std::find(m_billboardsForLights.begin(), m_billboardsForLights.end(), m_selectedObjsVect[0]);
	if(it != m_billboardsForLights.end())
		m_billboardsForLights.erase(it);

	m_selectedObjsVect.clear();
	m_lights.erase(m_lights.begin() + index);
	m_currentlySelectedLight = nullptr;
}

void MainApp::removeSelectedObjects(){
	for(auto& obj : m_selectedObjsVect){
		auto it = std::find(m_objectsInScene.begin(), m_objectsInScene.end(), obj);
		if(it != m_objectsInScene.end()){
			m_gameObjectsMap.erase(obj->getCode());
			m_objectsInScene.erase(it);
		}
	}
	m_selectedObjsVect.clear();
}

void MainApp::duplicateSelectedObjects(){
	GameObject* object;
	std::vector<GameObject*> backUp = m_selectedObjsVect;
	for(auto obj : m_selectedObjsVect)
		if(obj) obj->setSelected(false);
	m_selectedObjsVect.clear();

	for(auto& obj : backUp){
		object = new GameObject(*obj);
		m_objectsInScene.push_back(object);
		m_gameObjectsMap[object->getCode()] = object;
		obj->setSelected(true);
		m_selectedObjsVect.push_back(obj);
	}
	object = nullptr;
}

void MainApp::updateToDrawVector(){
	m_objects_ToDraw.clear();
	if(m_gui.b_creationTab){
		m_objects_ToDraw.push_back(&m_creationTabGameObject);
	}
	else{
		for(const auto& gameObject : m_objectsInScene)
			m_objects_ToDraw.push_back(gameObject);
		for(const auto& gameObject : m_billboardsForLights)
			m_objects_ToDraw.push_back(gameObject);
	}
}

void MainApp::drawGameObjects(bool bDrawColbodies){
	std::vector<renderer::CollisionBody*> colBodies;
	///first draw the normal gameobjects
	//prepare shader
	m_gameObjectsShader.use();
	m_gameObjectsShader.loadFlashlight(m_player.isFlashLightOn());
	
	if(m_gui.b_creationTab)
		m_gameObjectsShader.loadLights(std::vector<renderer::Light*>{(renderer::Light*)&m_creationTabLight});
	else
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
			modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());
			modelMatrix = glm::scale(modelMatrix, gameObject->getScale());
			m_gameObjectsShader.loadModelMatrix(modelMatrix);
			
			renderer::Renderer::DrawTexturedModel(batch.first);
		}
		if(bDrawColbodies){
			std::vector<renderer::CollisionBody>* cb = &batch.second[0]->getColBodies();
			for(auto& body : *cb)
				colBodies.push_back(&body);
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
				modelMatrix = modelMatrix * glm::toMat4(gameObject->getRotation());

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
			if(bDrawColbodies){
				std::vector<renderer::CollisionBody>* cb = &batch.second[0]->getColBodies();
				for(auto& body : *cb)
					colBodies.push_back(&body);
			}
		}
	}
	m_billboardShader.unuse();

	if(bDrawColbodies)
		drawCollisionBodies(colBodies);
}

void MainApp::drawCollisionBodies(std::vector<renderer::CollisionBody*>& colBodies){
	if(!colBodies.size())
		return;

	// view/projection transformations
	glm::mat4 view = m_player.getCamera()->getViewMatrix();
	m_basicColorShader.use();
	m_basicColorShader.loadViewMatrix(view);
	m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	m_basicColorShader.loadColor(glm::vec3(0.0f, 1.0f, 0.0f));
	renderer::TexturedModel* td;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for(auto body : colBodies){
		glm::mat4 model;
		model = glm::translate(model, body->colRelativePos);
		model = model * glm::toMat4(body->colRotQuat);
		model = glm::scale(model, body->colScale);
		m_basicColorShader.loadModelMatrix(model);

		td = utilities::ResourceManager::loadModel(body->shape);
		glBindVertexArray(td->getMesh()->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, td->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_basicColorShader.unuse();
}

void MainApp::prePixelPickDraw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 colorCode;
	
	m_basicColorShader.use();
	glm::mat4 view = m_player.getCamera()->getViewMatrix();
	m_basicColorShader.loadViewMatrix(view);
	m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());

	//render gameobjects
	for(auto object : m_objects_ToDraw){
		glm::mat4 model;
		if(object->isBillboard()){
			model = glm::translate(model, object->getPosition());
			model = model * glm::toMat4(object->getRotation());

			model[0][0] = view[0][0];
			model[0][1] = view[1][0];
			model[0][2] = view[2][0];
			model[1][0] = view[0][1];
			model[1][1] = view[1][1];
			model[1][2] = view[2][1];
			model[2][0] = view[0][2];
			model[2][1] = view[1][2];
			model[2][2] = view[2][2];
			model = glm::scale(model, object->getScale());
		} else{
			model = glm::translate(model, object->getPosition());
			model = model * glm::toMat4(object->getRotation());
			model = glm::scale(model, object->getScale());
		}

		m_basicColorShader.loadModelMatrix(model);
		int code = object->getCode();
		colorCode = glm::vec3(
			((code & 0x000000FF) >>  0) / 255.0f,
			((code & 0x0000FF00) >>  8) / 255.0f,
			((code & 0x00FF0000) >> 16) / 255.0f
		);
		m_basicColorShader.loadColor(colorCode);
		glBindVertexArray(object->getTexturedModel()->getMesh()->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, object->getTexturedModel()->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
	}

	if(m_gizmos.canBeShown()){
		auto gizmos = m_gizmos.getGizmos();
		renderer::TexturedModel* td;
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::vec3 scale = glm::vec3(0.5f) * glm::distance(m_gizmos.getPosition(), m_player.getCamera()->getPos());
		for(auto gizmo : *gizmos){
			glm::mat4 model;
			model = glm::translate(model, m_gizmos.getPosition());
			
			if(m_gizmos.getGizmoMode() == (int)GizmoMode::ROTATE){
				auto baseRot = gizmo.obj->getRotation();
				model = model * glm::toMat4(m_gizmos.getRot() * baseRot);
			} else{
				model = model * glm::toMat4(gizmo.obj->getRotation());
			}

			model = glm::scale(model, scale);

			m_basicColorShader.loadModelMatrix(model);
			int code = gizmo.obj->getCode();
			colorCode = glm::vec3(
				((code & 0x000000FF) >>  0) / 255.0f,
				((code & 0x0000FF00) >>  8) / 255.0f,
				((code & 0x00FF0000) >> 16) / 255.0f
			);
			m_basicColorShader.loadColor(colorCode);
			td = gizmo.obj->getTexturedModel();
			glBindVertexArray(td->getMesh()->vertexArrayObject);
			glDrawElements(GL_TRIANGLES, td->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
		}
	}
	m_basicColorShader.unuse();
}

void MainApp::drawTransformGizmos(){
	if(!m_gizmos.canBeShown() || m_gui.b_creationTab)
		return;

	glClear(GL_DEPTH_BUFFER_BIT);
	const std::vector<Gizmo>* gzms = m_gizmos.getGizmos();

	m_basicColorShader.use();
	// view/projection transformations
	glm::mat4 view = m_player.getCamera()->getViewMatrix();
	m_basicColorShader.loadViewMatrix(view);
	m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	glm::vec3 scale = glm::vec3(0.5f) * glm::distance(m_gizmos.getPosition(), m_player.getCamera()->getPos());
	renderer::TexturedModel* td;
	for(auto gzm : *gzms){
		m_basicColorShader.loadColor(gzm.color);
		glm::mat4 model;
		model = glm::translate(model, m_gizmos.getPosition());

		if(m_gizmos.getGizmoMode() == (int)GizmoMode::ROTATE){
			auto baseRot = gzm.obj->getRotation();
			model = model * glm::toMat4(m_gizmos.getRot() * baseRot);
		} else{
			model = model * glm::toMat4(gzm.obj->getRotation());
		}

		model = glm::scale(model, scale);

		m_basicColorShader.loadModelMatrix(model);

		td = gzm.obj->getTexturedModel();
		glBindVertexArray(td->getMesh()->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, td->getMesh()->indexCount, GL_UNSIGNED_INT, 0);
	}
	m_basicColorShader.unuse();
}

void MainApp::drawLines(){
	///bind the line VAO
	glBindVertexArray(m_lineVAO);
	///tell opengl to draw wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	///load appropiate uniforms
	m_basicColorShader.use();
	m_basicColorShader.loadViewMatrix(m_player.getCamera()->getViewMatrix());
	m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	glm::mat4 model;
	///directional lines
	//x
	model = glm::translate(model, glm::vec3(0.0f));
	m_basicColorShader.loadColor(glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	m_basicColorShader.loadModelMatrix(model);
	glDrawArrays(GL_LINES, 0, 2);
	//y
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f));
	m_basicColorShader.loadColor(glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	m_basicColorShader.loadModelMatrix(model);
	glDrawArrays(GL_LINES, 0, 2);
	//z
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f));
	m_basicColorShader.loadColor(glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f));
	m_basicColorShader.loadModelMatrix(model);
	glDrawArrays(GL_LINES, 0, 2);

	///tell opengl to draw filled polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	///unbind line VAO
	glBindVertexArray(0);
	///stop using this shader
	m_basicColorShader.unuse();
}

void MainApp::drawGrid(){
	///bind the line VAO
	glBindVertexArray(m_lineVAO);
	///tell opengl to draw wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	///load appropiate uniforms
	m_basicColorShader.use();
	m_basicColorShader.loadViewMatrix(m_player.getCamera()->getViewMatrix());
	m_basicColorShader.loadProjectionMatrix(renderer::Renderer::GetProjectionMatrix());
	m_basicColorShader.loadColor(glm::vec3(0.3f, 0.3f, 0.3f));

	glm::vec3 gridPosition;
	gridPosition.x = std::round(m_player.getPosition().x / Grid::getStep()) * Grid::getStep();
	gridPosition.y = Grid::getHeight();
	gridPosition.z = std::round(m_player.getPosition().z / Grid::getStep()) * Grid::getStep();

	float scaleFactor = 0.069f;
	int ord = 0;
	while(Grid::getStep() * ord <= 7.0f){
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(gridPosition.x, Grid::getHeight(), gridPosition.z + Grid::getStep() * ord));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(gridPosition.x, Grid::getHeight(), gridPosition.z - Grid::getStep() * ord));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		ord++;
	}
	ord = 0;
	while(Grid::getStep() * ord <= 7.0f){
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(gridPosition.x + Grid::getStep() * ord, Grid::getHeight(), gridPosition.z));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(gridPosition.x - Grid::getStep() * ord, Grid::getHeight(), gridPosition.z));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scaleFactor));
		m_basicColorShader.loadModelMatrix(model);
		glDrawArrays(GL_LINES, 0, 2);

		ord++;
	}

	///tell opengl to draw filled polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	///unbind line VAO
	glBindVertexArray(0);
	///stop using this shader
	m_basicColorShader.unuse();
}
