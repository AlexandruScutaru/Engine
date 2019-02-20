#include "GUI.h"
#include "MainApp.h"
#include <climits>
#include <dirent/dirent.h>
#include <cfloat>
#include "Utilities.h"
#include <Engine/CollisionBody.h>
#include <ImGUI/imgui_internal.h>


bool VectorOfStringGetter(void* data, int n, const char** out_text);
bool VectorOfObjectsGetter(void* data, int n, const char** out_text);
bool VectorOfShapesGetter(void* data, int n, const char** out_text);
bool VectorOfColVolumesGetter(void* data, int n, const char** out_text);
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
	b_showGridWindow(false),
	b_showEditRotWindow(false),
	b_showPopup(false),
	m_rotationEditJustShown(false),
	placedGameobjectEntryItem(-1),
	placedLightEntryItem(-1),
	placedCollisionVolumeEntryItem(-1),
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
	
	m_sceneTabs = 0ui8;
}

GUI::~GUI(){}


void GUI::updateImGuiWindows(){
	ImGui_ImplSdlGL3_NewFrame(app->m_window.getWindow());

	showMainMenu();
	showToolbar();
	showEditorWindow();
	if(b_showOpenFileDialog) showOpenFileDialog();
	if(b_showSaveFileDialog) showSaveFileDialog();
	if(b_showGridWindow)     showGridSettingsWindow();
	if(b_showEditRotWindow)  showRotationEditWindow();

	if(b_showPopup)			 showWarningPopup();
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
			if(ImGui::MenuItem("Grid")){
				b_showGridWindow = true;
			}
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
	//td = *utilities::ResourceManager::getTexture("res/editor/add-pointLight.png");
	//if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
	//	app->addPointLight();
	//}

	//add gameobject
	//ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/editor/add-gameobject.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		if(m_sceneTabs & 1ui8 << Scene_Tabs::LIGHTS){
			app->addPointLight();
		} else if(m_sceneTabs & 1ui8 << Scene_Tabs::GAMEOBJECTS){
			b_showOpenFileDialog = true;
			currentPath = "res/gameobjects/";
			fdMode = FD_Mode::ADD_GAMEOBJECT;
			updateDirContents(dirContents);
		} else if(m_sceneTabs & 1ui8 << Scene_Tabs::COL_VOLUMES){
			app->addNewColVolume();
		}
	}
	//duplicate selected
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/editor/duplicate.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		if(m_sceneTabs & 1ui8 << Scene_Tabs::GAMEOBJECTS)
			app->duplicateSelectedGameObjects();
		else if(m_sceneTabs & 1ui8 << Scene_Tabs::LIGHTS)
			app->duplicateSelectedPointLights();
		else if(m_sceneTabs & 1ui8 << Scene_Tabs::COL_VOLUMES)
			app->duplicateSelectedColVolumes();
	}
	//remove selected
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/editor/remove.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		if(m_sceneTabs & 1ui8 << Scene_Tabs::GAMEOBJECTS)
			app->removeSelectedGameObjects();
		else if(m_sceneTabs & 1ui8 << Scene_Tabs::LIGHTS)
			app->removeSelectedGameObjects();
		else if(m_sceneTabs & 1ui8 << Scene_Tabs::COL_VOLUMES)
			app->removeSelectedColVolumes();
	}

	ImGui::PopStyleColor();
	///separator
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/editor/sep.png");
	ImGui::Image((ImTextureID)td.id, ImVec2(3, 27), ImVec2(0, 0), ImVec2(1, -1));

	///transform gizmo mode selectors
	//none - select only
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/editor/select.png");
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
	td = *utilities::ResourceManager::getTexture("res/editor/move.png");
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
	td = *utilities::ResourceManager::getTexture("res/editor/rotate.png");
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
	td = *utilities::ResourceManager::getTexture("res/editor/scale.png");
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
	td = *utilities::ResourceManager::getTexture("res/editor/sep.png");
	ImGui::Image((ImTextureID)td.id, ImVec2(3, 27), ImVec2(0, 0), ImVec2(1, -1));

	///center camera
	ImGui::SameLine();
	td = *utilities::ResourceManager::getTexture("res/editor/center.png");
	if(ImGui::ImageButton((ImTextureID)td.id, ImVec2(22, 22), ImVec2(0, 0), ImVec2(1, -1), 1) && b_placementTab){
		glm::vec3 pos(0.0f);
		if(placedLightEntryItem > 1 || placedLightEntryItem == 0){
			//light selected
			pos = app->m_selectedObjsVect[0]->getPosition();
		} else if(app->m_selectedObjsVect.size() > 0){
			//object selected
			pos = app->m_selectedObjsVect.back()->getPosition();	
		}
		
		pos += glm::vec3(0.0f, 0.64f, 2 * 0.64f);
		app->m_player.setPosition(pos);
		app->m_player.setRotation(glm::vec3(-35.0f, -90.0f, 0.0f));
	}

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
		ImGui::Text("Visual info");
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

		ImGui::Checkbox("Billboard", &(app->m_creationTabGameObject.isBillboardRef()));
		
		ImGui::Separator();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);
		ImGui::Text("\nPhysics info");
		ImGui::Checkbox  ("Affected by gravity", &app->m_creationTabGameObject.m_gravityEnabled);
		ImGui::Checkbox  ("Allowed to sleep", &app->m_creationTabGameObject.m_allowedToSleep);
		ImGui::InputFloat("Bounciness", &app->m_creationTabGameObject.m_bounciness, 0.0f, 0.0f, 3);
		ImGui::InputFloat("Friction Coefficient", &app->m_creationTabGameObject.m_frictionCoefficient, 0.0f, 0.0f, 3);
		ImGui::InputFloat("Rolliing Resistance", &app->m_creationTabGameObject.m_rollingResistance, 0.0f, 0.0f, 3);
		ImGui::InputFloat("Linear Damping", &app->m_creationTabGameObject.m_linearDamping, 0.0f, 0.0f, 3);
		ImGui::InputFloat("Angular Damping", &app->m_creationTabGameObject.m_angularDamping, 0.0f, 0.0f, 3);

		ImGui::Separator();
		ImGui::PopItemWidth();
		ImGui::Text("\nObject collision");
		
		ImGui::ListBox("##collisionBodiesListBox", &collisionBodyEntryItem, VectorOfShapesGetter, (void*)(&collisionBodies), (int)(collisionBodies.size()), 5);
		
		static int collisionBodyIndex = renderer::CollisionShapes::SHAPE_CUBE;
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
			ImGui::BeginChild("colls", ImVec2(-1, 110), false);
			{
				ImGui::Text("Transforms");
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
				ImGui::DragFloat("##dragRotX", &body.colRotEuler.x, 1.0f, -359.0f, 359.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##dragRotY", &body.colRotEuler.y, 1.0f, -359.0f, 359.0f, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##dragRotZ", &body.colRotEuler.z, 1.0f, -359.0f, 359.0f, "%.2f");
				body.updateQuaternion();

				ImGui::Text("S:");
				ImGui::SameLine();
				if(body.shape == renderer::SHAPE_SPHERE){
					ImGui::DragFloat("##dragScaleX", &body.colScale.x, 0.01f, 0.01f, 100.0f);
					ImGui::SameLine();
					ImGui::DragFloat("##dragScaleX", &body.colScale.y, 0.01f, 0.01f, 100.0f);
					ImGui::SameLine();
					ImGui::DragFloat("##dragScaleX", &body.colScale.z, 0.01f, 0.01f, 100.0f);
				} else if(body.shape == renderer::SHAPE_CAPSULE){
					ImGui::DragFloat("##dragScaleX", &body.colScale.x, 0.01f, 0.01f, 100.0f);
					ImGui::SameLine();
					ImGui::DragFloat("##dragScaleY", &body.colScale.y, 0.01f, 0.01f, 100.0f);
					ImGui::SameLine();
					ImGui::DragFloat("##dragScaleX", &body.colScale.z, 0.01f, 0.01f, 100.0f);
				} else{
					ImGui::DragFloat("##dragScaleX", &body.colScale.x, 0.01f, 0.01f, 100.0f);
					ImGui::SameLine();
					ImGui::DragFloat("##dragScaleY", &body.colScale.y, 0.01f, 0.01f, 100.0f);
					ImGui::SameLine();
					ImGui::DragFloat("##dragScaleZ", &body.colScale.z, 0.01f, 0.01f, 100.0f);
				}
				ImGui::Text("Mass:"); ImGui::SameLine();
				ImGui::InputFloat("##mass", &body.mass, 0.0f, 0.0f, 3);
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
		if(ImGui::Button("GameObjects", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))){
			m_sceneTabs = (m_sceneTabs & 1ui8 << Scene_Tabs::GAMEOBJECTS) ^ 1ui8 << Scene_Tabs::GAMEOBJECTS;
			app->deselectAll();
		}
		if(m_sceneTabs & 1ui8 << Scene_Tabs::GAMEOBJECTS)
			showGameobjectsTab();
		
		if(ImGui::Button("Collision Volumes", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))){
			m_sceneTabs = (m_sceneTabs & 1ui8 << Scene_Tabs::COL_VOLUMES) ^ 1ui8 << Scene_Tabs::COL_VOLUMES;
			app->deselectAll();
		}
		if(m_sceneTabs & 1ui8 << Scene_Tabs::COL_VOLUMES)
			showColVolumesTab();

		if(ImGui::Button("Lights", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))){
			m_sceneTabs = (m_sceneTabs & 1ui8 << Scene_Tabs::LIGHTS) ^ 1ui8 << Scene_Tabs::LIGHTS;
			app->deselectAll();
		}
		if(m_sceneTabs & 1ui8 << Scene_Tabs::LIGHTS)
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
	static char buf[OBJECT_NAME_SIZE] = "";
	ImGui::InputText("##filename", buf, OBJECT_NAME_SIZE);
	ImGui::Text("\t\t\t");
	ImGui::SameLine();
	if(ImGui::Button("Save as")){
		bool exists = false;
		for(auto dir : dirContents){
			if(dir.compare(std::string(buf)) == 0){
				break;
			}
		}
		if(!exists){
			b_showSaveFileDialog = false;
			if(fdMode == FD_Mode::OBJECT_SAVE){
				Utilities::saveCreatedObject(app, buf);
			} else if(fdMode == FD_Mode::MAP_SAVE){
				Utilities::saveMap(app, buf);
			}
		}
		memset(buf, '\0', OBJECT_NAME_SIZE);
	}
	ImGui::SameLine();
	if(ImGui::Button("Cancel")){
		b_showSaveFileDialog = false;
	}
	ImGui::PopItemWidth();
	ImGui::End();
}

