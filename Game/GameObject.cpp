#include "GameObject.h"

using namespace renderer;


GameObject::GameObject(){}

GameObject::GameObject(TexturedModel* model, bool static_obj) :
	RenderableEntity(model)
{}

GameObject::GameObject(TexturedModel* model, glm::vec3& pos, glm::quat& rot, glm::vec3& scale, bool static_obj) :
	RenderableEntity(model),
	Actor(pos, rot, scale)
{}

GameObject::GameObject(const GameObject& other) :
	Actor(other),
	RenderableEntity(other)
{}

GameObject::~GameObject(){}
