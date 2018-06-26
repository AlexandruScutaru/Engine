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
			   float movementSpeed = 3.5f, float mouseSensitivity = 0.1f, float fov = 70.0f);
		~Camera();

		glm::mat4 getViewMatrix();
		float getFOV(){ return m_FOV; }
		float getPitch(){ return m_pitch; }
		float getYaw(){ return m_yaw; }
		bool getFlashlight(){ return m_flashlight; }
		glm::vec3 getPos(){ return m_position; }
		glm::vec3 getFront(){ return m_front; }
		glm::vec3 getUp(){ return m_up; }
		glm::vec3 backupCameraProperties(){ return glm::vec3(m_FOV, m_pitch, m_yaw); }
		
		void setPitch(float val){ m_pitch = val; }
		void setYaw(float val){ m_yaw = val; }
		void setPos(glm::vec3& pos){ m_position = pos; }
		void restoreCameraProperties(glm::vec3& data){ m_FOV = data.x; m_pitch = data.y; m_yaw = data.z; }

		void update(InputManager& manager, float deltaTime);

	private:
		void updateCameraVectors();
		void incCamSpeed();
		void decCamSpeed();

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
		float m_FOV;
		bool m_flashlight;

	};

}

#endif // !CAMERA_H
