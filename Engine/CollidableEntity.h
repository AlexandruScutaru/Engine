#ifndef COLLIDABLE_ENTITY_h
#define COLLIDABLE_ENTITY_h

#include "CollisionBody.h"
#include <vector>


namespace renderer{

	class CollidableEntity{
	public:
		CollidableEntity();
		CollidableEntity(const CollidableEntity& other);
		virtual ~CollidableEntity();

		void addColBody(renderer::CollisionBody& body){ m_colBodies.push_back(body); };
		void removeColBody(size_t index);
		std::vector<renderer::CollisionBody>& getColBodies(){ return m_colBodies; }
		void clearColBodies(){ m_colBodies.clear(); }

	protected:
		std::vector<renderer::CollisionBody> m_colBodies;

	};

}

#endif // !COLLIDABLE_ENTITY_h
