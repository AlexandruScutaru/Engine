#include "GUI.h"
#include "MainApp.h"
#include <climits>
#include <dirent/dirent.h>
#include <cfloat>

bool VectorOfStringGetter(void* data, int n, const char** out_text);
bool VectorOfObjectsGetter(void* data, int n, const char** out_text);
bool VectorOfShapesGetter(void* data, int n, const char** out_text);
bool VectorOfLightsGetter(void* data, int n, const char** out_text);


GUI::GUI():
	app(nullptr)
{}

GUI::GUI(MainApp* app) :
	app(app),
	b_creationTab(false),
	b_placementTab(true),
	b_showOpenFileDialog(false),
	b_showSaveFileDialog(false),
	placedGameobjectEntryItem(-1),
	placedLightEntryItem(-1),
	collisionBodyEntryItem(-1),
	m_moveInc(0.01f)
{
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().ChildRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;
	ImGui::GetStyle().GrabRounding = 0.0f;
	ImGui::GetStyle().PopupRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;
	//ImGui::GetStyle().WindowBorderSize = 1.0f;
}

GUI::~GUI(){}


void GUI::updateImGuiWindows(){
	ImGui_ImplSdlGL3_NewFrame(app->m_window.getWindow());

	showMainMenu();
	showToolbar();
	showEditorWindow();
	if(b_showOpenFileDialog) showOpenFileDialog();
	if(b_showSaveFileDialog) showSaveFileDialog();
}

void GUI::showMainMenu(){
	if(ImGui::BeginMainMenuBar()){
		if(ImGui::BeginMenu("File")){
			if(ImGui::MenuItem("New")) {
				app->resetData();
				app->addDefaultLighting();
			}
			if(ImGui::MenuItem("Open")) {
				b_showOpenFileDialog = true;
				currentPath = "res/maps/";
				fdMode = FD_Mode::MAP_OPEN;
				updateDirContents(dirContents);
			}
			if(ImGui::MenuItem("Save")){
				b_showSaveFileDialog = true;
				currentPath = "res/maps/";
				fdMode = FD_Mode::MAP_SAVE;
				updateDirContents(dirContents);
			}
			ImGui::Separator();
			if(ImGui::MenuItem("Exit")) { 
				app->m_appState = AppState::EXIT; 
			}
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit")){
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void GUI::showToolbar(){
	static ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoTitleBar |
											ImGuiWindowFlags_NoScrollbar |
											ImGuiWindowFlags_NoMove |
											ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowSize(ImVec2((float)renderer::Window::getW(), TOOLBAR_HEIGHT), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(0.0f, MAIN_MENU_HEIGHT), ImGuiCond_Once);

	ImGui::Begin("##toolbar", NULL, toolbar_flags);

	renderer::TextureData td;
	///gameobject and lights placement
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

	//add point light
	td = *utilities::ResourceManager::getTexture("res/gui/add-pointLight.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		app->addPointLight();
	}

	//add gameobject
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/add-gameobject.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		b_showOpenFileDialog = true;
		currentPath = "res/gameobjects/";
		fdMode = FD_Mode::ADD_GAMEOBJECT;
		updateDirContents(dirContents);
	}
	//duplicate gameobject
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/duplicate.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		if(placedGameobjectEntryItem >= 0){
			//object selected
			app->duplicateSelectedObject(placedGameobjectEntryItem);
		} else if(placedLightEntryItem > 1){
			//point light selected
			app->duplicatePointLight(placedLightEntryItem);
		}
	}
	//remove gameobject
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/remove.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		if(placedGameobjectEntryItem >= 0){
			//remove object
			app->removeSelectedObject(placedGameobjectEntryItem);
			placedGameobjectEntryItem = -1;
		} else if(placedLightEntryItem > 1){
			//remove point light
			app->removePointLight(placedLightEntryItem);
			placedLightEntryItem = -1;
		}
	}

	ImGui::PopStyleColor();
	///separator
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/sep.png");
	ImGui::Image((ImTextureID)td.id, ImVec2(3, 27), ImVec2(0, 0), ImVec2(1, -1));

	///transform gizmo mode selectors
	//none - select only
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/select.png");
	if(app->m_gizmos.getGizmoMode() == (int)GizmoMode::NONE)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(200, 200, 200, 255));
	else
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1)){
		app->m_gizmos.setGizmoMode((int)GizmoMode::NONE);
	}
	ImGui::PopStyleColor();
	//translate
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/move.png");
	if(app->m_gizmos.getGizmoMode() == (int)GizmoMode::TRANSLATE)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(200, 200, 200, 255));
	else
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1)){
		app->m_gizmos.setGizmoMode((int)GizmoMode::TRANSLATE);
	}
	ImGui::PopStyleColor();
	//rotate
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/rotate.png");
	if(app->m_gizmos.getGizmoMode() == (int)GizmoMode::ROTATE)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(200, 200, 200, 255));
	else
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1)){
		app->m_gizmos.setGizmoMode((int)GizmoMode::ROTATE);
	}
	ImGui::PopStyleColor();
	//scale
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/scale.png");
	if(app->m_gizmos.getGizmoMode() == (int)GizmoMode::SCALE)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(200, 200, 200, 255));
	else
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1)){
		app->m_gizmos.setGizmoMode((int)GizmoMode::SCALE);
	}
	ImGui::PopStyleColor();

	///separator
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/gui/sep.png");
	ImGui::Image((ImTextureID)td.id, ImVec2(3, 27), ImVec2(0, 0), ImVec2(1, -1));

	///Grid Size selection
	ImGui::SameLine();
	ImGui::Text("Grid size:");
	ImGui::SameLine();
	static int it = 0;
	ImGui::PushItemWidth(60);
	ImGui::Combo("##incStepCombo", &it, " 0.01 \0 0.10 \0 1.00 \0\0", 3);
	switch(it){
	case 0:
		app->m_gizmos.gridStep = 0.01f;
		break;
	case 1:
		app->m_gizmos.gridStep = 0.1f;
		break;
	case 2:
		app->m_gizmos.gridStep = 1.0f;
		break;
	default:
		break;
	}
	ImGui::PopItemWidth();

	ImGui::End();
}

