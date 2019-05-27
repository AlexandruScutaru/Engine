#include "TranformGizmos.h"
#include "GameObject.h"

#include <Engine/ResourceManager.h>
#include <Engine/InputManager.h>
#include <SDL/SDL.h>
#include <iostream>
#include <Engine/Camera.h>
#include <math.h>
#include "Grid.h"
#include <GLM/gtx/quaternion.hpp>

#define DEGREES_PER_SECOND 10.0f
#define SNAP_OFFSET 0.00001f


static void translateX	(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void translateY	(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void translateZ	(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void translateXY	(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void translateXZ	(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void translateYZ	(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
						 
static void scaleX		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void scaleY		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void scaleZ		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void scaleXY		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void scaleXZ		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void scaleYZ		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void scaleXYZ	(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
				
static void rotateX		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void rotateY		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);
static void rotateZ		(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt);


TranformGizmos::TranformGizmos(){}

TranformGizmos::~TranformGizmos(){
	for(auto gizmo : m_gizmosTranslate)
		delete gizmo.obj;
	m_gizmosTranslate.clear();

	for(auto gizmo : m_gizmosScale)
		delete gizmo.obj;
	m_gizmosScale.clear();

	for(auto gizmo : m_gizmosRotate)
		delete gizmo.obj;
	m_gizmosRotate.clear();

	m_gizmoFunctionality.clear();
}


void TranformGizmos::init(std::vector<Actor*>* obj){
	m_currentlyActivated = 0;
	m_pSelectedVect = obj;
	m_gizmoMode = GizmoMode::TRANSLATE;
	GameObject* object;
	glm::quat orientation = glm::quat();

	///translate
	//x translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-t1a"));
	object->setRotation(glm::quat());

	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateX;
	//xy translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::quat());

	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = translateXY;

	//y translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-t1a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	object->setRotation(orientation);

	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateY;
	//xz translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	object->setRotation(orientation);

	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateXZ;

	//z translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-t1a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	object->setRotation(orientation);

	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = translateZ;

	//yz translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	object->setRotation(orientation);

	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateYZ;

	///scale
	//x scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s1a"));
	object->setRotation(glm::quat());

	m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleX;
	//xy scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::quat());

	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleXY;

	//y scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s1a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	object->setRotation(orientation);

	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleY;
	//xz scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	object->setRotation(orientation);

	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleXZ;

	//z scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s1a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	object->setRotation(orientation);

	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleZ;
	//yz scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	object->setRotation(orientation);

	m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleYZ;

	//xyz scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s3a"));
	object->setRotation(glm::quat());

	m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleXYZ;

	///rotate
	//x roate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-r"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	object->setRotation(orientation);
	m_gizmosRotate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = rotateX;
	//y rotate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-r"));
	object->setRotation(glm::quat());
	m_gizmosRotate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = rotateY;
	//z roate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-r"));
	orientation = glm::quat();
	orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	object->setRotation(orientation);

	m_gizmosRotate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = rotateZ;
}

bool TranformGizmos::canBeShown(){
	if((*m_pSelectedVect).size() > 0 && m_gizmoMode != GizmoMode::NONE)
		return true;
	return false;
}

const std::vector<Gizmo>* TranformGizmos::getGizmos(){
	switch(m_gizmoMode){
	case GizmoMode::TRANSLATE:
		return &m_gizmosTranslate;
		break;
	case GizmoMode::SCALE:
		return &m_gizmosScale;
		break;
	case GizmoMode::ROTATE:
		return &m_gizmosRotate;
		break;
	default:
		return nullptr;
		break;
	}
}

const glm::vec3& TranformGizmos::getPosition(){
	return ((*m_pSelectedVect).back())->getPosition();
}

glm::quat TranformGizmos::getRot(){
	return ((*m_pSelectedVect).back())->getRotation();
}

void TranformGizmos::updateGizmo(renderer::Camera* camera, utilities::InputManager& input, float deltaTime){
	if(m_currentlyActivated && input.isKeyDown(SDL_BUTTON_LEFT)){
		for(auto& obj: *m_pSelectedVect)
			m_gizmoFunctionality[m_currentlyActivated](obj, input, camera, deltaTime/2.0f);
	} else if(m_currentlyActivated && m_gizmoMode == GizmoMode::TRANSLATE && !input.isKeyPressed(SDL_BUTTON_LEFT)){
		if(Grid::isSnapEnabled()){
			for(auto& obj : *m_pSelectedVect){
				obj->getPosition().x = std::round((obj)->getPosition().x / Grid::getStep()) * Grid::getStep() + SNAP_OFFSET;
				obj->getPosition().y = std::round((obj)->getPosition().y / Grid::getStep()) * Grid::getStep() + SNAP_OFFSET;
				obj->getPosition().z = std::round((obj)->getPosition().z / Grid::getStep()) * Grid::getStep() + SNAP_OFFSET;
			}
		}
		m_currentlyActivated = 0;
	}
}

bool TranformGizmos::wasClicked(int val){
	auto it = m_gizmoFunctionality.find(val);
	if(it != m_gizmoFunctionality.end())
		return true;
	return false;
}


//function definitions
void translateX(Actor* pActor, utilities::InputManager& input, renderer::Camera* camera, float dt){
	float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getRight());
	float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getUp());
		
	if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
		if(dotUX > 0.0f)
			pActor->getPosition().x -= input.getMouseDY()*dt; //up
		else
			pActor->getPosition().x += input.getMouseDY()*dt; //down
	else if(dotRX > 0.0f)
		pActor->getPosition().x += input.getMouseDX()*dt; //right
	else
		pActor->getPosition().x -= input.getMouseDX()*dt; //left
}

void translateY(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	pActor->getPosition().y -= input.getMouseDY()*dt;
}

void translateZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getRight());
	float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getUp());

	if(-0.25f <= dotRZ && dotRZ <= 0.25f) //up or down
		if(dotUZ > 0.0f)
			pActor->getPosition().z -= input.getMouseDY()*dt; //up
		else
			pActor->getPosition().z += input.getMouseDY()*dt; //down
	else if(dotRZ > 0.0f)
		pActor->getPosition().z += input.getMouseDX()*dt; //right
	else
		pActor->getPosition().z -= input.getMouseDX()*dt; //left
}

void translateXY(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getRight());
	float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getUp());

	if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
		if(dotUX > 0.0f)
			pActor->getPosition().x -= input.getMouseDY()*dt; //up
		else
			pActor->getPosition().x += input.getMouseDY()*dt; //down
	else if(dotRX > 0.0f){
		pActor->getPosition().x += input.getMouseDX()*dt; //right
		pActor->getPosition().y -= input.getMouseDY()*dt;
	} else{
		pActor->getPosition().x -= input.getMouseDX()*dt; //left
		pActor->getPosition().y -= input.getMouseDY()*dt;
	}
}

void translateXZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getRight());
	float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getUp());
															  
	float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getRight());
	float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getUp());

	if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
		if(dotUX > 0.0f){
			pActor->getPosition().x -= input.getMouseDY()*dt; //up
			if(dotRZ > 0.0f)
				pActor->getPosition().z += input.getMouseDX()*dt; //right
			else
				pActor->getPosition().z -= input.getMouseDX()*dt; //left

		} else{
			pActor->getPosition().x += input.getMouseDY()*dt; //down
			if(dotRZ > 0.0f)
				pActor->getPosition().z += input.getMouseDX()*dt; //right
			else
				pActor->getPosition().z -= input.getMouseDX()*dt; //left
		}
	else if(dotRX > 0.0f){
		pActor->getPosition().x += input.getMouseDX()*dt; //right
		if(dotUZ > 0.0f)
			pActor->getPosition().z -= input.getMouseDY()*dt; //up
		else
			pActor->getPosition().z += input.getMouseDY()*dt; //down
	} else{
		pActor->getPosition().x -= input.getMouseDX()*dt; //left
		if(dotUZ > 0.0f)
			pActor->getPosition().z -= input.getMouseDY()*dt; //up
		else
			pActor->getPosition().z += input.getMouseDY()*dt; //down
	}
}

void translateYZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getRight());
	float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getUp());

	if(-0.25f <= dotRZ && dotRZ <= 0.25f) //up or down
		if(dotUZ > 0.0f)
			pActor->getPosition().z -= input.getMouseDY()*dt; //up
		else
			pActor->getPosition().z += input.getMouseDY()*dt; //down
	else if(dotRZ > 0.0f){
		pActor->getPosition().z += input.getMouseDX()*dt; //right
		pActor->getPosition().y -= input.getMouseDY()*dt;
	} else{
		pActor->getPosition().z -= input.getMouseDX()*dt; //left
		pActor->getPosition().y -= input.getMouseDY()*dt;
	}
}

void scaleX(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getRight());
	float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getUp());

	if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
		if(dotUX > 0.0f)
			pActor->getScale().x -= input.getMouseDY()*dt; //up
		else
			pActor->getScale().x += input.getMouseDY()*dt; //down
	else if(dotRX > 0.0f)
		pActor->getScale().x += input.getMouseDX()*dt; //right
	else
		pActor->getScale().x -= input.getMouseDX()*dt; //left
}

void scaleY(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	pActor->getScale().y -= input.getMouseDY()*dt;
}

void scaleZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getRight());
	float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getUp());

	if(-0.25f <= dotRZ && dotRZ <= 0.25f) //up or down
		if(dotUZ > 0.0f)
			pActor->getScale().z -= input.getMouseDY()*dt; //up
		else
			pActor->getScale().z += input.getMouseDY()*dt; //down
	else if(dotRZ > 0.0f)
		pActor->getScale().z += input.getMouseDX()*dt; //right
	else
		pActor->getScale().z -= input.getMouseDX()*dt; //left
}

void scaleXY(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float d = (input.getMouseDX() - input.getMouseDY()) / 2.0f;
	pActor->getScale().x += d *dt;
	pActor->getScale().y += d *dt;
}

void scaleXZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float d = (input.getMouseDX() - input.getMouseDY()) / 2.0f;
	pActor->getScale().x += d *dt;
	pActor->getScale().z += d *dt;
}

void scaleYZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float d = (input.getMouseDX() - input.getMouseDY()) / 2.0f;
	pActor->getScale().y += d *dt;
	pActor->getScale().z += d *dt;
}

void scaleXYZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	pActor->setScale(pActor->getScale() + glm::vec3(input.getMouseDX()*dt));
}

void rotateX(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	pActor->setRotation(glm::rotate(pActor->getRotation(), glm::radians(input.getMouseDX() * DEGREES_PER_SECOND * dt), glm::vec3(1.0f, 0.0f, 0.0f)));
}

void rotateY(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	pActor->setRotation(glm::rotate(pActor->getRotation(), glm::radians(input.getMouseDX() * DEGREES_PER_SECOND * dt), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void rotateZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	pActor->setRotation(glm::rotate(pActor->getRotation(), glm::radians(input.getMouseDX() * DEGREES_PER_SECOND * dt), glm::vec3(0.0f, 0.0f, 1.0f)));
}
