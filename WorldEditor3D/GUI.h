#ifndef GUI_H
#define GUI_H

#include <ImGUI/imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include <GLM/glm.hpp>

#include <vector>
#include <string>

#define OBJECT_NAME 32

class MainApp;

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
		boxRot = glm::vec3(0.0f);
		boxPos = glm::vec3(0.0f);
		boxScale = glm::vec3(1.0f);
		isBillboard = false;
	}
	std::string diff;
	std::string spec;
	std::string mesh;
	bool isBillboard;
	glm::vec3 boxRot;
	glm::vec3 boxPos;
	glm::vec3 boxScale;

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
	FD_Mode fdMode;
	std::string currentPath;
	std::vector<std::string> dirContents;
	std::vector<std::string> dir_gameobjects;

private:
	MainApp* app;

};

#endif // !GUI_H
