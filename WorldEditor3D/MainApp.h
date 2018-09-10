#ifndef MAINAPP_H
#define MAINAPP_H

#include "GUI.h"
#include <Engine/Window.h>
#include <Engine/Camera.h>
#include <Engine/IShaderProgram.h>
#include <Engine/Lights.h>
#include <Engine/TexturedModel.h>
#include <Engine/RenderableEntity.h>
#include <Engine/Renderer.h>
#include <Engine/Timing.h>
#include <Engine/InputManager.h>
#include <Engine/ResourceManager.h>

#include "GameObjectShader.h"
#include "BasicColorShader.h"
#include "BillboardShader.h"

#include "GameObject.h"
#include "TranformGizmos.h"
#include "Player.h"

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
	
	void resetData();
	void openMap(const std::string& file);
	void saveMap(const std::string& file);
	void saveCreatedObject(char* buf);
	void openCreatedObject(const std::string& object);
	void pixelPick(glm::vec2& coords);
	void addNewObject(const std::string& file);
	void addPointLight();
	void duplicatePointLight(int index);
	void removePointLight(int index);
	void removeSelectedObject(int index);
	void duplicateSelectedObject(int index);

	void updateToDrawVector();
	void drawGameObjects(bool drawCollisionBodies);
	void drawCollisionBodies(std::vector<renderer::CollisionBody*>& colBodies);
	void prePixelPickDraw();
	void drawTransformGizmos();

	renderer::Window m_window; //the app window
	utilities::InputManager m_inputManager;
	utilities::FpsLimiter m_fpsLimiter;
	//renderer::MasterRenderer m_masterRenderer;
	TranformGizmos m_gizmos;
	GameObject* m_currentlySelectedObject;
	renderer::Light* m_currentlySelectedLight;

	
	AppState m_appState;

	//mapping all objects by a specific code to pixel select them in the editor
	std::map<unsigned int, GameObject*> m_gameObjectsMap;
	std::map<GameObject*, renderer::Light*> m_billboardLightsMap;

	std::vector<GameObject*> m_billboardsForLights;
	std::vector<GameObject*> m_objectsInScene;
	//this vector is to be filled with objects to draw 
	//i hope at aleat a frustum culling will be implemented
	std::vector<GameObject*> m_objects_ToDraw;

	//creation state variables;
	GameObject m_creationTabGameObject;
	renderer::DirLight m_creationTabLight;

	// lighting
	std::vector<renderer::Light*> m_lights;
	Player m_player;

	GameObjectShader m_gameObjectsShader;
	BasicColorShader m_basicColorShader;
	BillboardShader  m_billboardShader;

	//gui stuff
	GUI m_gui;
};

#endif // !MAINAPP_H
