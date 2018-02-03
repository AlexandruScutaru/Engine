#ifndef GUI_H
#define GUI_H

#include <ImGUI/imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include <GLM/glm.hpp>

#include <vector>
#include <string>

class MainApp;

enum class FD_Mode{
	DIFF,
	SPEC,
	MESH,
	STATIC_OBJECT_OPEN,
	STATIC_OBJECT_SAVE,
	ENTITY_OBJECT_SAVE,
	ENTITY_OBJECT_OPEN,
	MAP_OPEN,
	MAP_SAVE
};

struct CreatedObject{
	CreatedObject(){
		diff = "default.png";
		spec = "no_SPEC.png";
		mesh = "sphere.obj";
		boxRot = glm::vec3(0.0f);
		boxScale = glm::vec3(1.0f);
	}
	std::string diff;
	std::string spec;
	std::string mesh;
	int object_type;
	glm::vec3 boxRot;
	glm::vec3 boxScale;
};


class GUI{
public:
	GUI();
	GUI(MainApp* app);
	~GUI();

	void updateImGuiWindows();
	void showEditorWindow();
	void showCreationTab();
	void showPlacementTab();
	void showOpenFileDialog();
	void showSaveFileDialog();
	void openButtonPressed();
	void updateDirContents(std::vector<std::string>& dir);

	void showAddObjectTab();

	//imgui variables and methods
	bool b_creationTab;
	bool b_placementTab;
	bool b_showOpenFileDialog;
	bool b_showSaveFileDialog;

	int creation_object_type;
	const char* m_combo_objectType;
	int fdEntryItem;
	int gameobjectEntryItem;
	FD_Mode fdMode;
	std::string currentPath;
	std::vector<std::string> dirContents;
	std::vector<std::string> availableEntities;


private:
	MainApp* app;

};

#endif // !GUI_H
