#ifndef ACTOR_H
#define ACTOR_H


#include <GLM/glm.hpp>
#include <GLM/gtx/quaternion.hpp>


namespace renderer{
	class TexturedModel;
}

class Actor{
public:
	Actor();
	Actor(renderer::TexturedModel* model);
	Actor(renderer::TexturedModel* model, glm::vec3& pos, glm::quat& rot, glm::vec3& scale);
	Actor(const Actor& other);
	virtual ~Actor();

	virtual void update(float deltaTime){}

	virtual void setPosition(glm::vec3& pos){ m_pos = pos; }
	void setRotation(glm::quat& rot){ m_rot = rot; }
	void setScale(glm::vec3& scale){ m_scale = scale; }
	void setIsBillboard(bool isBillboard);
	void setTexturedModel(renderer::TexturedModel* model);
	void setSelected(bool selected){ m_selected = selected; }
	void setDoubleSided(bool doubleSided);
	void setAtlasIndex(int index){ m_atlasIndex = index; }

	renderer::TexturedModel* getTexturedModel(){ return m_model; }
	bool isSelected(){ return m_selected; }
	bool isDoubleSided();
	bool& isDoubleSidedRef();
	bool isBillboard();
	bool& isBillboardRef();
	int getAtlasIndex(){ return m_atlasIndex; }
	int& getAtlasIndexRef(){ return m_atlasIndex; }
	glm::vec3& getPosition(){ return m_pos; }
	glm::quat& getRotation(){ return m_rot; }
	glm::vec3& getScale(){ return m_scale; }
	int getCode(){ return m_code; }

	glm::vec2 getTextureOffset();

protected:
	int m_code;
	glm::vec3 m_pos;
	glm::quat m_rot;
	glm::vec3 m_scale;
	renderer::TexturedModel* m_model;
	bool m_selected;
	static int objectCount;
	int m_atlasIndex;
};

#endif // !ACTOR_H