void GUI::showEditorWindow(){
	static ImGuiWindowFlags editorWindow_flags = ImGuiWindowFlags_NoMove |
												 ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowSize(ImVec2(EDITOR_WINDOW_WIDTH, (float)app->m_window.getH() - MAIN_MENU_HEIGHT - TOOLBAR_HEIGHT), ImGuiCond_Always);
	ImGui::SetNextWindowPos (ImVec2((float)app->m_window.getW() - EDITOR_WINDOW_WIDTH, MAIN_MENU_HEIGHT + TOOLBAR_HEIGHT), ImGuiSetCond_Always);

	ImGui::Begin("##editorWindow", NULL, editorWindow_flags);

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.8f);

	///main tabs for the editor
	//gameobject creation
	if(ImGui::Button("Object Creation") && !b_creationTab){
		b_creationTab = true;
		b_placementTab = false;

		//backup and set new camera transforms
		app->m_player.backup();
		app->m_player.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
		app->m_player.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
		app->m_player.setFlashlight(false);
	}
	ImGui::SameLine();
	//gameobject placement
	if(ImGui::Button("Object Placement") && !b_placementTab){
		b_placementTab = true;
		b_creationTab = false;

		//restore camera 
		app->m_player.restore();
	}

	if(b_creationTab) showCreationTab();
	if(b_placementTab) showPlacementTab();

	ImGui::End();
}

