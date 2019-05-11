#include "Player.h"
#include <Engine/Camera.h>
#include <SDL/SDL.h>
#include <Engine/InputManager.h>
#include <iostream>

#define LOOK_SENSITIVITY 0.6f

const glm::vec3 Player::m_cameraOffset = glm::vec3(0.0f, 1.1f, 0.0f);


Player::Player() :
	m_flashLightOn(false),
	m_pos(glm::vec3(0.0f)),
	m_rot(glm::vec3(0.0f)),
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
	
	m_camera->getForward();
	m_camera->getRight();
	
	glm::vec3 dir(0.0f);
	float force = 1000.0f;
	
	if(inputManager.isKeyDown(SDLK_LSHIFT))
		force = 2000.0f;
	
	if(inputManager.isKeyDown(SDLK_w)){
		dir = -m_camera->getForward();
	} else if(inputManager.isKeyDown(SDLK_s)){
		dir = +m_camera->getForward();
	}
	
	if(inputManager.isKeyDown(SDLK_a)){
		dir += (-m_camera->getRight());
	} else if(inputManager.isKeyDown(SDLK_d)){
		dir += m_camera->getRight();
	} 
	
	if(dir.x != 0.0f || dir.y != 0.0f || dir.z != 0.0f) {
		dir.y = 0.0f;
		dir = glm::normalize(dir);
		dir *= force;
		m_rigidBody->setFrictionCoefficient(0.0f);
		m_rigidBody->setRollingResistance(0.0f);
		m_rigidBody->applyForceToCenterOfMass(dir);
	} else{
		m_rigidBody->setFrictionCoefficient(1.0f);
		m_rigidBody->setRollingResistance(1.0f);
	}

	auto crtVel = m_rigidBody->getLinearVelocity();
	m_rigidBody->setLinearVelocity(glm::vec3(crtVel.x * 0.8f, crtVel.y, crtVel.z * 0.8f));
			
	bool canjump = false;
	auto body = (*m_rigidBody).m_body;
	const rp3d::ContactManifoldListElement* listElem;
	listElem = body->getContactManifoldsList();
	//std::cout << "Player pos: " << m_pos.x << " " << m_pos.y << " " << m_pos.z << std::endl;
	for(; listElem != nullptr; listElem = listElem->next) {
		rp3d::ContactManifold* manifold = listElem->contactManifold;
	
		// For each contact point of the manifold 
		for(int i = 0; i<manifold->getNbContactPoints(); i++) {
	
			// Get the contact point 
			rp3d::ContactPoint* point = manifold->getContactPoint(i);
	
			// Get the world-space contact point on body 1 
			rp3d::Vector3 pos = point->getWorldPointOnBody1();
	
			// Get the world-space contact normal 
			rp3d::Vector3 normal = point->getNormal();
	
			//std::cout << "Contact #" << i << ": pos=" << pos.x << " " << pos.y << " " << pos.z << "\tnormal: " << normal.x << " " << normal.y << " " << normal.z << std::endl;
			if(pos.y <= m_pos.y)
				canjump = true;
		}
	}
	
	if(canjump && inputManager.isKeyPressed(SDLK_SPACE)){
		m_rigidBody->applyForceToCenterOfMass(glm::vec3(0.0f, 20000.0f, 0.0f));
	}
	
	if(inputManager.isKeyPressed(SDLK_f))
		m_flashLightOn = !m_flashLightOn;
	
	//camera rotation
	m_rot.x -= (inputManager.getRelMouseCoords().y * LOOK_SENSITIVITY);
	m_rot.y += (inputManager.getRelMouseCoords().x * LOOK_SENSITIVITY);

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

	//updateCamera();
	m_camera->setPos(m_pos + m_cameraOffset);
	m_camera->setPitch(m_rot.x);
	m_camera->setYaw(m_rot.y);
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
