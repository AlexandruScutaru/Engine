#include "GUI.h"
#include "MainApp.h"

#include <dirent/dirent.h>


bool VectorOfStringGetter(void* data, int n, const char** out_text);


GUI::GUI():
	app(nullptr)
{}

GUI::GUI(MainApp* app):
	app(app),
	b_creationHeader(false),
	b_placementHeader(true),
	b_showOpenFileDialog(false),
	b_showSaveFileDialog(false)
{}

GUI::~GUI(){}


void GUI::updateImGuiWindows(){
	ImGui_ImplSdlGL3_NewFrame(app->m_window.getWindow());

	showEditorWindow();
	if(b_showOpenFileDialog) showOpenFileDialog();
	if(b_showSaveFileDialog) showSaveFileDialog();
}

void GUI::showEditorWindow(){
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(app->m_window.getW() - 300.0f, 0.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, (float)app->m_window.getH()), ImGuiCond_Once);

	ImGui::Begin("editor", NULL);

	///main tabs for the editor
	//gameobject creation
	if(ImGui::Button("Object Creation") && !b_creationHeader){
		b_creationHeader = true;
		b_placementHeader = false;

		//backup and set new camera transforms
		app->m_cameraPos = app->m_camera.getPos();
		app->m_cameraBck = app->m_camera.backupCameraProperties();
		app->m_camera.setPos(glm::vec3(0.0f, 0.0f, 5.0f));
		app->m_camera.setPitch(0.0f);
		app->m_camera.setYaw(-90.0f);
	}
	ImGui::SameLine();
	//gameobject placement
	if(ImGui::Button("Object Placement") && !b_placementHeader){
		b_placementHeader = true;
		b_creationHeader = false;

		//restore camera 
		app->m_camera.setPos(app->m_cameraPos);
		app->m_camera.restoreCameraProperties(app->m_cameraBck);
	}

	if(b_creationHeader){
		ImGui::BeginChild("creation", ImVec2(-1.0f, -1.0f), true, ImGuiWindowFlags_NoScrollWithMouse);
		{
			ImGui::Text("Object info");
			static float radius;
			ImGui::InputText("Name", app->m_currentCreating.name, 32);
			if(ImGui::Button("Set diffuse")){
				b_showOpenFileDialog = true;
				currentPath = "res/textures/";
				fdMode = FD_Mode::DIFF;
				updateDirContents();
			}
			ImGui::SameLine();
			ImGui::Text(app->m_currentCreating.diff.c_str());
			if(ImGui::Button("Set specular")){
				b_showOpenFileDialog = true;
				currentPath = "res/textures/";
				fdMode = FD_Mode::SPEC;
				updateDirContents();
			}
			ImGui::SameLine();
			ImGui::Text(app->m_currentCreating.spec.c_str());
			if(ImGui::Button("Set mesh")){
				b_showOpenFileDialog = true;
				currentPath = "res/models/";
				fdMode = FD_Mode::MESH;
				updateDirContents();
			}
			ImGui::SameLine();
			ImGui::Text(app->m_currentCreating.mesh.c_str());

			ImGui::Text("Object bounding box");

			ImGui::SliderFloat("Size x", &app->m_currentCreating.boxScale.x, 0.1f, 4.0f);
			ImGui::SliderFloat("Size y", &app->m_currentCreating.boxScale.y, 0.1f, 4.0f);
			ImGui::SliderFloat("Size z", &app->m_currentCreating.boxScale.z, 0.1f, 4.0f);

			ImGui::SliderAngle("Rot x", &app->m_currentCreating.boxRot.x, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Y", &app->m_currentCreating.boxRot.y, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Z", &app->m_currentCreating.boxRot.z, 0.0f, 359.0f);

			if(ImGui::Button("Save object")){
				b_showSaveFileDialog = true;
				currentPath = "res/gameobjects/";
				fdMode = FD_Mode::OBJECT_SAVE;
				updateDirContents();
			}
			if(ImGui::Button("Reset")){
				app->m_creatingModel = *(renderer::ResourceManager::getTexturedModelAt(0));
				app->m_currentCreating = CreatedObject();
			}
		}
		ImGui::EndChild();
	}


	if(b_placementHeader){
		ImGui::BeginChild("placement", ImVec2(), true);
		{
			ImGui::Text("place");
		}
		ImGui::EndChild();
	}

	ImGui::End();
}

