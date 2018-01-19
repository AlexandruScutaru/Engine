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
	OBJECT_OPEN,
	OBJECT_SAVE,
	MAP_OPEN,
	MAP_SAVE
};

struct CreatedObject{
	CreatedObject(){
		memset(name, '\0', 32);
		diff = "default.png";
		spec = "no_SPEC.png";
		mesh = "sphere.obj";
		boxRot = glm::vec3(0.0f);
		boxScale = glm::vec3(1.0f);
	}
	char name[32];
	std::string diff;
	std::string spec;
	std::string mesh;
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
	void showOpenFileDialog();
	void showSaveFileDialog();
	void openButtonPressed();
	void updateDirContents();

	//imgui variables and methods
	bool b_creationHeader;
	bool b_placementHeader;
	bool b_showOpenFileDialog;
	bool b_showSaveFileDialog;
	int fdEntryItem;
	FD_Mode fdMode;
	std::string currentPath;
	std::vector<std::string> dirContents;

private:
	MainApp* app;

};

#endif // !GUI_H
