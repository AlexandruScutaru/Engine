#ifndef COLLISION_BODY_H
#define COLLISION_BODY_H

#include <GLM/glm.hpp>
#include <GLM/gtx/quaternion.hpp>

namespace renderer{

	enum CollisionShapes{
		SHAPE_CUBE = 0,
		SHAPE_SPHERE,
		SHAPE_CILINDER,
		SHAPE_CONE,
		SHAPE_CAPSULE
	};

	struct CollisionBody{
		CollisionBody(){
			colRelativePos = glm::vec3(0.0f);
			colRotQuat = glm::quat();
			colRotEuler = glm::vec3();
			colScale = glm::vec3(1.0f);
			shape = 0;
			mass = 1.0f;
		}
		void updateQuaternion(){
			colRotQuat = glm::quat(glm::vec3(glm::radians(colRotEuler.x), glm::radians(colRotEuler.y), glm::radians(colRotEuler.z)));
		}
		int shape;
		glm::vec3 colRelativePos;
		glm::quat colRotQuat;
		glm::vec3 colRotEuler;
		glm::vec3 colScale;
		float mass;

	};

}

#endif // !COLLISION_BODY_H
