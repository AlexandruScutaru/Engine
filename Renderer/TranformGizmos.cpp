#include "TranformGizmos.h"
#include "ResourceManager.h"
#include "RenderableEntity.h"
#include "InputManager.h"
#include "Camera.h"


namespace renderer{

	static void translateX	(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void translateY	(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void translateZ	(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void translateXY	(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void translateXZ	(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void translateYZ	(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);

	static void scaleX		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void scaleY		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void scaleZ		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void scaleXY		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void scaleXZ		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void scaleYZ		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void scaleXYZ	(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);

	static void rotateX		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void rotateY		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);
	static void rotateZ		(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt);


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


	void TranformGizmos::init(RenderableEntity** obj){
		m_currentlyActivated = 0;
		m_pSelectedGameObject = obj;
		m_gizmoMode = GizmoMode::TRANSLATE;
		RenderableEntity* object;

		///translate
		//x translate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-t1a"));
		object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = translateX;
		//xy translate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-2a"));
		object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
		m_gizmoFunctionality[object->getCode()] = translateXY;

		//y translate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-t1a"));
		object->setRotation(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)));
		m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = translateY;
		//xz translate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-2a"));
		object->setRotation(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));
		m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = translateXZ;

		//z translate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-t1a"));
		object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
		m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
		m_gizmoFunctionality[object->getCode()] = translateZ;

		//yz translate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-2a"));
		object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
		m_gizmosTranslate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = translateYZ;

		///scale
		//x scale gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-s1a"));
		object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = scaleX;
		//xy scale gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-2a"));
		object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
		m_gizmoFunctionality[object->getCode()] = scaleXY;

		//y scale gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-s1a"));
		object->setRotation(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)));
		m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = scaleY;
		//xz scale gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-2a"));
		object->setRotation(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));
		m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = scaleXZ;

		//z scale gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-s1a"));
		object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
		m_gizmosScale.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
		m_gizmoFunctionality[object->getCode()] = scaleZ;
		//yz scale gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-2a"));
		object->setRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
		m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = scaleYZ;

		//xyz scale gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-s3a"));
		object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		m_gizmosScale.push_back(Gizmo(object, glm::vec3(1.0f, 1.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = scaleXYZ;

		///rotate
		//x roate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-r"));
		object->setRotation(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)));
		m_gizmosRotate.push_back(Gizmo(object, glm::vec3(1.0f, 0.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = rotateX;
		//y rotate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-r"));
		object->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		m_gizmosRotate.push_back(Gizmo(object, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_gizmoFunctionality[object->getCode()] = rotateY;
		//z roate gizmo
		object = new RenderableEntity(ResourceManager::loadModel("gizmo-r"));
		object->setRotation(glm::vec3(0.0f, glm::radians(90.0f), glm::radians(90.0f)));
		m_gizmosRotate.push_back(Gizmo(object, glm::vec3(0.0f, 0.0f, 1.0f)));
		m_gizmoFunctionality[object->getCode()] = rotateZ;
	}

	bool TranformGizmos::canBeShown(){
		if(*m_pSelectedGameObject && m_gizmoMode != GizmoMode::NONE)
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
		return (*m_pSelectedGameObject)->getPosition();
	}

	void TranformGizmos::updateGizmo(Camera& camera, InputManager& input, float deltaTime){
		if(m_currentlyActivated && input.isKeyDown(SDL_BUTTON_LEFT))
			m_gizmoFunctionality[m_currentlyActivated](*m_pSelectedGameObject, input, camera, deltaTime);
		else
			m_currentlyActivated = 0;
	}

	bool TranformGizmos::wasClicked(int val){
		auto it = m_gizmoFunctionality.find(val);
		if(it != m_gizmoFunctionality.end())
			return true;
		return false;
	}


	//function definitions
	void translateX(RenderableEntity* pObj, InputManager& input, Camera& camera, float dt){
		float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getRight());
		float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getUp());
		
		if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
			if(dotUX > 0.0f)
				pObj->getPosition().x -= input.getMouseDY()*dt; //up
			else
				pObj->getPosition().x += input.getMouseDY()*dt; //down
		else if(dotRX > 0.0f)
			pObj->getPosition().x += input.getMouseDX()*dt; //right
		else
			pObj->getPosition().x -= input.getMouseDX()*dt; //left
	}

	void translateY(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		pObj->getPosition().y -= input.getMouseDY()*dt;
	}

	void translateZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getRight());
		float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getUp());

		if(-0.25f <= dotRZ && dotRZ <= 0.25f) //up or down
			if(dotUZ > 0.0f)
				pObj->getPosition().z -= input.getMouseDY()*dt; //up
			else
				pObj->getPosition().z += input.getMouseDY()*dt; //down
		else if(dotRZ > 0.0f)
			pObj->getPosition().z += input.getMouseDX()*dt; //right
		else
			pObj->getPosition().z -= input.getMouseDX()*dt; //left
	}

	void translateXY(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getRight());
		float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getUp());

		if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
			if(dotUX > 0.0f)
				pObj->getPosition().x -= input.getMouseDY()*dt; //up
			else
				pObj->getPosition().x += input.getMouseDY()*dt; //down
		else if(dotRX > 0.0f){
			pObj->getPosition().x += input.getMouseDX()*dt; //right
			pObj->getPosition().y -= input.getMouseDY()*dt;
		} else{
			pObj->getPosition().x -= input.getMouseDX()*dt; //left
			pObj->getPosition().y -= input.getMouseDY()*dt;
		}
	}

	void translateXZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getRight());
		float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getUp());

		float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getRight());
		float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getUp());

		if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
			if(dotUX > 0.0f){
				pObj->getPosition().x -= input.getMouseDY()*dt; //up
				if(dotRZ > 0.0f)
					pObj->getPosition().z += input.getMouseDX()*dt; //right
				else
					pObj->getPosition().z -= input.getMouseDX()*dt; //left

			} else{
				pObj->getPosition().x += input.getMouseDY()*dt; //down
				if(dotRZ > 0.0f)
					pObj->getPosition().z += input.getMouseDX()*dt; //right
				else
					pObj->getPosition().z -= input.getMouseDX()*dt; //left
			}
		else if(dotRX > 0.0f){
			pObj->getPosition().x += input.getMouseDX()*dt; //right
			if(dotUZ > 0.0f)
				pObj->getPosition().z -= input.getMouseDY()*dt; //up
			else
				pObj->getPosition().z += input.getMouseDY()*dt; //down
		} else{
			pObj->getPosition().x -= input.getMouseDX()*dt; //left
			if(dotUZ > 0.0f)
				pObj->getPosition().z -= input.getMouseDY()*dt; //up
			else
				pObj->getPosition().z += input.getMouseDY()*dt; //down
		}
	}

	void translateYZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getRight());
		float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getUp());

		if(-0.25f <= dotRZ && dotRZ <= 0.25f) //up or down
			if(dotUZ > 0.0f)
				pObj->getPosition().z -= input.getMouseDY()*dt; //up
			else
				pObj->getPosition().z += input.getMouseDY()*dt; //down
		else if(dotRZ > 0.0f){
			pObj->getPosition().z += input.getMouseDX()*dt; //right
			pObj->getPosition().y -= input.getMouseDY()*dt;
		} else{
			pObj->getPosition().z -= input.getMouseDX()*dt; //left
			pObj->getPosition().y -= input.getMouseDY()*dt;
		}
	}

	void scaleX(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getRight());
		float dotUX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getUp());

		if(-0.25f <= dotRX && dotRX <= 0.25f) // up or down
			if(dotUX > 0.0f)
				pObj->getScale().x -= input.getMouseDY()*dt; //up
			else
				pObj->getScale().x += input.getMouseDY()*dt; //down
		else if(dotRX > 0.0f)
			pObj->getScale().x += input.getMouseDX()*dt; //right
		else
			pObj->getScale().x -= input.getMouseDX()*dt; //left
	}

	void scaleY(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		pObj->getScale().y -= input.getMouseDY()*dt;
	}

	void scaleZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getRight());
		float dotUZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getUp());

		if(-0.25f <= dotRZ && dotRZ <= 0.25f) //up or down
			if(dotUZ > 0.0f)
				pObj->getScale().z -= input.getMouseDY()*dt; //up
			else
				pObj->getScale().z += input.getMouseDY()*dt; //down
		else if(dotRZ > 0.0f)
			pObj->getScale().z += input.getMouseDX()*dt; //right
		else
			pObj->getScale().z -= input.getMouseDX()*dt; //left
	}

	void scaleXY(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float d = (input.getMouseDX() - input.getMouseDY()) / 2.0f;
		pObj->getScale().x += d *dt;
		pObj->getScale().y += d *dt;
	}

	void scaleXZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float d = (input.getMouseDX() - input.getMouseDY()) / 2.0f;
		pObj->getScale().x += d *dt;
		pObj->getScale().z += d *dt;
	}

	void scaleYZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float d = (input.getMouseDX() - input.getMouseDY()) / 2.0f;
		pObj->getScale().y += d *dt;
		pObj->getScale().z += d *dt;
	}

	void scaleXYZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		pObj->setScale(pObj->getScale() + glm::vec3(input.getMouseDX()*dt));
	}

	void rotateX(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRX = glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camera.getRight());
		if(dotRX >= 0.0f)
			pObj->getRotation().x += input.getMouseDY() * dt;
		else
			pObj->getRotation().x -= input.getMouseDY() * dt;
	}

	void rotateY(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		pObj->getRotation().y += input.getMouseDX() * dt;
	}

	void rotateZ(RenderableEntity * pObj, InputManager & input, Camera & camera, float dt){
		float dotRZ = glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), camera.getRight());
		if(dotRZ >= 0.0f)
			pObj->getRotation().z += input.getMouseDY() * dt;
		else
			pObj->getRotation().z -= input.getMouseDY() * dt;
	}

}