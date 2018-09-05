#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <Engine/RenderableEntity.h>
#include <Engine/CollidableEntity.h>
#include "Actor.h"


class GameObject : public Actor, public renderer::RenderableEntity, public renderer::CollidableEntity {
public:
	GameObject();
	GameObject(renderer::TexturedModel* model,
			   bool static_obj
	);
	GameObject(renderer::TexturedModel* model,
			   glm::vec3& pos = glm::vec3(0.0f),
			   glm::vec3& rot = glm::vec3(0.0f),
			   glm::vec3& scale = glm::vec3(1.0f),
			   bool static_obj = false
	);
	GameObject(const GameObject& other);
	~GameObject();

	void setDiffName(const std::string& diff){ m_diffName = diff; }
	void setSpecName(const std::string& spec){ m_specName = spec; }
	void setMeshName(const std::string& mesh){ m_meshName = mesh; }
	void setIsBillboard(bool isBillboard){ m_model->setBillboard(isBillboard); }
	
	void setIsStatic(bool isStatic){ m_isStatic = isStatic; }
	void setName(const std::string& name){ m_name = name; }
	void setInEditorName(const std::string& inEditorName){ m_inEditorName = inEditorName; }
	void setSelected(bool selected){ m_selected = selected; }

	bool isStatic(){ return m_isStatic; }
	bool isSelected(){ return m_selected; }
	bool isBillboard(){ return m_model->isBillboard(); }
	bool& isBillboardRef(){ return m_model->isBillboardRef(); }
	const std::string& getName(){ return m_name; }
	const std::string& getInEditorName(){ return m_inEditorName; }
	const std::string& getDiffName(){ return m_diffName; }
	const std::string& getSpecName(){ return m_specName; }
	const std::string& getMeshName(){ return m_meshName; }

private:
	std::string m_name;
	std::string m_inEditorName;
	std::string m_diffName;
	std::string m_specName;
	std::string m_meshName;

	bool m_isStatic;
	bool m_selected;

};
#endif // !GAMEOBJECT_H
