#include "MainApp.h"

#include <dirent/dirent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define MAX_FPS 120


bool VectorOfStringGetter(void* data, int n, const char** out_text);

MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_creationHeader(false),
	m_placementHeader(true),
	m_showOpenFileDialog(false),
	m_showSaveFileDialog(false)
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

	m_fpsLimiter.setMaxFPS(MAX_FPS);

	renderer::ResourceManager::Init();
}

void MainApp::initLevel(){
	//game objects
	//set default data for the creatingEntiy;
	m_creatingModel = *(renderer::ResourceManager::getTexturedModelAt(0));
	//m_creatingEntity->setPosition(glm::vec3(0.0f));
	m_creatingLight = renderer::DirLight(glm::vec3(-2000.0f, 2000.0f, 2000.0f),
										 glm::vec3(0.05f, 0.05f, 0.05f),
										 glm::vec3(0.4f, 0.4f, 0.4f),
										 glm::vec3(0.5f, 0.5f, 0.5f));

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
	if(m_showOpenFileDialog) showOpenFileDialog();
	if(m_showSaveFileDialog) showSaveFileDialog();

}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

	if(m_creationHeader){
		m_masterRenderer.renderSingleEntity(&m_creatingModel, m_creatingLight, m_camera);
		m_masterRenderer.renderBoundingBox(renderer::ResourceManager::getTexturedModelAt(1), m_currentCreating.boxScale, m_currentCreating.boxRot, m_camera);
	}
	else if(m_placementHeader){
		m_masterRenderer.renderScene(m_entities, m_billboards, m_dirLight, m_pointLights, m_spotLight, m_camera);
	}

	ImGui::Render();
	//sdl: swap buffers
	m_window.swapBuffer();
}

void MainApp::updateDirContents(){
	m_dirContents.clear();
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(m_currentPath.c_str())) != NULL){
		while((ent = readdir(dir)) != NULL) {
			if(strncmp(ent->d_name, ".", 1) != 0){
				m_dirContents.push_back(ent->d_name);
			}
		}
		closedir(dir);
	} else {
		std::cout << "DIRENT::opendir '" << m_currentPath << "' error!" << std::endl;
	}
}

void MainApp::openButtonPressed(){
	switch(m_fdMode){
	case FD_Mode::DIFF:
		m_currentCreating.diff = m_dirContents[m_fdEntryItem];
		m_creatingModel.getMaterial().setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/" + m_dirContents[m_fdEntryItem]));
		break;
	case FD_Mode::SPEC:
		m_currentCreating.spec = m_dirContents[m_fdEntryItem];
		m_creatingModel.getMaterial().setSpecularMap(renderer::ResourceManager::getTexture("res/textures/" + m_dirContents[m_fdEntryItem]));
		break; 
	case FD_Mode::MESH:
		m_currentCreating.mesh = m_dirContents[m_fdEntryItem];
		m_creatingModel.setMesh(renderer::ResourceManager::getMesh("res/models/" + m_dirContents[m_fdEntryItem]));
		break;
	case FD_Mode::MAP:
		break;
	default:
		break;
	}
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

/// ImGui
void MainApp::showEditorWindow(){
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH-300.0f, 0.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, SCREEN_HEIGHT), ImGuiCond_Once);

	ImGui::Begin("editor", NULL);
	
	///main tabs for the editor
	//gameobject creation
	if(ImGui::Button("Object Creation")){
		m_creationHeader = true;
		m_placementHeader = false;

		//backup and set new camera transforms
		m_cameraPos = m_camera.getPos();
		m_cameraBck = m_camera.backupCameraProperties();
		m_camera.setPos(glm::vec3(0.0f, 0.0f, 5.0f));
		m_camera.setPitch(0.0f);
		m_camera.setYaw(-90.0f);
	}
	ImGui::SameLine();
	//gameobject placement
	if(ImGui::Button("Object Placement")){
		m_placementHeader = true;
		m_creationHeader = false;

		//restore camera 
		m_camera.setPos(m_cameraPos);
		m_camera.restoreCameraProperties(m_cameraBck);
	}

	if(m_creationHeader){
		ImGui::BeginChild("creation", ImVec2(-1.0f, -1.0f), true, ImGuiWindowFlags_NoScrollWithMouse);
		{
			ImGui::Text("Object info");
			static float radius;
			ImGui::InputText("Name", m_currentCreating.name, 32);
			if(ImGui::Button("Set diffuse")){
				m_showOpenFileDialog = true;
				m_currentPath = "res/textures/";
				m_fdMode = FD_Mode::DIFF;
				updateDirContents();
			}
			ImGui::SameLine();
			ImGui::Text(m_currentCreating.diff.c_str());
			if(ImGui::Button("Set specular")){
				m_showOpenFileDialog = true;
				m_currentPath = "res/textures/";
				m_fdMode = FD_Mode::SPEC;
				updateDirContents();
			}
			ImGui::SameLine();
			ImGui::Text(m_currentCreating.spec.c_str());
			if(ImGui::Button("Set mesh")){
				m_showOpenFileDialog = true;
				m_currentPath = "res/models/";
				m_fdMode = FD_Mode::MESH;
				updateDirContents();
			}
			ImGui::SameLine();
			ImGui::Text(m_currentCreating.mesh.c_str());

			ImGui::Text("Object bounding box");
			
			ImGui::SliderFloat("Size x", &m_currentCreating.boxScale.x, 0.2f, 4.0f);
			ImGui::SliderFloat("Size y", &m_currentCreating.boxScale.y, 0.2f, 4.0f);
			ImGui::SliderFloat("Size z", &m_currentCreating.boxScale.z, 0.2f, 4.0f);

			ImGui::SliderAngle("Rot x", &m_currentCreating.boxRot.x, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Y", &m_currentCreating.boxRot.y, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Z", &m_currentCreating.boxRot.z, 0.0f, 359.0f);

			if(ImGui::Button("Save object")){
				m_showSaveFileDialog = true;
				m_currentPath = "res/gameobjects/";
				m_fdMode = FD_Mode::OBJECT;
				updateDirContents();
			}
			if(ImGui::Button("Reset")){
				m_creatingModel = *(renderer::ResourceManager::getTexturedModelAt(0));
				m_currentCreating = CreatedObject();
			}
		}
		ImGui::EndChild();
	}
	
	
	if(m_placementHeader){
		ImGui::BeginChild("placement", ImVec2(), true);
		{
			ImGui::Text("place");
		}
		ImGui::EndChild();
	}

	ImGui::End();
}

