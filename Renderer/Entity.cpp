#include "Entity.h"

namespace renderer{

	Entity::Entity(){}

	Entity::Entity(unsigned int texturedModel) :
		GameObject(texturedModel)
	{}

	Entity::~Entity(){}

	void Entity::update(float deltaTime){}

	void Entity::prepareToRender(){}

	void Entity::render(){}

}