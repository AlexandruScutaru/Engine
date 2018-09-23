#ifndef COLLISION_BODY_H
#define COLLISION_BODY_H

#include <GLM/glm.hpp>


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
			colRot = glm::vec3(0.0f);
			colScale = glm::vec3(1.0f);
			shape = 0;
		}
		int shape;
		glm::vec3 colRelativePos;
		glm::vec3 colRot;
		glm::vec3 colScale;
	};

}

#endif // !COLLISION_BODY_H
