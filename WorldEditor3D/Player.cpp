#include "Player.h"
#include <Engine/Camera.h>
#include <SDL/SDL.h>
#include <Engine/InputManager.h>
#include <iostream>

const glm::vec3 Player::m_cameraOffset = glm::vec3(0.0f, 0.5f, 0.0f);


Player::Player() :
	m_flashLightOn(false),
	m_pos(glm::vec3(0.0f)),
	m_rot(glm::vec3(0.0f, -90.0f, 0.0f)),
	m_scale(glm::vec3(1.0f)),
	movementSpeed(3.5f)
{
	backup();

	m_camera = new renderer::Camera();
	updateCamera();
}

Player::Player(glm::vec3 & pos, glm::vec3 & rot, glm::vec3 & scale):
	m_flashLightOn(false),
	m_pos(pos),
	m_rot(rot),
	m_scale(scale),
	movementSpeed(3.5f)
{
	backup();
	m_camera = new renderer::Camera();
	updateCamera();
}

Player::~Player(){
	delete m_camera;
}


void Player::update(utilities::InputManager& inputManager, float deltaTime){
	//camera movement
	float cameraSpeed = movementSpeed * deltaTime;

	if(inputManager.isKeyDown(SDLK_w))
		m_pos += cameraSpeed * m_camera->getFront();
	else if(inputManager.isKeyDown(SDLK_s))
		m_pos -= cameraSpeed * m_camera->getFront();

	if(inputManager.isKeyDown(SDLK_a))
		m_pos -= glm::normalize(glm::cross(m_camera->getFront(), m_camera->getUp())) * cameraSpeed;
	else if(inputManager.isKeyDown(SDLK_d))
		m_pos += glm::normalize(glm::cross(m_camera->getFront(), m_camera->getUp())) * cameraSpeed;

	if(inputManager.isKeyDown(SDLK_q))
		m_pos -= cameraSpeed *  m_camera->getUp();
	else if(inputManager.isKeyDown(SDLK_e))
		m_pos += cameraSpeed *  m_camera->getUp();

	if(inputManager.isKeyPressed(SDLK_f))
		m_flashLightOn = !m_flashLightOn;

	//camera rotation
	//for now it is only in edit mode so Right Mouse Button needs to be pressed in order to rotate the camera
	if(inputManager.isKeyDown(SDL_BUTTON_RIGHT)){
		m_rot.x += inputManager.getMouseDY() * 0.1f;
		m_rot.y += -inputManager.getMouseDX() * 0.1f;
	}

	if(inputManager.getMouseWheel() == 1){
		movementSpeed += 0.2f;
		if(movementSpeed > 6.0f)
			movementSpeed = 6.0f;
	}
	else if(inputManager.getMouseWheel() == -1){
		movementSpeed -= 0.2f;
		if(movementSpeed < 1.0f)
			movementSpeed = 1.0f;
	}

	updateCamera();
	m_camera->updateCameraVectors();
}

void Player::backup(){
	m_buPos = m_pos;
	m_buRot = m_rot;
	m_buFL = m_flashLightOn;
}

void Player::restore(){
	m_pos = m_buPos;
	m_rot = m_buRot;
	m_flashLightOn = m_buFL;
}

void Player::updateCamera(){
	m_camera->setPos(m_pos + m_cameraOffset);
	m_camera->setPitch(m_rot.x);
	m_camera->setYaw(m_rot.y);
}

void Player::setPosition(glm::vec3& pos){
	m_pos = pos;
	updateCamera();
}

void Player::setRotation(glm::vec3& rot){
	m_rot = rot;
	updateCamera();
}

void Player::setScale(glm::vec3& scale){
	m_scale = scale;
}

void Player::incPos(float dx, float dy, float dz){ 
	m_pos.x = dx; 
	m_pos.y = dy; 
	m_pos.z = dz;
	updateCamera();
}

void Player::incRot(float dx, float dy, float dz){ 
	m_rot.x = dx; 
	m_rot.y = dy; 
	m_rot.z = dz; 
	updateCamera();
}

void Player::incScale(float dx, float dy, float dz){ 
	m_scale.x = dx; 
	m_scale.y = dy; 
	m_scale.z = dz;
}
