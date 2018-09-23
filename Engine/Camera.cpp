#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace renderer{

	Camera::Camera(glm::vec3& position, glm::vec3& up, float pitch, float yaw, float fov) :
		m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_position(position),
		m_worldUp(up),
		m_pitch(pitch),
		m_yaw(yaw),
		m_FOV(fov)
	{
		updateCameraVectors();
	}

	Camera::~Camera(){}

	glm::mat4 Camera::getViewMatrix(){
		return glm::lookAt(m_position, m_position + m_front, m_up);
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
		m_forward = glm::normalize(glm::cross(m_right, m_worldUp));
	}

	void Camera::clampRotation(){
		if(m_pitch > 89.0f)
			m_pitch = 89.0f;
		else if(m_pitch < -89.0f)
			m_pitch = -89.0f;
		while(m_yaw > 180.0f)
			m_yaw -= 360.0f;
		while(m_yaw < -180.0f)
			m_yaw += 360.0f;
	}

}