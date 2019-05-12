#ifndef PLAYER_H
#define PLAYER_H

#include <Engine/PhysicsBody.h>

#include <GLM/glm.hpp>

#include <memory>

namespace utilities{
	class InputManager;
}
namespace renderer{
	class Camera;
}


class Player{
public:
	Player();
	Player(glm::vec3& pos, glm::vec3& rot, glm::vec3& scale);
	~Player();

	void setPosition(glm::vec3& pos);
	void setRotation(glm::vec3& rot);
	void setScale(glm::vec3& scale);
	void incPos(float dx, float dy, float dz);
	void incRot(float dx, float dy, float dz);
	void incScale(float dx, float dy, float dz);
	void setCamera(renderer::Camera* camera){ m_camera = camera; }
	void setFlashlight(bool value){ m_flashLightOn = value; }

	glm::vec3& getPosition(){ return m_pos; }
	glm::vec3& getRotation(){ return m_rot; }
	glm::vec3& getScale(){ return m_scale; }
	renderer::Camera* getCamera(){ return m_camera; }
	bool isFlashLightOn(){ return m_flashLightOn; }

	void update(utilities::InputManager& inputManager, float deltaTime);
	void backup();
	void restore();

	void setPhysicsBody(std::shared_ptr<physics::PhysicsBody> body){ m_rigidBody = body; }
	std::shared_ptr<physics::PhysicsBody> getPhysicsBody(){ return m_rigidBody; }

	bool hasKey = false;

private:
	void updateCamera();

	glm::vec3 m_pos;
	glm::vec3 m_rot;
	glm::vec3 m_scale;
	renderer::Camera* m_camera;
	static const glm::vec3 m_cameraOffset;
	bool m_flashLightOn;
	float movementSpeed;

	glm::vec3 m_buPos;
	glm::vec3 m_buRot;
	bool m_buFL;

	std::shared_ptr<physics::PhysicsBody> m_rigidBody;

};

#endif // !PLAYER_H
