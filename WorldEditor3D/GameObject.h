#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <Renderer/Renderer.h>

class GameObject : public renderer::RenderableEntity {
public:
	GameObject();
	GameObject(renderer::TexturedModel* model,
			   bool static_obj
	);
	GameObject(renderer::TexturedModel* model,
			   glm::vec3& pos = glm::vec3(0.0f),
			   glm::vec3& rot = glm::vec3(0.0f),
			   glm::vec3& scale = glm::vec3(1.0f),
			   bool static_obj = false
	);
	GameObject(const GameObject& other);

	~GameObject();

private:

};
#endif // !GAMEOBJECT_H
