#ifndef MAINAPP_H
#define MAINAPP_H

#include <ImGUI/imgui.h>
#include "imgui_impl_sdl_gl3.h"
#include <Renderer/Renderer.h>
#include <GLM/glm.hpp>

#include <vector>
#include <map>

#include <JSON/json.hpp>
using json = nlohmann::json;

enum class FD_Mode{
	DIFF,
	SPEC,
	MESH,
	OBJECT,
	MAP
};

struct CreatedObject{
	CreatedObject(){
		memset(name, '\0', 32);
		diff = "default.png";
		spec = "no_SPEC.png";
		mesh = "sphere.obj";
		boxRot = glm::vec3(0.0f);
		boxScale = glm::vec3(1.0f);
	}
	char name[32];
	std::string diff;
	std::string spec;
	std::string mesh;
	glm::vec3 boxRot;
	glm::vec3 boxScale;
};

enum class AppState{
	EDIT,
	EXIT
};

class MainApp
{
public:
	MainApp();
	~MainApp();

	void run(); // runs the app

private:
	void initSystems(); //initializes the core systems
	void initLevel(); //initilizes the current level
	void loop(); //main app loop
	void processInput(); //handles input processing
	void update(float deltaTime); //updates based on deltatime
	void updateImGuiWindows();
	void drawGame(); //draws on screen
	void updateDirContents();
	void openButtonPressed();
	void saveCreatedObject(char* buf);

	renderer::Window m_window; //the app window
	renderer::InputManager m_inputManager;
	renderer::FpsLimiter m_fpsLimiter;
	renderer::MasterRenderer m_masterRenderer;
	renderer::Camera m_camera;
	
	AppState m_appState;

	//some objects to draw
	std::vector<renderer::Entity*> m_entities;
	std::vector<renderer::BillBoard*> m_billboards;

	//creation state variables;
	CreatedObject m_currentCreating;
	renderer::TexturedModel m_creatingModel;
	renderer::DirLight m_creatingLight;
	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraBck;

	// lighting
	renderer::DirLight m_dirLight;
	std::vector<renderer::PointLight> m_pointLights;
	renderer::SpotLight m_spotLight;

	
	//imgui variables and methods
	void showEditorWindow();
	bool m_creationHeader;
	bool m_placementHeader;

	void showOpenFileDialog();
	void showSaveFileDialog();
	bool m_showOpenFileDialog;
	bool m_showSaveFileDialog;
	int m_fdEntryItem;
	FD_Mode m_fdMode;
	std::string m_currentPath;
	std::vector<std::string> m_dirContents;

};

#endif // !MAINAPP_H
