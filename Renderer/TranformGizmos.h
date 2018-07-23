#ifndef TRANSFORM_GIZMOS_H
#define TRANSFORM_GIZMOS_H

#include <GLM/glm.hpp>
#include <vector>
#include <map>
#include <functional>


namespace renderer{

	class RenderableEntity;
	class InputManager;
	class Camera;

	enum class GizmoMode{
		NONE=0,
		TRANSLATE,
		SCALE,
		ROTATE
	};

	struct Gizmo{
		Gizmo(){}
		Gizmo(RenderableEntity* obj, glm::vec3& color):obj(obj), color(color){}
		RenderableEntity* obj;
		glm::vec3 color;
	};


	class TranformGizmos{
	public:
		TranformGizmos();
		~TranformGizmos();
		
		void init(RenderableEntity** obj);
		bool canBeShown();
		void updateGizmo(Camera& camera, InputManager& input, float deltaTime);
		bool wasClicked(int val);
		
		void setSelectedGameObject(RenderableEntity** obj){ m_pSelectedGameObject = obj; }
		void setGizmoMode(int mode){ m_gizmoMode = (GizmoMode)mode; }
		void setActivated(int val){ m_currentlyActivated = val; }
		
		const std::vector<Gizmo>* getGizmos();
		const glm::vec3& getPosition();
		int getGizmoMode(){ return (int)m_gizmoMode; }
		int getActivated(){ return m_currentlyActivated; }

	private:
		RenderableEntity** m_pSelectedGameObject;
		GizmoMode m_gizmoMode;
		std::vector<Gizmo> m_gizmosTranslate;
		std::vector<Gizmo> m_gizmosScale;
		std::vector<Gizmo> m_gizmosRotate;
		std::map<int, std::function<void(RenderableEntity*, InputManager&, Camera&, float)>> m_gizmoFunctionality;
		int m_currentlyActivated;

	};

}

#endif // !TRANSFORM_GIZMOS_H