#include "TranformGizmos.h"
#include "GameObject.h"

#include <Engine/ResourceManager.h>
#include <Engine/InputManager.h>
#include <SDL/SDL.h>
#include <iostream>
#include <Engine/Camera.h>
#include <math.h>

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


TranformGizmos::TranformGizmos() :
	gridStep(0.01f)
{}

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


void TranformGizmos::init(Actor** obj){
	m_currentlyActivated = 0;
	m_pSelectedActor = obj;
	m_gizmoMode = GizmoMode::TRANSLATE;
	GameObject* object;

	///translate
	//x translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-t1a"));
	object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateX;
	//xy translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = translateXY;

	//y translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-t1a"));
	object->setRotation(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)));
	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateY;
	//xz translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));
	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateXZ;

	//z translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-t1a"));
	object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = translateZ;

	//yz translate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
	m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = translateYZ;

	///scale
	//x scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s1a"));
	object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleX;
	//xy scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleXY;

	//y scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s1a"));
	object->setRotation(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)));
	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleY;
	//xz scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));
	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleXZ;

	//z scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s1a"));
	object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
	m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleZ;
	//yz scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-2a"));
	object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
	m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleYZ;

	//xyz scale gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-s3a"));
	object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = scaleXYZ;

	///rotate
	//x roate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-r"));
	object->setRotation(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)));
	m_gizmosRotate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = rotateX;
	//y rotate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-r"));
	object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_gizmosRotate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_gizmoFunctionality[object->getCode()] = rotateY;
	//z roate gizmo
	object = new GameObject(utilities::ResourceManager::loadModel("gizmo-r"));
	object->setRotation(glm::vec3(0.0f, glm::radians(90.0f), glm::radians(90.0f)));
	m_gizmosRotate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
	m_gizmoFunctionality[object->getCode()] = rotateZ;
}

bool TranformGizmos::canBeShown(){
	if(*m_pSelectedActor && m_gizmoMode != GizmoMode::NONE)
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
	return (*m_pSelectedActor)->getPosition();
}

void TranformGizmos::updateGizmo(renderer::Camera* camera, utilities::InputManager& input, float deltaTime){
	if(m_currentlyActivated && input.isKeyDown(SDL_BUTTON_LEFT)){
		m_gizmoFunctionality[m_currentlyActivated](*m_pSelectedActor, input, camera, deltaTime);
	} else if(m_currentlyActivated && !input.isKeyPressed(SDL_BUTTON_LEFT)){
		(*m_pSelectedActor)->getPosition().x = std::round((*m_pSelectedActor)->getPosition().x / gridStep) * gridStep;
		(*m_pSelectedActor)->getPosition().y = std::round((*m_pSelectedActor)->getPosition().y / gridStep) * gridStep;
		(*m_pSelectedActor)->getPosition().z = std::round((*m_pSelectedActor)->getPosition().z / gridStep) * gridStep;
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

	float inc = 1.0f;

	//pActor->getPosition().x = std::round(pActor->getPosition().x / inc) * inc;
	//pActor->getPosition().y = std::round(pActor->getPosition().y / inc) * inc;
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
	float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera->getRight());
	if(dotRX >= 0.0f)
		pActor->getRotation().x += input.getMouseDY() * dt;
	else
		pActor->getRotation().x -= input.getMouseDY() * dt;
}

void rotateY(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	pActor->getRotation().y += input.getMouseDX() * dt;
}

void rotateZ(Actor* pActor, utilities::InputManager & input, renderer::Camera* camera, float dt){
	float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera->getRight());
	if(dotRZ >= 0.0f)
		pActor->getRotation().z += input.getMouseDY() * dt;
	else
		pActor->getRotation().z -= input.getMouseDY() * dt;
}
