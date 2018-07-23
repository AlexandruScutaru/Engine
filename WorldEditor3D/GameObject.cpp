#include "GameObject.h"

using namespace renderer;


GameObject::GameObject(){}

GameObject::GameObject(TexturedModel* model, bool static_obj) :
	RenderableEntity(model, static_obj)
{}

GameObject::GameObject(TexturedModel* model,
					   glm::vec3& pos,
					   glm::vec3& rot,
					   glm::vec3& scale,
					   bool static_obj) :
	RenderableEntity(model,pos,rot,scale,static_obj)
{}

GameObject::GameObject(const GameObject& other) :
	RenderableEntity(other)
{}

GameObject::~GameObject(){}