void GUI::showOpenFileDialog(){
	ImGui::SetNextWindowSize(ImVec2(300, 11.2f * ImGui::GetFrameHeightWithSpacing()), ImGuiSetCond_Always);
	ImGui::SetNextWindowFocus();
	ImGui::Begin("Open file", &b_showOpenFileDialog,
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoCollapse
	);
	ImGui::PushItemWidth(-1);

	ImGui::Text("Available files:");
	ImGui::ListBox("##", &fdEntryItem, VectorOfStringGetter, (void*)(&dirContents), (int)(dirContents.size()), 10);
	ImGui::Text("\t\t\t");
	ImGui::SameLine();
	if(ImGui::Button("Open")){
		b_showOpenFileDialog = false;
		openButtonPressed();
	}
	ImGui::SameLine();
	if(ImGui::Button("Cancel")){
		b_showOpenFileDialog = false;
	}
	ImGui::End();
}

void GUI::showSaveFileDialog(){
	ImGui::SetNextWindowSize(ImVec2(300, 11.2f * ImGui::GetFrameHeightWithSpacing()), ImGuiSetCond_Always);
	ImGui::Begin("Save file", &b_showSaveFileDialog,
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoCollapse
	);
	ImGui::PushItemWidth(-1);

	ImGui::Text("Existing files:");

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("existingfiles", ImVec2(0, 150), true);
	for(auto dir : dirContents){
		ImGui::Text(dir.c_str());
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::Text("Name:");
	ImGui::SameLine();
	std::string filename;
	static char buf[32] = "";
	ImGui::InputText("##filename", buf, 32);
	ImGui::Text("\t\t\t");
	ImGui::SameLine();
	if(ImGui::Button("Save as")){
		bool exists = false;
		for(auto dir : dirContents){
			if(dir.compare(std::string(buf)) == 0){
				exists = true;
				break;
			}
		}
		if(!exists){
			memset(buf, '\0', 32);
			b_showSaveFileDialog = false;
			if(fdMode == FD_Mode::OBJECT_SAVE)
				app->saveCreatedObject(buf);
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Cancel")){
		b_showSaveFileDialog = false;
	}
	ImGui::End();
}

void GUI::openButtonPressed(){
	switch(fdMode){
	case FD_Mode::DIFF:
		app->m_currentCreating.diff = dirContents[fdEntryItem];
		app->m_creatingModel.getMaterial().setDiffuseMap(renderer::ResourceManager::getTexture("res/textures/" + dirContents[fdEntryItem]));
		break;
	case FD_Mode::SPEC:
		app->m_currentCreating.spec = dirContents[fdEntryItem];
		app->m_creatingModel.getMaterial().setSpecularMap(renderer::ResourceManager::getTexture("res/textures/" + dirContents[fdEntryItem]));
		break;
	case FD_Mode::MESH:
		app->m_currentCreating.mesh = dirContents[fdEntryItem];
		app->m_creatingModel.setMesh(renderer::ResourceManager::getMesh("res/models/" + dirContents[fdEntryItem]));
		break;
	case FD_Mode::MAP_OPEN:
		break;
	default:
		break;
	}
}

void GUI::updateDirContents(){
	dirContents.clear();
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(currentPath.c_str())) != NULL){
		while((ent = readdir(dir)) != NULL) {
			if(strncmp(ent->d_name, ".", 1) != 0){
				dirContents.push_back(ent->d_name);
			}
		}
		closedir(dir);
	} else {
		std::cout << "DIRENT::opendir '" << currentPath << "' error!" << std::endl;
	}
}


bool VectorOfStringGetter(void* data, int n, const char** out_text){
	std::vector<std::string>* v = (std::vector<std::string>*)data;
	*out_text = (*v)[n].c_str();
	return true;
}