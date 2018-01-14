#ifndef ENTITY_H
#define ENTITY_H

#include "GameObject.h"

namespace renderer{

	class Entity : public GameObject{
	public:
		Entity();
		Entity(unsigned int texturedModel);
		~Entity();

		void update(float deltaTime);
		void prepareToRender();
		void render();

	private:

	};

}

#endif // !ENTITY_H
