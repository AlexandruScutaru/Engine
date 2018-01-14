#include "Camera.h"
#include "InputManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL.h>

#define SCROLL_SENSITIVITY  2.0f;

namespace renderer{

	Camera::Camera(glm::vec3& position, glm::vec3& up, float pitch, float yaw,
				   float movementSpeed, float mouseSensitivity, float fov) :
		m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_position(position),
		m_worldUp(up),
		m_pitch(pitch),
		m_yaw(yaw),
		m_movementSpeed(movementSpeed),
		m_mouseSensitivity(mouseSensitivity),
		m_FOV(fov)
	{
		updateCameraVectors();
	}

	Camera::~Camera(){}


	glm::mat4 Camera::getViewMatrix(){
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void Camera::update(InputManager& manager, float deltaTime){
		//camera movement
		float cameraSpeed = m_movementSpeed * deltaTime;
		if(manager.isKeyDown(SDLK_w))
			m_position += cameraSpeed * m_front;
		else if(manager.isKeyDown(SDLK_s))
			m_position -= cameraSpeed * m_front;

		if(manager.isKeyDown(SDLK_a))
			m_position -= glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;
		else if(manager.isKeyDown(SDLK_d))
			m_position += glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;

		if(manager.isKeyDown(SDLK_q))
			m_position -= cameraSpeed * m_up;
		else if(manager.isKeyDown(SDLK_e))
			m_position += cameraSpeed * m_up;

		//if(manager.isKeyPressed(SDL_BUTTON_RIGHT))
		//	manager.setOldMouseCoords(manager.getMouseCoords().x, manager.getMouseCoords().y);

		//camera fov
		//if(m_FOV >= 1.0f && m_FOV <= 75.0f)
		//	m_FOV -= manager.getMouseWheel() * SCROLL_SENSITIVITY;
		//if(m_FOV <= 1.0f)
		//	m_FOV = 1.0f;
		//if(m_FOV >= 75.0f)
		//	m_FOV = 75.0f;

		//camera rotation
		//for now it is only in edit mode so Right Mouse Button needs to be pressed in order to rotate the camera
		if(manager.isKeyDown(SDL_BUTTON_RIGHT)){
			m_pitch += manager.getMouseDY() * m_mouseSensitivity;
			m_yaw -= manager.getMouseDX() * m_mouseSensitivity;

			if(m_pitch > 89.0f)
				m_pitch = 89.0f;
			if(m_pitch < -89.0f)
				m_pitch = -89.0f;
		}
		updateCameraVectors();
	}

	void Camera::updateCameraVectors(){
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}

}