#include "MainApp.h"
#include "Config.h"
#include "Utilities.h"
#include "RenderUtilities.h"

#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

///TODO:
	//investigate object rename issue
	//lights in creation mode shouldn't appear
	//add utilities for loading/saving maps/objects
	//add thumbnail for model selected to be added

MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_gui(this)
{
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

	m_gizmos.init(&m_selectedObjsVect);

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
	for(auto lightBillboard : m_lightsBillboards)
		lightBillboard->update(deltaTime);
	for(auto gameobject : m_objectsInScene)
		gameobject->update(deltaTime);
	for(auto colVolume : m_colVolumeBillboards)
		colVolume->update(deltaTime);


	updateToDrawVector();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	renderer::Renderer::updateProjectionMatrix(m_player.getCamera()->getFOV(), renderer::Window::getW(), renderer::Window::getH());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	bool bDrawColBodies = false;
	if(m_gui.b_creationTab)
		bDrawColBodies = true;
	RenderUtilities::DrawGameObjects(this, bDrawColBodies);
	RenderUtilities::DrawLightBillboards(this);
	RenderUtilities::DrawColVolumesBillboards(this);
	RenderUtilities::DrawLines(this);
	if(Grid::isEnabled()) RenderUtilities::DrawGrid(this);
	RenderUtilities::DrawTransformGizmos(this);
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
	
	LightBillboard* lightBillboard = new LightBillboard(utilities::ResourceManager::loadModel(billboard_file), m_lights[0]);
	lightBillboard->setPosition(static_cast<renderer::DirLight*>(m_lights[0])->direction);
	m_lightsBillboards.push_back(lightBillboard);
	m_lightsBillboardsMap[lightBillboard->getCode()] = lightBillboard;
	
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

		LightBillboard* lightBillboard = new LightBillboard(utilities::ResourceManager::loadModel(billboard_file), m_lights.back());
		lightBillboard->setPosition(static_cast<renderer::DirLight*>(m_lights.back())->direction);
		m_lightsBillboards.push_back(lightBillboard);
		m_lightsBillboardsMap[lightBillboard->getCode()] = lightBillboard;
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
    	object = new LightBillboard(*static_cast<LightBillboard*>(obj));
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
		auto it = std::find(m_lights.begin(), m_lights.end(), static_cast<LightBillboard*>(obj)->getLight());
		if(it != m_lights.end()){
			m_lightsBillboardsMap.erase(obj->getCode());
			m_lights.erase(it);
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
