#include "GUI.h"
#include "MainApp.h"
#include <climits>
#include <dirent/dirent.h>


bool VectorOfStringGetter(void* data, int n, const char** out_text);
bool VectorOfObjectsGetter(void* data, int n, const char** out_text);
bool VectorOfShapesGetter(void* data, int n, const char** out_text);

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
	placeGameobjectEntryItem(-1),
	collisionBodyEntryItem(-1),
	m_moveInc(0.01f)
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
		ImGui::Text("\nObject collision");
		
		ImGui::ListBox("##3", &collisionBodyEntryItem, VectorOfShapesGetter, (void*)(&collisionBodies), (int)(collisionBodies.size()), 5);
		
		static int collisionBodyIndex = renderer::ResourceManager::CollisionShapes::SHAPE_CUBE;
		ImGui::Combo("Shape", &collisionBodyIndex, "Cube\0Sphere\0Cilinder\0Cone\0Capsule\0\0");
		if(ImGui::Button("Add##shape")){
			std::string shape = std::string(renderer::ResourceManager::IndexToShape(collisionBodyIndex));
			collisionBodies.push_back(collisionBodyIndex);
			renderer::CollisionBody body;
			body.shape = collisionBodyIndex;
			app->m_currentCreating.colBodies.push_back(body);
		}
		ImGui::SameLine();
		if(ImGui::Button("Remove##shape") && collisionBodyEntryItem != -1){
			collisionBodies.erase(collisionBodies.begin() + collisionBodyEntryItem);
			app->m_currentCreating.colBodies.erase(app->m_currentCreating.colBodies.begin() + collisionBodyEntryItem);
			collisionBodyEntryItem--;
		}

