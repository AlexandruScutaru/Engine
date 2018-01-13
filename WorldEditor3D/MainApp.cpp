#include "MainApp.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define MAX_FPS 120


MainApp::MainApp():
	m_appState(AppState::EDIT),
	m_collapseEditor(false)
{
	initSystems();
}

MainApp::~MainApp(){
	ImGui_ImplSdlGL3_Shutdown();

	glDeleteVertexArrays(1, &m_crate.VAO);
	glDeleteVertexArrays(1, &m_lightSource.VAO);
	glDeleteBuffers(1, &m_crate.VBO);
	glDeleteTextures(1, &m_crate_DIFF.id);
	glDeleteTextures(1, &m_crate_SPEC.id);
}


void MainApp::run(){
	initLevel();
	loop();
}

void MainApp::initSystems(){
	m_window.create("Renderer", SCREEN_WIDTH, SCREEN_HEIGHT/*, renderer::WindowFlags::BORDERLESS*/);
	
	//Setup ImGui
	ImGui_ImplSdlGL3_Init(m_window.getWindow());
	ImGui::StyleColorsDark();

	// build and compile the shader programs
	m_lightingShader.init("shaders/colors.vs", "shaders/colors.fs");
	m_lightSourceShader.init("shaders/lightSource.vs", "shaders/lightSource.fs");

	m_fpsLimiter.setMaxFPS(MAX_FPS);
}

void MainApp::initLevel(){
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	// first, configure the cube's VAO (and VBO)
	//unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &m_crate.VAO);
	glGenBuffers(1, &m_crate.VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_crate.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_crate.VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	//unsigned int lightVAO;
	glGenVertexArrays(1, &m_lightSource.VAO);
	glBindVertexArray(m_lightSource.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_crate.VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	m_crate_DIFF = renderer::TextureLoader::loadTexture("res/textures/crate_DIFF.png");
	m_crate_SPEC = renderer::TextureLoader::loadTexture("res/textures/crate_SPEC.png");

	m_cratePos = glm::vec3(0.0f);
	m_dirLight = renderer::DirLight(glm::vec3(2.0f, 1.0f, 0.0f), 
									glm::vec3(0.05f, 0.05f, 0.05f), 
									glm::vec3(0.4f, 0.4f, 0.4f), 
									glm::vec3(0.5f, 0.5f, 0.5f));
	

	// shader configuration
	// --------------------
	m_lightingShader.use();
	m_lightingShader.loadInt("material.diffuse", 0);
	m_lightingShader.loadInt("material.specular", 1);
	m_lightingShader.loadFloat("material.shininess", 32.0f);

	//lights
	m_lightingShader.loadInt("pointLightsNum", 0);
	// directional light
	//m_lightingShader.loadVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	m_lightingShader.loadVec3("dirLight.ambient", m_dirLight.ambient);
	m_lightingShader.loadVec3("dirLight.diffuse", m_dirLight.diffuse);
	m_lightingShader.loadVec3("dirLight.specular", m_dirLight.specular);
	
	//m_lightingShader.loadBool("flashlightOn", false);
}

void MainApp::loop(){
	while(m_appState == AppState::EDIT){
		float deltaTime = m_fpsLimiter.begin();
		
		processInput();
		updateImGuiWindows();
		update(deltaTime);
		drawGame();

		m_fpsLimiter.end();
	}
}

void MainApp::processInput(){
	m_inputManager.update();

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		ImGui_ImplSdlGL3_ProcessEvent(&e);
		switch(e.type) {
		case SDL_QUIT:
			// Exit the app here!
			m_appState = AppState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			m_inputManager.setMouseCoords((float)e.motion.xrel, (float)e.motion.yrel);
			break;
		case SDL_MOUSEWHEEL:
			m_inputManager.setMouseWheel(e.wheel.y);
			break;
		case SDL_KEYDOWN:
			m_inputManager.pressKey(e.key.keysym.sym);
			break;
		case SDL_KEYUP:
			m_inputManager.releaseKey(e.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_inputManager.pressKey(e.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			m_inputManager.releaseKey(e.button.button);
			break;
		}
	}
}

void MainApp::update(float deltaTime){
	if(!ImGui::GetIO().WantCaptureKeyboard)
		m_camera.update(m_inputManager, deltaTime);

	///object rotation
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, 1.0f*deltaTime, glm::vec3(0.0, 1.0, 0.0));
	m_dirLight.direction = glm::vec3(rotationMat * glm::vec4(m_dirLight.direction, 1.0));
}

void MainApp::updateImGuiWindows(){
	ImGui_ImplSdlGL3_NewFrame(m_window.getWindow());

	showEditorWindow();
}

void MainApp::drawGame(){
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// be sure to activate shader when setting uniforms/drawing objects
	m_lightingShader.use();
	m_lightingShader.loadVec3("viewPos", m_camera.getPos());

	//light data
	//m_lightingShader.loadVec3("dirLight.direction", m_dirLightPos);
	m_lightingShader.loadVec3("dirLight.direction", -m_dirLight.direction);


	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.getZoom()), (float)renderer::Window::getW() / (float)renderer::Window::getH(), 0.1f, 100.0f);
	glm::mat4 view = m_camera.getViewMatrix();
	m_lightingShader.loadMat4("projection", projection);
	m_lightingShader.loadMat4("view", view);
	// world transformation
	glm::mat4 model;
	m_lightingShader.loadMat4("model", model);
	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_crate_DIFF.id);
	// bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_crate_SPEC.id);

	// render the crate
	glBindVertexArray(m_crate.VAO);
	model = glm::translate(model, m_cratePos);
	m_lightingShader.loadMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	// also draw the lamp object
	m_lightSourceShader.use();
	m_lightSourceShader.loadMat4("projection", projection);
	m_lightSourceShader.loadMat4("view", view);

	glBindVertexArray(m_lightSource.VAO);
	model = glm::mat4();
	model = glm::translate(model, m_dirLight.direction);
	model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
	m_lightSourceShader.loadMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	ImGui::Render();
	//sdl: swap buffers
	m_window.swapBuffer();
}

/// ImGui
void MainApp::showEditorWindow(){
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH-300.0f, 0.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, SCREEN_HEIGHT), ImGuiCond_Once);

	ImGui::Begin("editor", NULL);
	
	ImGui::Text("test");
	static char a[32];
	if(ImGui::InputText("testInput", a, 32)){

	}
	
	ImGui::End();
}