void GUI::showWarningPopup(){
	ImGui::OpenPopup(popupTitle.c_str());
	if(ImGui::BeginPopupModal(popupTitle.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		ImGui::Text("");
		ImGui::Text(popupInfo.c_str());
		ImGui::Text("");
		ImGui::Separator();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - 40);
		if(ImGui::Button("OK", ImVec2(80, 0))) { 
			ImGui::CloseCurrentPopup(); 
			b_showPopup = false;
		}
		ImGui::EndPopup();
	}
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
		Utilities::openMap(app, dirContents[fdEntryItem]);
		break;
	case FD_Mode::OBJECT_OPEN:
		Utilities::openCreatedObject(app, dirContents[fdEntryItem]);
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
	//gameobjects in scene
	if(app->m_selectedObjsVect.size() == 1){
		for(size_t i = 0; i < app->m_objectsInScene.size(); i++)
			if(app->m_objectsInScene[i] == app->m_selectedObjsVect[0]){
				placedGameobjectEntryItem = i;
				break;
			}
	} else{
		placedGameobjectEntryItem = -1;
	}

	ImGui::PushItemWidth(-1);
	ImGui::Text("Gameobjects in scene:");
	ImGui::ListBox("##gameobjectsList", &placedGameobjectEntryItem, VectorOfObjectsGetter, (void*)(&app->m_objectsInScene), (int)(app->m_objectsInScene.size()), 10);
	//placement settings
	if(placedGameobjectEntryItem >= 0){
		for(auto& obj : app->m_selectedObjsVect)
			if(obj) obj->setSelected(false);
		app->m_selectedObjsVect.clear();
		
		app->m_selectedObjsVect.push_back(app->m_objectsInScene[placedGameobjectEntryItem]);
		app->m_selectedObjsVect[0]->setSelected(true);
		//object info
		memset(m_name, '\0', OBJECT_NAME_SIZE);
		strncat_s(m_name, app->m_objectsInScene[placedGameobjectEntryItem]->getInEditorName().c_str(), OBJECT_NAME_SIZE);
		ImGui::InputText("##gameobjectName", m_name, OBJECT_NAME_SIZE);
		app->m_objectsInScene[placedGameobjectEntryItem]->setInEditorName(m_name);
		
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
		int bodyTypeIndex = (int)app->m_objectsInScene[placedGameobjectEntryItem]->getBodyType();
		ImGui::Combo("Body type", &bodyTypeIndex, "Static\0Kinematic\0Dynamic\0\0");
		app->m_objectsInScene[placedGameobjectEntryItem]->setBodyType(static_cast<physics::BodyType>(bodyTypeIndex));
		ImGui::PopItemWidth();

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

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			auto rot = glm::eulerAngles(obj->getRotation());
			rot = glm::vec3(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
			ImGui::InputFloat("##objInputRotX", &rot.x, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputRotY", &rot.y, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##objInputRotZ", &rot.z, 0.0f, 0.0f, 2);
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();

			ImGui::SameLine();
			if(ImGui::Button("Edit")){
				m_rot = glm::eulerAngles(obj->getRotation());
				b_showEditRotWindow = true;
				m_rotationEditJustShown = true;
				m_rotateGameobject = true;
			}
			
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

void GUI::showColVolumesTab(){
	if(app->m_selectedObjsVect.size() == 1){
		for(size_t i = 0; i < app->m_colVolumeBillboards.size(); i++)
			if(app->m_colVolumeBillboards[i] == app->m_selectedObjsVect[0]){
				placedCollisionVolumeEntryItem = i;
				break;
			}
	} else{
		placedCollisionVolumeEntryItem = -1;
	}

	ImGui::PushItemWidth(-1);
	ImGui::Text("Collision volumes in scene:");
	ImGui::ListBox("##colVolumesList", &placedCollisionVolumeEntryItem, VectorOfColVolumesGetter, (void*)(&app->m_colVolumeBillboards), (int)(app->m_colVolumeBillboards.size()), 10);
	
	if(placedCollisionVolumeEntryItem >= 0){
		for(auto& colVol : app->m_colVolumeBillboards)
			if(colVol) colVol->setSelected(false);
		app->m_selectedObjsVect.clear();

		app->m_selectedObjsVect.push_back(app->m_colVolumeBillboards[placedCollisionVolumeEntryItem]);
		app->m_selectedObjsVect[0]->setSelected(true);
		
		auto currentCollisionVolume = app->m_colVolumeBillboards[placedCollisionVolumeEntryItem];
		//collision volume info
		memset(m_name, '\0', OBJECT_NAME_SIZE);
		strncat_s(m_name, currentCollisionVolume->getName().c_str(), OBJECT_NAME_SIZE);
		ImGui::InputText("##colVolumeName", m_name, OBJECT_NAME_SIZE);
		currentCollisionVolume->setName(m_name);
		ImGui::Separator();

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
		ImGui::Text("Volume type: ");
		ImGui::SameLine();
		int volumeType = currentCollisionVolume->getVolumeType();
		ImGui::Combo("##colVolumeType", &volumeType, "Level start\0Level end\0Trigger\0\0");
		currentCollisionVolume->setVolumeType(static_cast<VolumeType>(volumeType));
		if(volumeType == static_cast<int>(VolumeType::START)) {
			auto& colBody = currentCollisionVolume->getColBodyRef();
			colBody.shape = renderer::CollisionShapes::SHAPE_CAPSULE;
			colBody.colScale = glm::vec3(1.0f, 1.65f, 1.0f);
		}

		ImGui::Text("Volume shape:");
		ImGui::SameLine();
		ImGui::Combo("##colVolumeShape", &currentCollisionVolume->getColBodyPtr()->shape, "Cube\0Sphere\0Cilinder\0Cone\0Capsule\0\0");
		ImGui::PopItemWidth();
		ImGui::Separator();

		ImGui::Text("Transforms");
		ImGui::BeginChild("colVOlumeTransforms", ImVec2(-1, 65), false);
		{
			ImGui::PushItemWidth(70);
			ImGui::Text("P:");
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputPosX", &currentCollisionVolume->getPosition().x, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputPosY", &currentCollisionVolume->getPosition().y, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputPosZ", &currentCollisionVolume->getPosition().z, 0.0f, 0.0f, 2);
		
			ImGui::Text("R:");
			ImGui::SameLine();
			
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			auto rot = glm::eulerAngles(currentCollisionVolume->getColBodyRef().colRotQuat);
			rot = glm::vec3(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z));
			ImGui::InputFloat("##colVolInputRotX", &rot.x, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputRotY", &rot.y, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputRotZ", &rot.z, 0.0f, 0.0f, 2);
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
			
			ImGui::SameLine();
			if(ImGui::Button("Edit")){
				m_rot = glm::eulerAngles(currentCollisionVolume->getColBodyRef().colRotQuat);
				b_showEditRotWindow = true;
				m_rotationEditJustShown = true;
				m_rotateGameobject = false;
			}
			
			ImGui::Text("S:");
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputScaleX", &currentCollisionVolume->getColBodyPtr()->colScale.x, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputScaleY", &currentCollisionVolume->getColBodyPtr()->colScale.y, 0.0f, 0.0f, 2);
			ImGui::SameLine();
			ImGui::InputFloat("##colVolInputScaleZ", &currentCollisionVolume->getColBodyPtr()->colScale.z, 0.0f, 0.0f, 2);
			
			ImGui::PopItemWidth();
		}
		ImGui::EndChild();
	}
	ImGui::PopItemWidth();
}

void GUI::showLightsTab(){
	//lights in scene list
	if(app->m_selectedObjsVect.size() == 1){
		for(size_t i = 0; i < app->m_lights.size(); i++)
			if(app->m_lights[i] == static_cast<LightBillboard*>(app->m_selectedObjsVect[0])->getLight()){
				placedLightEntryItem = i;
				break;
			}
	} else if(placedLightEntryItem == 1){
		//do nothing
	} else{
		placedLightEntryItem = -1;
	}

	ImGui::PushItemWidth(-1);
	ImGui::Text("Lights in scene:");
	ImGui::ListBox("##lightsList", &placedLightEntryItem, VectorOfLightsGetter, (void*)(&app->m_lights), (int)(app->m_lights.size()), 10);
	
	//lights settings
	ImGui::Separator();
	ImGui::BeginChild("gameobjectTransforms", ImVec2(-1, -1), false);
	{
		if(placedLightEntryItem >= 0){
			ImGui::Text("Properties");
			ImGui::PushItemWidth(65);
			app->deselectAll();

			for(auto entry : app->m_lightsBillboards){
				if(entry->getLight() == app->m_lights[placedLightEntryItem]){
					app->m_selectedObjsVect.push_back(entry);
					app->m_selectedObjsVect[0]->setSelected(true);
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
				ImGui::DragFloat("##lightDragDirX", &app->m_selectedObjsVect[0]->getPosition().x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirY", &app->m_selectedObjsVect[0]->getPosition().y, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirZ", &app->m_selectedObjsVect[0]->getPosition().z, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
			}
			else if(placedLightEntryItem == 1){
				app->deselectAll();

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
				ImGui::DragFloat("##lightDragDirX", &app->m_selectedObjsVect[0]->getPosition().x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirY", &app->m_selectedObjsVect[0]->getPosition().y, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::SameLine();
				ImGui::DragFloat("##lightDragDirZ", &app->m_selectedObjsVect[0]->getPosition().z, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
			
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

void GUI::showGridSettingsWindow(){
	ImGui::SetNextWindowSize(ImVec2(200, 6.5f * ImGui::GetFrameHeightWithSpacing()), ImGuiSetCond_Always);
	ImGui::Begin("Grid Settings", &b_showGridWindow,
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoCollapse
	);

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);
	///Show grid
	bool boolVal = Grid::isEnabled();
	ImGui::Checkbox("Show Grid", &boolVal);
	Grid::setEnabled(boolVal);
	ImGui::Separator();
	///Grid snapping
	boolVal = Grid::isSnapEnabled();
	ImGui::Checkbox("Snap to Grid", &boolVal);
	Grid::setSnapEnabled(boolVal);
	///Grid Step selection
	static int it = 0;
	ImGui::Combo("Grid Step", &it, " 0.16 \0 0.32 \0 0.64 \0 1.28 \0 2.56 \0\0", 5);
	switch(it){
	case 0:
		Grid::setStep(0.16f);
		break;
	case 1:
		Grid::setStep(0.32f);
		break;
	case 2:
		Grid::setStep(0.64f);
		break;
	case 3:
		Grid::setStep(1.28f);
		break;
	case 4:
		Grid::setStep(2.56f);
		break;
	default:
		break;
	}
	ImGui::Separator();
	///Dec/Inc Grid height
	ImGui::Text("%.3f Grid Height", Grid::getHeight());
	if(ImGui::Button("Decrease")) Grid::decHeight();
	ImGui::SameLine();
	if(ImGui::Button("Increase")) Grid::incHeight();

	ImGui::PopItemWidth();
	ImGui::End();
}

void GUI::showRotationEditWindow(){
	ImGui::SetNextWindowSize(ImVec2(300, 2.5f * ImGui::GetFrameHeightWithSpacing()), ImGuiSetCond_Always);
	ImGui::Begin("Edit Rotation", &b_showEditRotWindow,
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoCollapse
	);
	Actor* obj;
	if(m_rotateGameobject)
		obj = app->m_objectsInScene[placedGameobjectEntryItem];
	else
		obj = app->m_colVolumeBillboards[placedCollisionVolumeEntryItem];

	static glm::vec3 rotDg = glm::vec3(glm::degrees(m_rot.x), glm::degrees(m_rot.y), glm::degrees(m_rot.z));
	if(m_rotationEditJustShown){
		rotDg = glm::vec3(glm::degrees(m_rot.x), glm::degrees(m_rot.y), glm::degrees(m_rot.z));
		m_rotationEditJustShown = false;
	}
	ImGui::PushItemWidth(70);

	ImGui::InputFloat("##objInputRotX", &rotDg.x, 0.0f, 0.0f, 2);
	ImGui::SameLine();
	ImGui::InputFloat("##objInputRotY", &rotDg.y, 0.0f, 0.0f, 2);
	ImGui::SameLine();
	ImGui::InputFloat("##objInputRotZ", &rotDg.z, 0.0f, 0.0f, 2);
	ImGui::SameLine();
	if(ImGui::Button("set")){
		m_rot = glm::vec3(glm::radians(rotDg.x), glm::radians(rotDg.y), glm::radians(rotDg.z));
		obj->setRotation(glm::quat(m_rot));
		b_showEditRotWindow = false;
	}

	if(!ImGui::IsWindowFocused())
		b_showEditRotWindow = false;
	ImGui::PopItemWidth();
	ImGui::End();	
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

bool VectorOfColVolumesGetter(void * data, int n, const char ** out_text){
	std::vector<CollisionVolumeBillboard*>* v = (std::vector<CollisionVolumeBillboard*>*)data;
	*out_text = (*v)[n]->getName().c_str();
	return true;
}

bool VectorOfLightsGetter(void * data, int n, const char ** out_text){
	if(n == 0)		*out_text = "directional";
	else if(n == 1)	*out_text = "spot";
	else			*out_text = "point";
	return true;
}
