#ifndef GUI_H
#define GUI_H

#include <ImGUI/imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include <GLM/glm.hpp>

#include <vector>
#include <string>

#define OBJECT_NAME 32

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
	MAP_OPEN,
	MAP_SAVE
};

struct CreatedObject{
	CreatedObject(){
		diff = "default.png";
		spec = "no_SPEC.png";
		mesh = "sphere.obj";
		isBillboard = false;
	}
	std::string diff;
	std::string spec;
	std::string mesh;
	bool isBillboard;
	std::vector<renderer::CollisionBody> colBodies;
};

class GUI{
public:
	GUI();
	GUI(MainApp* app);
	~GUI();

	//imgui variables and methods
	void updateImGuiWindows();
	void showEditorWindow();
	void showCreationTab();
	void showPlacementTab();
	void showOpenFileDialog();
	void showSaveFileDialog();
	void openButtonPressed();
	void updateDirContents(std::vector<std::string>& dir);
	void showAddObjectTab();
	void showSceneTab();

	bool b_creationTab;
	bool b_placementTab;
	bool b_showOpenFileDialog;
	bool b_showSaveFileDialog;

	char m_name[OBJECT_NAME];
	int fdEntryItem;
	int addGameobjectEntryItem;
	int placeGameobjectEntryItem;
	int collisionBodyEntryItem;
	FD_Mode fdMode;
	std::string currentPath;
	std::vector<std::string> dirContents;
	std::vector<std::string> dir_gameobjects;
	std::vector<int> collisionBodies;

private:
	float m_moveInc;
	MainApp* app;

};

#endif // !GUI_H