void GUI::showCreationTab(){
	ImGui::BeginChild("creation", ImVec2(-1.0f, -1.0f), true/*, ImGuiWindowFlags_NoScrollWithMouse*/);
	{
		ImGui::PushItemWidth(-1);
		ImGui::Text("Object info");
		static float radius;
		if(ImGui::Button("Set diff")){
			b_showOpenFileDialog = true;
			currentPath = "res/textures/";
			fdMode = FD_Mode::DIFF;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		ImGui::Text(app->m_creationTabGameObject.getDiffName().c_str());
		if(ImGui::Button("Set spec")){
			b_showOpenFileDialog = true;
			currentPath = "res/textures/";
			fdMode = FD_Mode::SPEC;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		ImGui::Text(app->m_creationTabGameObject.getSpecName().c_str());
		if(ImGui::Button("Set mesh")){
			b_showOpenFileDialog = true;
			currentPath = "res/models/";
			fdMode = FD_Mode::MESH;
			updateDirContents(dirContents);
		}
		ImGui::SameLine();
		ImGui::Text(app->m_creationTabGameObject.getMeshName().c_str());
		
		ImGui::Separator();
		ImGui::Text("\nObject type");
		ImGui::Checkbox("Billboard", &(app->m_creationTabGameObject.isBillboardRef()));
		
		ImGui::Separator();
		ImGui::Text("\nObject collision");
		
		ImGui::ListBox("##collisionBodiesListBox", &collisionBodyEntryItem, VectorOfShapesGetter, (void*)(&collisionBodies), (int)(collisionBodies.size()), 5);
		
		static int collisionBodyIndex = utilities::ResourceManager::CollisionShapes::SHAPE_CUBE;
		ImGui::Combo("##collisionBodyShape", &collisionBodyIndex, "Cube\0Sphere\0Cilinder\0Cone\0Capsule\0\0");
		if(ImGui::Button("Add##collisionShape")){
			std::string shape = std::string(utilities::ResourceManager::IndexToShape(collisionBodyIndex));
			collisionBodies.push_back(collisionBodyIndex);
			renderer::CollisionBody body;
			body.shape = collisionBodyIndex;
			app->m_creationTabGameObject.addColBody(body);
		}
		ImGui::SameLine();
		if(ImGui::Button("Remove##collisionShape") && collisionBodyEntryItem != -1){
			collisionBodies.erase(collisionBodies.begin() + collisionBodyEntryItem);
			app->m_creationTabGameObject.removeColBody(collisionBodyEntryItem);
			collisionBodyEntryItem--;
		}

		if(collisionBodyEntryItem != -1){
			renderer::CollisionBody& body = app->m_creationTabGameObject.getColBodies()[collisionBodyEntryItem];
			ImGui::BeginChild("colls", ImVec2(-1, 65), false);
			{
				ImGui::PushItemWidth(70);
				ImGui::Text("P:");
				ImGui::SameLine();
				ImGui::DragFloat("##dragPosX", &body.colRelativePos.x, 0.01f, -100.0f, 100.0f);
				ImGui::SameLine();
				ImGui::DragFloat("##dragPosY", &body.colRelativePos.y, 0.01f, -100.0f, 100.0f);
				ImGui::SameLine();
				ImGui::DragFloat("##dragPosZ", &body.colRelativePos.z, 0.01f, -100.0f, 100.0f);

				ImGui::Text("R:");
				ImGui::SameLine();
				body.colRot.x = glm::degrees(body.colRot.x);
				ImGui::DragFloat("##dragRotX", &body.colRot.x, 1.0f, 0.0f, 359.0f, "%.2f"); 
				body.colRot.x = glm::radians(body.colRot.x);
				ImGui::SameLine();
				body.colRot.y = glm::degrees(body.colRot.y);
				ImGui::DragFloat("##dragRotY", &body.colRot.y, 1.0f, 0.0f, 359.0f, "%.2f"); 
				body.colRot.y = glm::radians(body.colRot.y);
				ImGui::SameLine();
				body.colRot.z = glm::degrees(body.colRot.z);
				ImGui::DragFloat("##dragRotZ", &body.colRot.z, 1.0f, 0.0f, 359.0f, "%.2f"); 
				body.colRot.z = glm::radians(body.colRot.z);
				
				ImGui::Text("S:");
				ImGui::SameLine();
				ImGui::DragFloat("##dragScaleX", &body.colScale.x, 0.01f, 0.01f, 100.0f);
				ImGui::SameLine();						  
				ImGui::DragFloat("##dragScaleY", &body.colScale.y, 0.01f, 0.01f, 100.0f);
				ImGui::SameLine();						  
				ImGui::DragFloat("##dragScaleZ", &body.colScale.z, 0.01f, 0.01f, 100.0f);
				
				ImGui::PopItemWidth();
			}
			ImGui::EndChild();
		}
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
			app->m_creationTabGameObject = GameObject(utilities::ResourceManager::loadModel("default"));
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
	ImGui::BeginChild("scene", ImVec2(), true);
	{
		if(ImGui::CollapsingHeader("Gameobjects"))
			showGameobjectsTab();
		if(ImGui::CollapsingHeader("Lights"))
			showLightsTab();
	}
	ImGui::EndChild();
}

void GUI::showOpenFileDialog(){
	ImGui::SetNextWindowSize(ImVec2(500, 11.2f * ImGui::GetFrameHeightWithSpacing()), ImGuiSetCond_Always);
	ImGui::SetNextWindowFocus();
	ImGui::Begin("Open file", &b_showOpenFileDialog,
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoCollapse
	);

	ImGui::BeginChild("##fileSelecting", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f, 0), false);
	{
		ImGui::PushItemWidth(-1);
		ImGui::Text("Available files:");
		ImGui::ListBox("##", &fdEntryItem, VectorOfStringGetter, (void*)(&dirContents), (int)(dirContents.size()), 10);
		
		//ImGui::Text("\t\t\t\t\t");
		//ImGui::SameLine();
		if(ImGui::Button("Open")){
			b_showOpenFileDialog = false;
			openButtonPressed();
		}
		ImGui::SameLine();
		if(ImGui::Button("Cancel")){
			b_showOpenFileDialog = false;
		}

		ImGui::PopItemWidth();
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("##Preview", ImVec2(0, ImGui::GetWindowHeight()/2), false);
	{
		ImGui::PushItemWidth(-1);
		ImGui::Text("placeholder");
		//ImGui::Image()
		ImGui::PopItemWidth();
	}
	ImGui::EndChild();

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
	ImGui::PopItemWidth();
	ImGui::End();
}

void GUI::openButtonPressed(){
	switch(fdMode){
	case FD_Mode::DIFF:
		app->m_creationTabGameObject.setDiffName(dirContents[fdEntryItem]);
		app->m_creationTabGameObject.getTexturedModel()->getMaterial().setDiffuseMap(utilities::ResourceManager::getTexture("res/textures/" + dirContents[fdEntryItem]));
		break;
	case FD_Mode::SPEC:
		app->m_creationTabGameObject.setSpecName(dirContents[fdEntryItem]);
		app->m_creationTabGameObject.getTexturedModel()->getMaterial().setSpecularMap(utilities::ResourceManager::getTexture("res/textures/" + dirContents[fdEntryItem]));
		break;
	case FD_Mode::MESH:
		app->m_creationTabGameObject.setMeshName(dirContents[fdEntryItem]);
		app->m_creationTabGameObject.getTexturedModel()->setMesh(utilities::ResourceManager::getMesh("res/models/" + dirContents[fdEntryItem]));
		break;
	case FD_Mode::MAP_OPEN:
		app->openMap(dirContents[fdEntryItem]);
		break;
	case FD_Mode::OBJECT_OPEN:
		app->openCreatedObject(dirContents[fdEntryItem]);
		break;
	case FD_Mode::ADD_GAMEOBJECT:
		app->addNewObject(dirContents[fdEntryItem]);
		placedGameobjectEntryItem = app->m_objectsInScene.size() - 1;
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

void GUI::showGameobjectsTab(){
	//objects in scene list
	ImGui::PushItemWidth(-1);
	ImGui::Text("Gameobjects in scene:");
	ImGui::ListBox("##gameobjectsList", &placedGameobjectEntryItem, VectorOfObjectsGetter, (void*)(&app->m_objectsInScene), (int)(app->m_objectsInScene.size()), 10);
	//placement settings
	if(placedGameobjectEntryItem >= 0){
		if(app->m_currentlySelectedObject)
			app->m_currentlySelectedObject->setSelected(false);
		app->m_currentlySelectedLight = nullptr;
		placedLightEntryItem = -1;

		app->m_currentlySelectedObject = app->m_objectsInScene[placedGameobjectEntryItem];
		app->m_currentlySelectedObject->setSelected(true);
		//object info
		memset(m_name, '\0', OBJECT_NAME_SIZE);
		strncat_s(m_name, app->m_objectsInScene[placedGameobjectEntryItem]->getInEditorName().c_str(), OBJECT_NAME_SIZE);
		ImGui::InputText("##gameobjectName", m_name, OBJECT_NAME_SIZE);
		app->m_objectsInScene[placedGameobjectEntryItem]->setInEditorName(m_name);
		
		ImGui::Separator();
		ImGui::Text("Transforms");
		GameObject* obj = app->m_objectsInScene[placedGameobjectEntryItem];
		ImGui::BeginChild("gmaeobjectTransoforms", ImVec2(-1, 65), false);
		{
			ImGui::PushItemWidth(70);
			ImGui::Text("P:");
			ImGui::SameLine();
			ImGui::InputFloat("##objInputPosX", &obj->getPosition().x, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputPosY", &obj->getPosition().y, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputPosZ", &obj->getPosition().z, 0.0f, 0.0f, 2);

			ImGui::Text("R:");
			ImGui::SameLine();
			ImGui::InputFloat("##objInputRotX", &obj->getRotation().x, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputRotY", &obj->getRotation().y, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputRotZ", &obj->getRotation().z, 0.0f, 0.0f, 2);

			ImGui::Text("S:");
			ImGui::SameLine();
			ImGui::InputFloat("##objInputScaleX", &obj->getScale().x, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputScaleY", &obj->getScale().y, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputScaleZ", &obj->getScale().z, 0.0f, 0.0f, 2);

			ImGui::PopItemWidth();
		}
		ImGui::EndChild();
	}
	ImGui::PopItemWidth();
}

void GUI::showLightsTab(){
	//lights in scene list
	ImGui::PushItemWidth(-1);
	ImGui::Text("Lights in scene:");
	ImGui::ListBox("##lightsList", &placedLightEntryItem, VectorOfLightsGetter, (void*)(&app->m_lights), (int)(app->m_lights.size()), 10);
	
	//lights settings
	ImGui::Separator();
	ImGui::BeginChild("gmaeobjectTransoforms", ImVec2(-1, -1), false);
	{
		if(placedLightEntryItem >= 0){
			ImGui::Text("Properties");
			ImGui::PushItemWidth(65);
			if(app->m_currentlySelectedObject)
				app->m_currentlySelectedObject->setSelected(false);
			app->m_currentlySelectedObject = nullptr;
			app->m_currentlySelectedLight = nullptr;
			placedGameobjectEntryItem = -1;
			for(auto entry : app->m_billboardLightsMap){
				if(entry.second == app->m_lights[placedLightEntryItem]){
					app->m_currentlySelectedObject = entry.first;
					app->m_currentlySelectedObject->setSelected(true);
					app->m_currentlySelectedLight = app->m_lights[placedLightEntryItem];
					break;
				}
			}
			if(placedLightEntryItem == 0){
				//directional light
				renderer::DirLight* dl = static_cast<renderer::DirLight*>(app->m_lights[0]);
				ImGui::Text("Amb :");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbR", &dl->ambient.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbG", &dl->ambient.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbB", &dl->ambient.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Diff:");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffR", &dl->diffuse.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffG", &dl->diffuse.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffB", &dl->diffuse.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Spec:");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecR", &dl->specular.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecG", &dl->specular.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecB", &dl->specular.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Dir :");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirX", &app->m_currentlySelectedObject->getPosition().x, 0.01f, FLT_MIN, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirY", &app->m_currentlySelectedObject->getPosition().y, 0.01f, FLT_MIN, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirZ", &app->m_currentlySelectedObject->getPosition().z, 0.01f, FLT_MIN, FLT_MAX, "%.2f");
			}
			else if(placedLightEntryItem == 1){
				//spot light - doesnt have a billboard mapped to it
				if(app->m_currentlySelectedObject)
					app->m_currentlySelectedObject->setSelected(false);
				app->m_currentlySelectedObject = nullptr;
				app->m_currentlySelectedLight = nullptr;
				placedGameobjectEntryItem = -1;

				renderer::SpotLight* sl = static_cast<renderer::SpotLight*>(app->m_lights[1]);
				ImGui::Text("Amb :");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbR", &sl->ambient.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbG", &sl->ambient.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbB", &sl->ambient.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Diff:");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffR", &sl->diffuse.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffG", &sl->diffuse.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffB", &sl->diffuse.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Spec:");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecR", &sl->specular.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecG", &sl->specular.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecB", &sl->specular.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Att :");
				ImGui::SameLine();
				ImGui::InputFloat("##lightInputAttX", &sl->attenuation.x, 0.0f, 0.0f, 2);
				ImGui::SameLine();
				ImGui::InputFloat("##lightInputAttY", &sl->attenuation.y, 0.0f, 0.0f, 2);
				ImGui::SameLine();
				ImGui::InputFloat("##lightInputAttZ", &sl->attenuation.z, 0.0f, 0.0f, 2);

				ImGui::Text("      Cut off: "); ImGui::SameLine();
				float coneAngle = glm::degrees(glm::acos(sl->cutOff));
				ImGui::DragFloat("##lightInputCutOff", &coneAngle, 0.01f, 0.0f, 20.0f, "%.2f");
				sl->cutOff = glm::cos(glm::radians(coneAngle));
				ImGui::Text("Outer Cut off: "); ImGui::SameLine();
				coneAngle = glm::degrees(glm::acos(sl->outerCutOff));
				ImGui::DragFloat("##lightInputOuterCutOff", &coneAngle, 0.01f, 0.0f, 30.0f, "%.2f");
				sl->outerCutOff = glm::cos(glm::radians(coneAngle));
			}
			else if(placedLightEntryItem > 1){
				//point lights
				renderer::PointLight* pl = static_cast<renderer::PointLight*>(app->m_lights[placedLightEntryItem]);
				ImGui::Text("Amb :");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbR", &pl->ambient.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbG", &pl->ambient.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragAmbB", &pl->ambient.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Diff:");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffR", &pl->diffuse.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffG", &pl->diffuse.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDiffB", &pl->diffuse.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Spec:");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecR", &pl->specular.r, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecG", &pl->specular.g, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragSpecB", &pl->specular.b, 0.01f, 0.0f, 1.0f, "%.2f");

				ImGui::Text("Pos :");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirX", &app->m_currentlySelectedObject->getPosition().x, 0.01f, FLT_MIN, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirY", &app->m_currentlySelectedObject->getPosition().y, 0.01f, FLT_MIN, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirZ", &app->m_currentlySelectedObject->getPosition().z, 0.01f, FLT_MIN, FLT_MAX, "%.2f");
			
				ImGui::Text("Att :");
				ImGui::SameLine();
				ImGui::InputFloat("##lightInputAttX", &pl->attenuation.x, 0.0f, 0.0f, 2);
				ImGui::SameLine();
				ImGui::InputFloat("##lightInputAttY", &pl->attenuation.y, 0.0f, 0.0f, 2);
				ImGui::SameLine();
				ImGui::InputFloat("##lightInputAttZ", &pl->attenuation.z, 0.0f, 0.0f, 2);
			}
			ImGui::PopItemWidth();
		}
	}
	ImGui::EndChild();
	ImGui::PopItemWidth();
}


bool VectorOfStringGetter(void* data, int n, const char** out_text){
	std::vector<std::string>* v = (std::vector<std::string>*)data;
	*out_text = (*v)[n].c_str();
	return true;
}

bool VectorOfObjectsGetter(void * data, int n, const char ** out_text){
	std::vector<GameObject*>* v = (std::vector<GameObject*>*)data;
	*out_text = (*v)[n]->getInEditorName().c_str();
	return true;
}

bool VectorOfShapesGetter(void * data, int n, const char ** out_text){
	std::vector<int>* v = (std::vector<int>*)data;
	*out_text = utilities::ResourceManager::IndexToShape((*v)[n]);
		
	return true;
}

bool VectorOfLightsGetter(void * data, int n, const char ** out_text){
	if(n == 0)		*out_text = "directional";
	else if(n == 1)	*out_text = "spot";
	else			*out_text = "point";

	return true;
}
