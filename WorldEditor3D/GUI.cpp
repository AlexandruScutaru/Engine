#include "GUI.h"
#include "MainApp.h"

#include <dirent/dirent.h>


bool VectorOfStringGetter(void* data, int n, const char** out_text);


GUI::GUI():
	app(nullptr)
{}

GUI::GUI(MainApp* app):
	app(app),
	b_creationTab(false),
	b_placementTab(true),
	b_showOpenFileDialog(false),
	b_showSaveFileDialog(false),
	creation_object_type(0),
	m_combo_objectType("Static"),
	gameobjectEntryItem(-1)
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
	if(ImGui::Button("Object Creation") && !b_creationTab){
		b_creationTab = true;
		b_placementTab = false;

		//backup and set new camera transforms
		app->m_cameraPos = app->m_camera.getPos();
		app->m_cameraBck = app->m_camera.backupCameraProperties();
		app->m_camera.setPos(glm::vec3(0.0f, 0.0f, 5.0f));
		app->m_camera.setPitch(0.0f);
		app->m_camera.setYaw(-90.0f);
	}
	ImGui::SameLine();
	//gameobject placement
	if(ImGui::Button("Object Placement") && !b_placementTab){
		b_placementTab = true;
		b_creationTab = false;

		//restore camera 
		app->m_camera.setPos(app->m_cameraPos);
		app->m_camera.restoreCameraProperties(app->m_cameraBck);
	}

	if(b_creationTab) showCreationTab();
	if(b_placementTab) showPlacementTab();

	ImGui::End();
}

void GUI::showCreationTab(){
	ImGui::BeginChild("creation", ImVec2(-1.0f, -1.0f), true, ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImGui::Text("Object info");
		static float radius;
		if(ImGui::Button("Set diffuse")){
			b_showOpenFileDialog = true;
			currentPath = "res/textures/";
			fdMode = FD_Mode::DIFF;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		ImGui::Text(app->m_currentCreating.diff.c_str());
		if(ImGui::Button("Set specular")){
			b_showOpenFileDialog = true;
			currentPath = "res/textures/";
			fdMode = FD_Mode::SPEC;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		ImGui::Text(app->m_currentCreating.spec.c_str());
		if(ImGui::Button("Set mesh")){
			b_showOpenFileDialog = true;
			currentPath = "res/models/";
			fdMode = FD_Mode::MESH;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		ImGui::Text(app->m_currentCreating.mesh.c_str());
		ImGui::Separator();
		ImGui::Text("\nObject type");
		ImGui::RadioButton("static", &creation_object_type, 0); ImGui::SameLine();
		ImGui::RadioButton("entity", &creation_object_type, 1); ImGui::SameLine();
		ImGui::RadioButton("billboard", &creation_object_type, 2);
		
		ImGui::Separator();
		ImGui::Text("\nObject bounding box");
		ImGui::SliderFloat("Size x", &app->m_currentCreating.boxScale.x, 0.1f, 4.0f);
		ImGui::SliderFloat("Size y", &app->m_currentCreating.boxScale.y, 0.1f, 4.0f);
		ImGui::SliderFloat("Size z", &app->m_currentCreating.boxScale.z, 0.1f, 4.0f);

		ImGui::SliderAngle("Rot x", &app->m_currentCreating.boxRot.x, 0.0f, 359.0f);
		ImGui::SliderAngle("Rot Y", &app->m_currentCreating.boxRot.y, 0.0f, 359.0f);
		ImGui::SliderAngle("Rot Z", &app->m_currentCreating.boxRot.z, 0.0f, 359.0f);
		
		ImGui::Separator();
		ImGui::Text("\n");
		if(ImGui::Button("Save object")){
			b_showSaveFileDialog = true;
			if(creation_object_type == 0){
				currentPath = "res/gameobjects/static/";
				fdMode = FD_Mode::STATIC_OBJECT_SAVE;

			} else if(creation_object_type == 1){
				currentPath = "res/gameobjects/entities/";
				fdMode = FD_Mode::ENTITY_OBJECT_SAVE;
			}
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		if(ImGui::Button("Reset")){
			app->m_creatingModel = *(renderer::ResourceManager::getTexturedModelAt(0));
			app->m_currentCreating = CreatedObject();
		}
		ImGui::SameLine();
		if(ImGui::Button("Edit")){

		}
	}
	ImGui::EndChild();
}

void GUI::showPlacementTab(){
	ImGui::BeginChild("placement", ImVec2(), true);
	{
		if(ImGui::CollapsingHeader("Add Object")){
			showAddObjectTab();
		}
		if(ImGui::CollapsingHeader("Add light")){

		}
		if(ImGui::CollapsingHeader("Transforms")){

		}
	}
	ImGui::EndChild();
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
			b_showSaveFileDialog = false;
			if(fdMode == FD_Mode::STATIC_OBJECT_SAVE ||
			   fdMode == FD_Mode::ENTITY_OBJECT_SAVE)
			{
				app->saveCreatedObject(buf);
			}
		}
		memset(buf, '\0', 32);
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
	case FD_Mode::STATIC_OBJECT_OPEN:
		break;
	case FD_Mode::ENTITY_OBJECT_OPEN:
		break;
	default:
		break;
	}
}

void GUI::updateDirContents(std::vector<std::string>& directory){
	//dirContents.clear();
	directory.clear();
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(currentPath.c_str())) != NULL){
		while((ent = readdir(dir)) != NULL) {
			if(strncmp(ent->d_name, ".", 1) != 0){
				directory.push_back(ent->d_name);
			}
		}
		closedir(dir);
	} else {
		std::cout << "DIRENT::opendir '" << currentPath << "' error!" << std::endl;
	}
}

void GUI::showAddObjectTab(){
	const char* categories[] = {"Static", "Entity"};
	static int currentEntry;
	if(ImGui::BeginCombo("Type", m_combo_objectType)){
		for(int i = 0; i < IM_ARRAYSIZE(categories); i++){
			bool is_selected = (m_combo_objectType == categories[i]);
			if(ImGui::Selectable(categories[i], is_selected)){
				m_combo_objectType = categories[i];
				currentEntry = i;
			}
			if(is_selected){
				ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Text("Available files:");
	ImGui::SameLine();
	if(ImGui::Button("Update")){
		if(currentEntry == 0)
			currentPath = "res/gameobjects/entities/";
		else
			currentPath = "res/gameobjects/static/";
		updateDirContents(availableEntities);
	}

	ImGui::ListBox("##2", &gameobjectEntryItem, VectorOfStringGetter, (void*)(&availableEntities), (int)(availableEntities.size()), 10);

	if(ImGui::Button("Add")){
		std::cout << gameobjectEntryItem << std::endl;
	}

}


bool VectorOfStringGetter(void* data, int n, const char** out_text){
	std::vector<std::string>* v = (std::vector<std::string>*)data;
	*out_text = (*v)[n].c_str();
	return true;
}