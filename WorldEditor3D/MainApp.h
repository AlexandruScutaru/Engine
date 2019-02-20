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
#include "Grid.h"
#include "Actor.h" 
#include "GameObject.h"
#include "LightBillboard.h"
#include "CollisionVolumeBillboard.h"
#include "TranformGizmos.h"
#include "Player.h"

#include <GLM/glm.hpp>

#include <vector>
#include <map>

#include <JSON/json.hpp>
using json = nlohmann::json;

class GUI;
class RenderUtilities;
class Utilities;

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
	friend class RenderUtilities;
	friend class Utilities;

	void run(); // runs the app

private:
	void initSystems(); //initializes the core systems
	void initLevel(); //initilizes the current level
	void loop(); //main app loop
	void processInput(); //handles input processing
	void update(float deltaTime); //updates based on deltatime
	void drawGame(); //draws on screen
	void addDefaultLighting();
	void resetData();
	
	void pixelPick(glm::vec2& coords);
	void addNewObject(const std::string& file);
	void addPointLight();
	void addNewColVolume();
	void duplicateSelectedGameObjects();
	void duplicateSelectedPointLights();
	void duplicateSelectedColVolumes();
	void removeSelectedGameObjects();
	void removeSelectedPointLights();
	void removeSelectedColVolumes();
	void deselectAll();

	void updateToDrawVector();

	renderer::Window m_window; //the app window
	utilities::InputManager m_inputManager;
	utilities::FpsLimiter m_fpsLimiter;
	
	TranformGizmos m_gizmos;
	
	std::vector<Actor*> m_selectedObjsVect;
	
	//gameobjects
	std::vector<GameObject*> m_objectsInScene;
	std::map<unsigned int, GameObject*> m_gameObjectsMap;

	// lighting
	std::vector<renderer::Light*> m_lights;
	std::vector<LightBillboard*> m_lightsBillboards;
	std::map<unsigned int, LightBillboard*> m_lightsBillboardsMap;
	
	//collision volumes (triggers)
	std::vector<CollisionVolumeBillboard*> m_colVolumeBillboards;
	std::map<unsigned int, CollisionVolumeBillboard*> m_colVolumeBillboardsMap;

	AppState m_appState;

	//this vector is to be filled with objects to draw 
	//i hope at aleat a frustum culling will be implemented
	std::vector<GameObject*> m_objects_ToDraw;

	//creation state variables;
	GameObject m_creationTabGameObject;
	renderer::DirLight m_creationTabLight;

	Player m_player;

	GameObjectShader m_gameObjectsShader;
	BasicColorShader m_basicColorShader;
	BillboardShader  m_billboardShader;

	GLuint m_lineVAO;

	//gui stuff
	GUI m_gui;
};

#endif // !MAINAPP_H
