#ifndef MAINAPP_H
#define MAINAPP_H

#include "GUI.h"
#include <Renderer/Renderer.h>
#include <GLM/glm.hpp>

#include <vector>
#include <map>

#include <JSON/json.hpp>
using json = nlohmann::json;

class GUI;

enum class AppState{
	EDIT,
	EXIT
};

class MainApp
{
public:
	MainApp();
	~MainApp();
	friend class GUI;

	void run(); // runs the app

private:
	void initSystems(); //initializes the core systems
	void initLevel(); //initilizes the current level
	void loop(); //main app loop
	void processInput(); //handles input processing
	void update(float deltaTime); //updates based on deltatime
	void drawGame(); //draws on screen
	
	void saveCreatedObject(char* buf);
	void openCreatedObject(const std::string& object);
	void renderToSelect(glm::vec2& coords);
	void addNewObject(const std::string& file);
	void removeSelectedObject(int index);
	void duplicateSelectedObject(int index);

	renderer::Window m_window; //the app window
	renderer::InputManager m_inputManager;
	renderer::FpsLimiter m_fpsLimiter;
	renderer::MasterRenderer m_masterRenderer;
	renderer::Camera m_camera;

	renderer::GameObject* m_currentlySelectedObject;
	
	AppState m_appState;

	//mapping all objects by a specific code to pixel select them in the editor
	std::map<unsigned int, renderer::GameObject*> m_gameObjectsMap;
	std::map<renderer::GameObject*, renderer::Light*> m_billboardLightsMap;

	//this vector is to be filled with objects to draw 
	//i hope a frustum culling will be implemented
	std::vector<renderer::GameObject*> m_objectsInScene;
	std::vector<renderer::GameObject*> m_objects_ToDraw;

	//creation state variables;
	CreatedObject m_currentCreating;
	renderer::TexturedModel m_creatingModel;
	renderer::DirLight m_creatingLight;
	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraBck;

	// lighting
	std::vector<renderer::Light*> m_lights;

	//gui stuff
	GUI m_gui;
};

#endif // !MAINAPP_H
