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

	virtual void setPosition(glm::vec3& pos){ m_pos = pos; }
	virtual void setRotation(glm::quat& rot){ m_rot = rot; }
	virtual void setScale(glm::vec3& scale){ m_scale = scale; }

	void setAtlasIndex(int index){ m_atlasIndex = index; }
	
	glm::vec3& getPosition(){ return m_pos; }
	glm::quat& getRotation(){ return m_rot; }
	glm::vec3& getScale(){    return m_scale; }
	int getAtlasIndex(){ return m_atlasIndex; }
	int getCode(){ return m_code; }

protected:
	glm::vec3 m_pos;
	glm::quat m_rot;
	glm::vec3 m_scale;
	int m_code;
	static int objectCount;
	int m_atlasIndex;

};

#endif // !ACTOR_H
