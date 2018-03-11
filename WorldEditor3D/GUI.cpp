#include "GUI.h"
#include "MainApp.h"
#include <climits>
#include <dirent/dirent.h>


bool VectorOfStringGetter(void* data, int n, const char** out_text);
bool VectorOfObjectsGetter(void* data, int n, const char** out_text);

bool MapOfGameObjectsGetter(void* data, int n, const char** out_text);


GUI::GUI():
	app(nullptr)
{}

GUI::GUI(MainApp* app) :
	app(app),
	b_creationTab(false),
	b_placementTab(true),
	b_showOpenFileDialog(false),
	b_showSaveFileDialog(false),
	addGameobjectEntryItem(-1),
	placeGameobjectEntryItem(-1)
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
		ImGui::Checkbox("Billboard", &app->m_currentCreating.isBillboard);
		
		ImGui::Separator();
		ImGui::Text("\nObject bounding box");
		ImGui::Separator();
		
	/*	if(ImGui::Button("<##posxdec")) app->m_currentCreating.boxPos.x -= 0.05; ImGui::SameLine();
		ImGui::DragFloat("##Pos x", &app->m_currentCreating.boxPos.x, 1.0f, -300000.0f, 300000.0f); ImGui::SameLine();
		if(ImGui::Button(">##posxinc")) app->m_currentCreating.boxPos.x += 0.05;
		if(ImGui::Button("<##posydec")) app->m_currentCreating.boxPos.y -= 0.05; ImGui::SameLine();
		ImGui::DragFloat("##Pos Y", &app->m_currentCreating.boxPos.y, 1.0f, -300000.0f, 300000.0f); ImGui::SameLine();
		if(ImGui::Button(">##posyinc")) app->m_currentCreating.boxPos.y -= 0.05;
		if(ImGui::Button("<##poszdec")) app->m_currentCreating.boxPos.z -= 0.05; ImGui::SameLine();
		ImGui::DragFloat("##Pos Z", &app->m_currentCreating.boxPos.z, 1.0f, -300000.0f, 300000.0f); ImGui::SameLine();
		if(ImGui::Button(">##poszinc")) app->m_currentCreating.boxPos.z -= 0.05;*/
		
		ImGui::DragFloat("Pos x", &app->m_currentCreating.boxPos.x, 0.01f, -300000.0f, 300000.0f);
		ImGui::DragFloat("Pos Y", &app->m_currentCreating.boxPos.y, 0.01f, -300000.0f, 300000.0f);
		ImGui::DragFloat("Pos Z", &app->m_currentCreating.boxPos.z, 0.01f, -300000.0f, 300000.0f);
		ImGui::Separator();
		
		ImGui::SliderAngle("Rot x", &app->m_currentCreating.boxRot.x, 0.0f, 359.0f);
		ImGui::SliderAngle("Rot Y", &app->m_currentCreating.boxRot.y, 0.0f, 359.0f);
		ImGui::SliderAngle("Rot Z", &app->m_currentCreating.boxRot.z, 0.0f, 359.0f);
		ImGui::Separator();
		
		ImGui::DragFloat("Scale x", &app->m_currentCreating.boxScale.x, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Scale Y", &app->m_currentCreating.boxScale.y, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Scale Z", &app->m_currentCreating.boxScale.z, 0.01f, 0.01f, 10.0f);
		ImGui::Separator();

		ImGui::Text("\n");
		if(ImGui::Button("Save object")){
			b_showSaveFileDialog = true;
			currentPath = "res/gameobjects/";
			fdMode = FD_Mode::OBJECT_SAVE;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		if(ImGui::Button("Reset")){
			app->m_creatingModel = *(renderer::ResourceManager::loadModel("default"));
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
		if(ImGui::CollapsingHeader("Scene")){
			showSceneTab();
		}
		if(ImGui::CollapsingHeader("Add Object")){
			showAddObjectTab();
		}
		if(ImGui::CollapsingHeader("Add light")){

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
			if(fdMode == FD_Mode::OBJECT_SAVE){
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
	case FD_Mode::OBJECT_OPEN:
		break;
	default:
		break;
	}
}

void GUI::updateDirContents(std::vector<std::string>& directory){
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
	ImGui::Text("Available files:");
	ImGui::SameLine();
	if(ImGui::Button("Update")){
		currentPath = "res/gameobjects/";
		updateDirContents(dir_gameobjects);
	}
	
	ImGui::ListBox("##2", &addGameobjectEntryItem, VectorOfStringGetter, (void*)(&dir_gameobjects), (int)(dir_gameobjects.size()), 10);

	if(ImGui::Button("Add") && addGameobjectEntryItem >= 0){
		app->addNewObject(dir_gameobjects[addGameobjectEntryItem]);
		glm::vec3 pos = app->m_camera.getPos();
	}

}

void GUI::showSceneTab(){
reiterate:
	//objects in scene list
	ImGui::Text("Objects list:");
	ImGui::ListBox("##3", &placeGameobjectEntryItem, VectorOfObjectsGetter, (void*)(&app->m_objectsInScene), (int)(app->m_objectsInScene.size()), 10);
	//placement settings
	if(placeGameobjectEntryItem >= 0){
		//object info
		memset(m_name, '\0', OBJECT_NAME);
		strncat_s(m_name, app->m_objectsInScene[placeGameobjectEntryItem]->getName().c_str(), OBJECT_NAME);
		ImGui::InputText("Name", m_name, OBJECT_NAME);
		app->m_objectsInScene[placeGameobjectEntryItem]->setName(m_name);
		
		//object modifier
		if(ImGui::Button("Duplicate")){
			app->duplicateSelectedObject(placeGameobjectEntryItem);
			placeGameobjectEntryItem++;
			goto reiterate;
		}
		ImGui::SameLine();
		if(ImGui::Button("Remove")){
			app->removeSelectedObject(placeGameobjectEntryItem);
			placeGameobjectEntryItem--;
			goto reiterate;
		}
		//positional modifiers
		ImGui::Separator();
		renderer::GameObject* obj = app->m_objectsInScene[placeGameobjectEntryItem];	
		ImGui::DragFloat("Pos x", &obj->getPosition().x, 0.01f, -300000.0f, 300000.0f);
		ImGui::DragFloat("Pos Y", &obj->getPosition().y, 0.01f, -300000.0f, 300000.0f);
		ImGui::DragFloat("Pos Z", &obj->getPosition().z, 0.01f, -300000.0f, 300000.0f);
		ImGui::Text("\n");
		ImGui::SliderAngle("Rot x", &obj->getRotation().x, 0.0f, 359.0f);
		ImGui::SliderAngle("Rot Y", &obj->getRotation().y, 0.0f, 359.0f);
		ImGui::SliderAngle("Rot Z", &obj->getRotation().z, 0.0f, 359.0f);
		ImGui::Text("\n");
		ImGui::DragFloat("Scale x", &obj->getScale().x, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Scale Y", &obj->getScale().y, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Scale Z", &obj->getScale().z, 0.01f, 0.01f, 10.0f);
		ImGui::Text("\n");
	}
}


bool VectorOfStringGetter(void* data, int n, const char** out_text){
	std::vector<std::string>* v = (std::vector<std::string>*)data;
	*out_text = (*v)[n].c_str();
	return true;
}

bool VectorOfObjectsGetter(void * data, int n, const char ** out_text){
	std::vector<renderer::GameObject*>* v = (std::vector<renderer::GameObject*>*)data;
	*out_text = (*v)[n]->getName().c_str();
	return true;
}

bool MapOfGameObjectsGetter(void * data, int n, const char ** out_text){
	std::map<unsigned int, renderer::GameObject*>* m = (std::map<unsigned int, renderer::GameObject*>*)data;
	std::map<unsigned int, renderer::GameObject*>::iterator it;
	int i = 0;
	for(it = m->begin(); i <= n; it++){
		i++;
		*out_text = it->second->getName().c_str();
	}
	return true;
}
