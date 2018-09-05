#ifndef COLLISION_BODY_H
#define COLLISION_BODY_H

#include <GLM/glm.hpp>


namespace renderer{
	struct CollisionBody{
		CollisionBody(){
			colRelativePos = glm::vec3(0.0f);
			colRot = glm::vec3(0.0f);
			colScale = glm::vec3(1.0f);
		}
		int shape;
		glm::vec3 colRelativePos;
		glm::vec3 colRot;
		glm::vec3 colScale;
	};
}

#endif // !COLLISION_BODY_H
