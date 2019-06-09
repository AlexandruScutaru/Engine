#ifndef CAMERA_H
#define CAMERA_H

#include <GLM/glm.hpp>


namespace renderer{

	class Camera{
	public:
		//Camera();
		Camera(glm::vec3& position = glm::vec3(0.0f), glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
			   float pitch = 0.0f, float yaw = -90.0f, float fov = 80.0f);
		~Camera();

		void setPitch(float val){ m_pitch = val; clampRotation(); }
		void setYaw(float val){ m_yaw = val; clampRotation(); }
		void setPos(glm::vec3& pos){ m_position = pos; }
		void incPos(float dx, float dy, float dz){ m_position.x = dx; m_position.y = dy; m_position.z = dz; }
		void incRot(float dPitch, float dYaw){ m_pitch += dPitch; m_yaw += dYaw; clampRotation(); }

		float getFOV(){ return m_FOV; }
		float getPitch(){ return m_pitch; }
		float getYaw(){ return m_yaw; }
		glm::mat4 getViewMatrix();
		glm::vec3 getPos(){ return m_position; }
		glm::vec3 getFront(){ return m_front; }
		glm::vec3 getForward(){ return m_forward; }
		glm::vec3 getUp(){ return m_up; }
		glm::vec3 getRight(){ return m_right; }

		void updateCameraVectors();

	private:
		void clampRotation();

		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_forward;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldUp;
		
		float m_pitch;
		float m_yaw;
		float m_FOV;

	};

}

#endif // !CAMERA_H
