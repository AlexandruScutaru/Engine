#ifndef TRANSFORM_GIZMOS_H
#define TRANSFORM_GIZMOS_H

#include <GLM/glm.hpp>

#include <vector>
#include <map>
#include <functional>


namespace renderer{
	class Camera;
}
namespace utilities{
	class InputManager;
}
class Actor;
class GameObject;

enum class GizmoMode{
	NONE=0,
	TRANSLATE,
	SCALE,
	ROTATE
};

struct Gizmo{
	Gizmo(){}
	Gizmo(GameObject* obj, glm::vec3& color):obj(obj), color(color){}
	GameObject* obj;
	glm::vec3 color;
};


class TranformGizmos{
public:
	TranformGizmos();
	~TranformGizmos();
		
	void init(Actor** obj);
	bool canBeShown();
	void updateGizmo(renderer::Camera* camera, utilities::InputManager& input, float deltaTime);
	bool wasClicked(int val);
		
	void setSelectedActor(Actor** obj){ m_pSelectedActor = obj; }
	void setGizmoMode(int mode){ m_gizmoMode = (GizmoMode)mode; }
	void setActivated(int val){ m_currentlyActivated = val; }
		
	const std::vector<Gizmo>* getGizmos();
	const glm::vec3& getPosition();
	int getGizmoMode(){ return (int)m_gizmoMode; }
	int getActivated(){ return m_currentlyActivated; }

	float gridStep;
private:
	Actor** m_pSelectedActor;
	GizmoMode m_gizmoMode;
	std::vector<Gizmo> m_gizmosTranslate;
	std::vector<Gizmo> m_gizmosScale;
	std::vector<Gizmo> m_gizmosRotate;
	std::map<int, std::function<void(Actor*, utilities::InputManager&, renderer::Camera*, float)>> m_gizmoFunctionality;
	int m_currentlyActivated;

};

#endif // !TRANSFORM_GIZMOS_H