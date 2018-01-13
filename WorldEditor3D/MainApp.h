#ifndef MAINAPP_H
#define MAINAPP_H

#include <ImGUI/imgui.h>
#include "imgui_impl_sdl_gl3.h"
#include <Renderer/Renderer.h>
#include <GLM/glm.hpp>

#include <vector>


typedef struct{
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
} ObjectData;

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

	renderer::Window m_window; //the app window
	renderer::InputManager m_inputManager;
	renderer::FpsLimiter m_fpsLimiter;
	renderer::ShaderProgram m_lightSourceShader;
	renderer::ShaderProgram m_lightingShader;
	renderer::Camera m_camera;

	//some objects to draw
	renderer::TextureData m_crate_DIFF;
	renderer::TextureData m_crate_SPEC;

	ObjectData m_crate;
	ObjectData m_lightSource;
	std::vector<glm::vec3> m_cratePos;

	// lighting
	glm::vec3 m_dirLightPos;
	std::vector<glm::vec3> m_pointLightPos;
	std::vector<glm::vec3> m_pointLightCol;

	AppState m_appState;

	//imgui variables and methods
	void showEditorWindow();
	bool m_collapseEditor;
};

#endif // !MAINAPP_H
