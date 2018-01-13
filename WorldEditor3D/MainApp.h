#ifndef MAINAPP_H
#define MAINAPP_H

#include <ImGUI/imgui.h>
#include "imgui_impl_sdl_gl3.h"
#include <Renderer/Renderer.h>


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

	AppState m_appState;

	//imgui variables and methods
	void showEditorWindow();
	bool m_collapseEditor;
};

#endif // !MAINAPP_H
