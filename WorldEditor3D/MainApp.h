#ifndef MAINAPP_H
#define MAINAPP_H

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
	void drawGame(); //draws on screen

	renderer::Window m_window; //the app window

	AppState m_appState;
};

#endif // !MAINAPP_H
