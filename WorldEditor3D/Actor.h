#ifndef ACTOR_H
#define ACTOR_H

#include <GLM/glm.hpp>
#include <GLM/gtx/quaternion.hpp>


class Actor{
public:
	Actor();
	Actor(glm::vec3& pos, glm::quat& rot, glm::vec3& scale);
	Actor(const Actor& other);
	virtual ~Actor();

	virtual void update(float deltaTime){}
	virtual void draw(){}

	void setPosition(glm::vec3& pos){ m_pos = pos; }
	void setRotation(glm::quat& rot){ m_rot = rot; }
	void setScale(glm::vec3& scale){ m_scale = scale; }

	glm::vec3& getPosition(){ return m_pos; }
	glm::quat& getRotation(){ return m_rot; }
	glm::vec3& getScale(){ return m_scale; }
	int getCode(){ return m_code; }

protected:
	int m_code;
	glm::vec3 m_pos;
	glm::quat m_rot;
	glm::vec3 m_scale;

	static int objectCount;
};

#endif // !ACTOR_H
