#ifndef CAMERA_H
#define CAMERA_H

#include <GLM/glm.hpp>

namespace renderer{

	class InputManager;

	enum Camera_Movement{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	class Camera{
	public:
		Camera(glm::vec3& position = glm::vec3(0.0f), glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
			   float pitch = 0.0f, float yaw = -90.0f, 
			   float movementSpeed = 3.5f, float mouseSensitivity = 0.1f, float zoom = 70.0f);
		~Camera();

		glm::mat4 getViewMatrix();
		float getZoom(){ return m_zoom; }
		glm::vec3 getPos(){ return m_position; }
		glm::vec3 getFront(){ return m_front; }
		glm::vec3 getUp(){ return m_up; }

		void update(InputManager& manager, float deltaTime);

	private:
		void updateCameraVectors();

		// Camera Attributes
		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldUp;
		// Euler Angles
		float m_pitch;
		float m_yaw;
		// Camera options
		float m_movementSpeed;
		float m_mouseSensitivity;
		float m_zoom;

	};

}

#endif // !CAMERA_H
