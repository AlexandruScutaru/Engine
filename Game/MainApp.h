#ifndef MAINAPP_H
#define MAINAPP_H

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
#include <Engine/PhysicsWorld.h>

#include "GameObjectShader.h"
#include "BasicColorShader.h"
#include "BillboardShader.h"
#include "GameObject.h"
#include "Player.h"

#include <GLM/glm.hpp>

#include <vector>

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
	void drawGame(float interpolation); //draws on screen
	
	void resetData();

	void updateToDrawVector();
	void drawGameObjects();

	renderer::Window m_window; //the app window
	utilities::InputManager m_inputManager;
	utilities::FpsLimiter m_fpsLimiter;

	std::vector<GameObject*> m_objectsInScene;
	std::vector<GameObject*> m_objects_ToDraw;
	std::vector<renderer::Light*> m_lights;
	//GameObject* crate;
	
	Player m_player;
	AppState m_appState;

	GameObjectShader m_gameObjectsShader;
	BasicColorShader m_basicColorShader;
	BillboardShader  m_billboardShader;

	physics::PhysicsWorld m_dynamicWorld;

};

#endif // !MAINAPP_H
