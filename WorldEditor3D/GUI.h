#ifndef GUI_H
#define GUI_H

#include <ImGUI/imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include <GLM/glm.hpp>

#include <vector>
#include <string>

#define OBJECT_NAME_SIZE 32
#define TOOLBAR_HEIGHT 36
#define MAIN_MENU_HEIGHT 18
#define EDITOR_WINDOW_WIDTH 300

class MainApp;
namespace renderer{
	struct CollisionBody;
}

enum class FD_Mode{
	DIFF,
	SPEC,
	MESH,
	OBJECT_SAVE,
	OBJECT_OPEN,
	ADD_GAMEOBJECT,
	MAP_OPEN,
	MAP_SAVE
};


class GUI{
public:
	GUI();
	GUI(MainApp* app);
	~GUI();

	//imgui variables and methods
	void updateImGuiWindows();
	void showMainMenu();
	void showToolbar();
	void showEditorWindow();
	void showCreationTab();
	void showPlacementTab();
	void showOpenFileDialog();
	void showSaveFileDialog();
	void openButtonPressed();
	void updateDirContents(std::vector<std::string>& dir);
	void showGameobjectsTab();
	void showLightsTab();
	void showGridSettingsWindow();

	bool b_creationTab;
	bool b_placementTab;
	bool b_showOpenFileDialog;
	bool b_showSaveFileDialog;
	bool b_showGridWindow;

	char m_name[OBJECT_NAME_SIZE];
	int fdEntryItem;
	int placedGameobjectEntryItem;
	int placedLightEntryItem;
	int collisionBodyEntryItem;
	FD_Mode fdMode;
	std::string currentPath;
	std::vector<std::string> dirContents;
	std::vector<int> collisionBodies;

private:
	float m_moveInc;
	MainApp* app;

};

#endif // !GUI_H