//		ImGui::Separator();
		if(collisionBodyEntryItem != -1){
			renderer::CollisionBody& body = app->m_currentCreating.colBodies[collisionBodyEntryItem];

			ImGui::DragFloat("Pos x", &body.colRelativePos.x, 0.01f, -300000.0f, 300000.0f);
			ImGui::DragFloat("Pos Y", &body.colRelativePos.y, 0.01f, -300000.0f, 300000.0f);
			ImGui::DragFloat("Pos Z", &body.colRelativePos.z, 0.01f, -300000.0f, 300000.0f);
			ImGui::Separator();

			ImGui::SliderAngle("Rot x", &body.colRot.x, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Y", &body.colRot.y, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Z", &body.colRot.z, 0.0f, 359.0f);
			ImGui::Separator();	

			ImGui::DragFloat("Scale x", &body.colScale.x, 0.01f, 0.01f, 10.0f);
			ImGui::DragFloat("Scale Y", &body.colScale.y, 0.01f, 0.01f, 10.0f);
			ImGui::DragFloat("Scale Z", &body.colScale.z, 0.01f, 0.01f, 10.0f);
			ImGui::Separator();
		}

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
			collisionBodyEntryItem = -1;
			collisionBodies.clear();
		}
		ImGui::SameLine();
		if(ImGui::Button("Edit")){
			b_showOpenFileDialog = true;
			currentPath = "res/gameobjects/";
			fdMode = FD_Mode::OBJECT_OPEN;
			updateDirContents(dirContents);
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
		ImGui::Separator();
		if(ImGui::Button("Save")){
			b_showSaveFileDialog = true;
			currentPath = "res/maps/";
			fdMode = FD_Mode::MAP_SAVE;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		if(ImGui::Button("Open")){
			b_showOpenFileDialog = true;
			currentPath = "res/maps/";
			fdMode = FD_Mode::MAP_OPEN;
			updateDirContents(dirContents);
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
				//exists = true;
				break;
			}
		}
		if(!exists){
			b_showSaveFileDialog = false;
			if(fdMode == FD_Mode::OBJECT_SAVE){
				app->saveCreatedObject(buf);
			} else if(fdMode == FD_Mode::MAP_SAVE){
				app->saveMap(buf);
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
		app->openMap(dirContents[fdEntryItem]);
		break;
	case FD_Mode::OBJECT_OPEN:
		app->openCreatedObject(dirContents[fdEntryItem]);
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
	ImGui::ListBox("##2", &addGameobjectEntryItem, VectorOfStringGetter, (void*)(&dir_gameobjects), (int)(dir_gameobjects.size()), 10);
	if(ImGui::Button("Add") && addGameobjectEntryItem >= 0){
		app->addNewObject(dir_gameobjects[addGameobjectEntryItem]);
		placeGameobjectEntryItem = app->m_objectsInScene.size() - 1;
		glm::vec3 pos = app->m_camera.getPos();
	}
	ImGui::SameLine();
	if(ImGui::Button("Update")){
		currentPath = "res/gameobjects/";
		updateDirContents(dir_gameobjects);
	}

}

void GUI::showSceneTab(){
reiterate:
	//objects in scene list
	ImGui::Text("Objects list:");
	ImGui::ListBox("##3", &placeGameobjectEntryItem, VectorOfObjectsGetter, (void*)(&app->m_objectsInScene), (int)(app->m_objectsInScene.size()), 10);
	//placement settings
	if(placeGameobjectEntryItem >= 0){
		if(app->m_currentlySelectedObject)
			app->m_currentlySelectedObject->setSelected(false);
		app->m_currentlySelectedObject = app->m_objectsInScene[placeGameobjectEntryItem];
		app->m_currentlySelectedObject->setSelected(true);
		//object info
		memset(m_name, '\0', OBJECT_NAME);
		strncat_s(m_name, app->m_objectsInScene[placeGameobjectEntryItem]->getInEditorName().c_str(), OBJECT_NAME);
		ImGui::InputText("Name", m_name, OBJECT_NAME);
		app->m_objectsInScene[placeGameobjectEntryItem]->setInEditorName(m_name);
		
		//object modifier
		if(ImGui::Button("Duplicate")){
			app->duplicateSelectedObject(placeGameobjectEntryItem);
			placeGameobjectEntryItem = app->m_objectsInScene.size()-1;
			goto reiterate;
		}
		ImGui::SameLine();
		if(ImGui::Button("Remove")){
			app->removeSelectedObject(placeGameobjectEntryItem);
			placeGameobjectEntryItem = -1;
			goto reiterate;
		}
		//positional modifiers
		int gizmoMode = app->m_gizmos.getGizmoMode();
		ImGui::Separator();
		ImGui::Text("Transforms");
		ImGui::DragFloat("Step", &m_moveInc, 0.01f, 0.01f, 5.0f);
		renderer::GameObject* obj = app->m_objectsInScene[placeGameobjectEntryItem];
		ImGui::RadioButton("Move", &gizmoMode, (int)renderer::GizmoMode::TRANSLATE); ImGui::SameLine();
		ImGui::RadioButton("Scale", &gizmoMode, (int)renderer::GizmoMode::SCALE);	 ImGui::SameLine();
		ImGui::RadioButton("Rotate", &gizmoMode, (int)renderer::GizmoMode::ROTATE);	 ImGui::SameLine();
		ImGui::RadioButton("Off", &gizmoMode, (int)renderer::GizmoMode::NONE);
		app->m_gizmos.setGizmoMode(gizmoMode);

			/*ImGui::DragFloat("Pos x", &obj->getPosition().x, m_moveInc, -300.0f, 300.0f);
			ImGui::DragFloat("Pos Y", &obj->getPosition().y, m_moveInc, -300.0f, 300.0f);
			ImGui::DragFloat("Pos Z", &obj->getPosition().z, m_moveInc, -300.0f, 300.0f);
			ImGui::Text("\n");
			ImGui::SliderAngle("Rot x", &obj->getRotation().x, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Y", &obj->getRotation().y, 0.0f, 359.0f);
			ImGui::SliderAngle("Rot Z", &obj->getRotation().z, 0.0f, 359.0f);
			ImGui::Text("\n");
			ImGui::DragFloat("Scale x", &obj->getScale().x, 0.01f, 0.01f, 10.0f);
			ImGui::DragFloat("Scale Y", &obj->getScale().y, 0.01f, 0.01f, 10.0f);
			ImGui::DragFloat("Scale Z", &obj->getScale().z, 0.01f, 0.01f, 10.0f);
			ImGui::Text("\n");*/
	}
}


bool VectorOfStringGetter(void* data, int n, const char** out_text){
	std::vector<std::string>* v = (std::vector<std::string>*)data;
	*out_text = (*v)[n].c_str();
	return true;
}

bool VectorOfObjectsGetter(void * data, int n, const char ** out_text){
	std::vector<renderer::GameObject*>* v = (std::vector<renderer::GameObject*>*)data;
	*out_text = (*v)[n]->getInEditorName().c_str();
	return true;
}

bool VectorOfShapesGetter(void * data, int n, const char ** out_text){
	std::vector<int>* v = (std::vector<int>*)data;
	*out_text = renderer::ResourceManager::IndexToShape((*v)[n]);
		
	return true;
}

bool MapOfGameObjectsGetter(void * data, int n, const char ** out_text){
	std::map<unsigned int, renderer::GameObject*>* m = (std::map<unsigned int, renderer::GameObject*>*)data;
	std::map<unsigned int, renderer::GameObject*>::iterator it;
	int i = 0;
	for(it = m->begin(); i <= n; it++){
		i++;
		*out_text = it->second->getInEditorName().c_str();
	}
	return true;
}