void MainApp::showOpenFileDialog(){
	ImGui::SetNextWindowSize(ImVec2(300, 11.2 * ImGui::GetFrameHeightWithSpacing()), ImGuiSetCond_Always);
	ImGui::SetNextWindowFocus();
	ImGui::Begin("Open file", &m_showOpenFileDialog,
				 ImGuiWindowFlags_NoResize   |
				 ImGuiWindowFlags_NoCollapse 
	);
	ImGui::PushItemWidth(-1);
	
	ImGui::Text("Available files:");
	ImGui::ListBox("##", &m_fdEntryItem, VectorOfStringGetter, (void*)(&m_dirContents), (int)(m_dirContents.size()), 10);
	ImGui::Text("\t\t\t");
	ImGui::SameLine();
	if(ImGui::Button("Open")){
		m_showOpenFileDialog = false;
		openButtonPressed();
	}
	ImGui::SameLine();
	if(ImGui::Button("Cancel")){
		m_showOpenFileDialog = false;
	}
	ImGui::End();
}

void MainApp::showSaveFileDialog(){
	ImGui::SetNextWindowSize(ImVec2(300, 11.2 * ImGui::GetFrameHeightWithSpacing()), ImGuiSetCond_Always);
	ImGui::Begin("Save file", &m_showSaveFileDialog,
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoCollapse
	);
	ImGui::PushItemWidth(-1);

	ImGui::Text("Existing files:");

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("existingfiles", ImVec2(0, 150), true);
	for(auto dir : m_dirContents){
		ImGui::Text(dir.c_str());
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::Text("Name:");
	ImGui::SameLine();
	std::string filename;
	static char buf[32] = "";
	ImGui::InputText("##filename", buf, 32);
	ImGui::Text("\t\t\t");
	ImGui::SameLine();
	if(ImGui::Button("Save as")){
		bool exists = false;
		for(auto dir : m_dirContents){
			if(dir.compare(std::string(buf)) == 0){
				exists = true;
				break;
			}
		}
		if(!exists){
			memset(buf, '\0', 32);
			m_showSaveFileDialog = false;
			saveCreatedObject(buf);
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Cancel")){
		m_showSaveFileDialog = false;
	}
	ImGui::End();
}

bool VectorOfStringGetter(void* data, int n, const char** out_text){
	std::vector<std::string>* v = (std::vector<std::string>*)data;
	*out_text = (*v)[n].c_str();
	return true;
}