#include "CollisionVolume.h"


using namespace renderer;

CollisionVolume::CollisionVolume(glm::vec3 & pos) :
	Actor(pos, glm::quat(), glm::vec3(1.0f))
{}

CollisionVolume::CollisionVolume(const CollisionVolume & other) :
	Actor(other)
{}

CollisionVolume::~CollisionVolume() {}
