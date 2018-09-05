#include "CollidableEntity.h"

namespace renderer{

	CollidableEntity::CollidableEntity(){}

	CollidableEntity::~CollidableEntity(){}


	void CollidableEntity::removeColBody(size_t index){
		m_colBodies.erase(m_colBodies.begin() + index);
	}

}